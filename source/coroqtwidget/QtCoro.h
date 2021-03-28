#pragma once

#include <coroutine>
#include <iostream>
#include <memory>

#include "meta.h"

namespace qtcoro 
{
   template<typename T=void>
   struct return_object
   {
      struct promise_type;
      using handle_type = std::coroutine_handle<promise_type>;

      return_object(promise_type& p)
         : m_coro(handle_type::from_promise(p)) {}

      return_object(return_object const &) = delete;
      return_object(return_object&& other)
      {
         m_coro = other.m_coro;
         other.m_coro = nullptr;
      }

      ~return_object() {
         if (m_coro) m_coro.destroy();
      }

      //T get() {
      //   std::cout << "    MyFuture::get" << '\n';
      //   m_coro.resume();                                              // (6)
      //   return m_coro.promise().result;
      //}

      struct promise_type 
      {
         auto initial_suspend() const noexcept {
            return std::suspend_never();
         }
         auto final_suspend() const noexcept {
            return std::suspend_always();
         }

         return_object get_return_object() {
            return return_object{ *this };
            //return return_object{ handle_type::from_promise(*this) };
         }

         void return_void() const noexcept {}
         //void return_value(T v) {
         //   std::cout << "        promise_type::return_value" << '\n';
         //}

         void unhandled_exception() {
            std::exit(1);
         }
      };
   
   private:
      handle_type m_coro;
   };


   template<typename Result, typename... Args>
   struct make_slot;

   // for two or more arguments we supply a std::tuple from the awaiter
   template<typename... Args>
   struct make_slot<std::tuple<Args...>, Args...> 
   {
      using Result = std::tuple<Args...>;
      auto operator() (QMetaObject::Connection& signal_conn, Result& result, std::coroutine_handle<>& coro_handle) 
      {
         return [&signal_conn, &coro_handle, &result] (Args... a) 
         {
            // all our awaits are one-shot, so we immediately disconnect
            QObject::disconnect(signal_conn);
            // put the result where the awaiter can supply it from await_resume()
            result = std::make_tuple(a...);
            // resume execution inside the coroutine at the co_await
            coro_handle.resume();
         };
      }
   };


   // for a single argument it's just that particular type
   template<typename Arg>
   struct make_slot<Arg, Arg> 
   {
      auto operator() (QMetaObject::Connection& signal_conn, Arg& result, std::coroutine_handle<>& coro_handle) 
      {
         return [&signal_conn, &coro_handle, &result] (Arg a) {
            QObject::disconnect(signal_conn);
            result = a;
            coro_handle.resume();
         };
      }
   };

   // no argument - result is void, don't set anything
   template<>
   struct make_slot<void>
   {
      auto operator() (QMetaObject::Connection& signal_conn, std::coroutine_handle<>& coro_handle) 
      {
         return [&signal_conn, &coro_handle]() 
         {
            QObject::disconnect(signal_conn);
            coro_handle.resume();
         };
      }
   };

   //
   // Awaitable class
   //

   // first, a special base to handle possibly nullary signals
   template<typename Derived, typename Result>
   struct awaitable_signal_base 
   {
      // not nullary - we have a real value to set when the signal arrives
      template<typename Object, typename... Args>
      awaitable_signal_base(Object* source, void (Object::* method)(Args...), std::coroutine_handle<>& coro_handle) 
      {
         signal_conn_ = QObject::connect(source, method,
               make_slot<Result, Args...>()(signal_conn_, derived()->signal_args_, coro_handle));

      }

      Derived* derived() { return static_cast<Derived*>(this); }

   protected:
      Result signal_args_;
      QMetaObject::Connection signal_conn_;
   };

   template<typename Derived>
   struct awaitable_signal_base<Derived, void> 
   {
      // nullary, i.e., no arguments to signal and nothing to supply to co_await
      template<typename Object, typename... Args>
      awaitable_signal_base(Object* source, void (Object::* method)(Args...),
         std::coroutine_handle<>& coro_handle) 
      {
         // hook up the slot version that doesn't try to store signal args
         signal_conn_ = QObject::connect(source, method,
            make_slot<void>()(signal_conn_, coro_handle));
      }

   protected:
      QMetaObject::Connection signal_conn_;
      // no need to store signal arguments since there are none
   };


   // forward reference for our "signal args -> co_await result" TMP code
   template<typename F>
   struct signal_args_t;

   // The rest of our awaitable
   template<typename Signal, typename Result = typename signal_args_t<Signal>::type>
   struct awaitable_signal : awaitable_signal_base<awaitable_signal<Signal, Result>, Result> 
   {
      using obj_t = typename member_fn_t<Signal>::cls_t;

      awaitable_signal(obj_t* src, Signal method)
         : awaitable_signal_base<awaitable_signal, Result>(src, method, m_coro_handle) {}

      struct awaiter {
         awaiter(awaitable_signal* awaitable) : awaitable_(awaitable) {}

         bool await_ready() const noexcept {
            return false;  // we are waiting for the signal to arrive
         }

         template<typename P>
         void await_suspend(std::coroutine_handle<P> handle) noexcept 
         {
            // we have now been suspended but are able to do something before
            // returning to caller-or-resumer
            // such as storing the coroutine handle!
            awaitable_->m_coro_handle = handle;    // store for later resumption
         }

         template<typename R = Result>
         typename std::enable_if_t<!std::is_same_v<R, void>, R>
            await_resume() noexcept {
            return awaitable_->signal_args_;
         }

         template<typename R = Result>
         typename std::enable_if_t<std::is_same_v<R, void>, void>
            await_resume() noexcept {}

      private:
         awaitable_signal* awaitable_;

      };

      awaiter operator co_await () { return awaiter{ this }; }

   private:
      std::coroutine_handle<> m_coro_handle;

   };

   template<typename T, typename F>
   awaitable_signal<F> make_awaitable_signal(T* t, F fn) 
   {
      return awaitable_signal<F>{t, fn};
   }

   //
   // some light metaprogramming
   //

   // deduce the type we want to return from co_await from the signal's signature
   // result of co_await should be void, one value, or a tuple of values
   // depending on how many parameters the signal has

   // produce void or T for small tuples
   template<typename T>
   struct special_case_tuple 
   {
      using type = T;
   };

   // just one type
   template<typename T>
   struct special_case_tuple<std::tuple<T>> 
   {
      using type = T;
   };

   // empty list
   template<>
   struct special_case_tuple<std::tuple<>> 
   {
      using type = void;
   };

   //
   // Use a simple predicate and the filter metafunction
   // to make a list of non-empty types
   //

   template<typename T>
   using not_empty = std::negation<std::is_empty<T>>;

   template<typename Sequence>
   struct filter_empty_types {
      using type = typename filter<not_empty, Sequence>::type;
   };

   // now put it all together:

   template<typename F>
   struct signal_args_t {
      // get argument list
      using args_t = typename member_fn_t<F>::arglist_t;
      using classname_t = typename member_fn_t<F>::cls_t;
      // remove any empty (including "QPrivateSignal") parameters from the list
      using no_empty_t = typename filter_empty_types<args_t>::type;
      // apply std::decay_t to all arg types
      using decayed_args_t = typename apply_to_tuple<std::decay_t, no_empty_t>::type;
      // special case 0 and 1 argument
      using type = typename special_case_tuple<decayed_args_t>::type;
   };

}

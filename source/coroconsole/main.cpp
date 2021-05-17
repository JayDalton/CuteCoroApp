#include "common.h"

//#include "stdafx.h"

#include <iomanip>
#include <iostream>
#include <numbers>
#include <variant>
#include <coroutine>
#include <memory>

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include <QtWidgets/QApplication>

#include <QByteArray>
#include <QDebug>
#include <QString>
#include <QVector>
#include <QVariant>
#include <QXmlStreamWriter>

template<typename T>
struct MyEagerFuture 
{
   std::shared_ptr<T> value;

   MyEagerFuture(std::shared_ptr<T> p) : value(p) 
   {                         // (3)
      std::cout << "    MyFuture::MyFuture" << '\n';
   }

   ~MyEagerFuture() {
      std::cout << "    MyFuture::~MyFuture" << '\n';
   }

   T get() {
      std::cout << "    MyFuture::get" << '\n';
      return *value;
   }

   struct promise_type 
   {                                              // (4)
      std::shared_ptr<T> ptr = std::make_shared<T>();                // (11)
      
      promise_type() {
         std::cout << "        promise_type::promise_type" << '\n';
      }
      ~promise_type() {
         std::cout << "        promise_type::~promise_type" << '\n';
      }

      MyEagerFuture<T> get_return_object() {
         std::cout << "        promise_type::get_return_object" << '\n';
         return ptr;
      }

      void return_value(T v) {
         std::cout << "        promise_type::return_value" << '\n';
         *ptr = v;
      }
      std::suspend_never initial_suspend() {                         // (6)
         std::cout << "        promise_type::initial_suspend" << '\n';
         return {};
      }
      std::suspend_never final_suspend() noexcept {                 // (7)
         std::cout << "        promise_type::final_suspend" << '\n';
         return {};
      }
      void unhandled_exception() {
         std::exit(1);
      }
   };                                                                 // (5)
};

template<typename T>
struct MyLazyFuture 
{
   struct promise_type;
   using handle_type = std::coroutine_handle<promise_type>;

   handle_type coro;                                               // (5)

   MyLazyFuture(handle_type h) : coro(h) {
      std::cout << "    MyFuture::MyFuture" << '\n';
   }
   ~MyLazyFuture() {
      std::cout << "    MyFuture::~MyFuture" << '\n';
      if (coro) coro.destroy();                                 // (8)
   }

   T get() {
      std::cout << "    MyFuture::get" << '\n';
      coro.resume();                                              // (6)
      return coro.promise().result;
   }

   struct promise_type 
   {
      T result;

      promise_type() {
         std::cout << "        promise_type::promise_type" << '\n';
      }
      ~promise_type() {
         std::cout << "        promise_type::~promise_type" << '\n';
      }
      auto get_return_object() {                                  // (3)
         std::cout << "        promise_type::get_return_object" << '\n';
         return MyLazyFuture{ handle_type::from_promise(*this) };
      }
      void return_value(T v) {
         std::cout << "        promise_type::return_value" << '\n';
         result = v;
      }
      std::suspend_always initial_suspend() {                    // (1)
         std::cout << "        promise_type::initial_suspend" << '\n';
         return {};
      }
      std::suspend_always final_suspend() noexcept {            // (2)
         std::cout << "        promise_type::final_suspend" << '\n';
         return {};
      }
      void unhandled_exception() {
         std::exit(1);
      }
   };
};

template<typename T>
struct MyThreadFuture 
{
   struct promise_type;
   using handle_type = std::coroutine_handle<promise_type>;
   handle_type coro;

   MyThreadFuture(handle_type h) : coro(h) {}

   ~MyThreadFuture() {
      if (coro) coro.destroy();
   }

   T get() {                                           // (1)
      std::cout << "    MyFuture::get:  "
         << "std::this_thread::get_id(): "
         << std::this_thread::get_id() << '\n';

      std::thread t([this] { coro.resume(); });       // (2)
      t.join();
      return coro.promise().result;
   }

   struct promise_type 
   {
      promise_type() 
      {
         std::cout << "        promise_type::promise_type:  "
            << "std::this_thread::get_id(): "
            << std::this_thread::get_id() << '\n';
      }
      ~promise_type() 
      {
         std::cout << "        promise_type::~promise_type:  "
            << "std::this_thread::get_id(): "
            << std::this_thread::get_id() << '\n';
      }

      T result;
      auto get_return_object() {
         return MyThreadFuture{ handle_type::from_promise(*this) };
      }

      void return_value(T v) {
         std::cout << "        promise_type::return_value:  "
            << "std::this_thread::get_id(): "
            << std::this_thread::get_id() << '\n';
         std::cout << v << std::endl;
         result = v;
      }
      std::suspend_always initial_suspend() {
         return {};
      }

      std::suspend_always final_suspend() noexcept {
         std::cout << "        promise_type::final_suspend:  "
            << "std::this_thread::get_id(): "
            << std::this_thread::get_id() << '\n';
         return {};
      }

      void unhandled_exception() {
         std::exit(1);
      }
   };
};

template<typename T>
struct MyGenerator {

   struct promise_type;
   using handle_type = std::coroutine_handle<promise_type>;

   MyGenerator(handle_type h) : coro(h) {
      std::cout << "        Generator::Generator" << '\n';
   }

   handle_type coro;

   ~MyGenerator() { if (coro) coro.destroy(); }

   MyGenerator(const MyGenerator&) = delete;
   MyGenerator& operator = (const MyGenerator&) = delete;

   MyGenerator(MyGenerator&& oth) : coro(oth.coro) {
      oth.coro = nullptr;
   }

   MyGenerator& operator = (MyGenerator&& oth) {
      coro = oth.coro;
      oth.coro = nullptr;
      return *this;
   }

   T getNextValue() {
      coro.resume();                                             // (13) 
      return coro.promise().current_value;
   }

   bool next() {
      coro.resume();
      return not coro.done();
   }

   T getValue() {
      return coro.promise().current_value;
   }

   //auto begin()
   //{
   //   return {};
   //}

   //auto end()
   //{
   //   return {};
   //}

   struct promise_type {
      promise_type() {                                           // (2)
         std::cout << "            promise_type::promise_type" << '\n';
      }

      ~promise_type() {
         std::cout << "            promise_type::~promise_type" << '\n';
      }

      std::suspend_always initial_suspend() {                    // (5)
         std::cout << "            promise_type::initial_suspend" << '\n';
         return {};                                             // (6)
      }
      std::suspend_always final_suspend() noexcept {
         std::cout << "            promise_type::final_suspend" << '\n';
         return {};
      }
      auto get_return_object() {                                  // (3)
         std::cout << "            promise_type::get_return_object" << '\n';
         return MyGenerator{ handle_type::from_promise(*this) };     // (4)
      }

      std::suspend_always yield_value(int value) {                // (8)
         std::cout << "            promise_type::yield_value" << '\n';
         current_value = value;                                  // (9)
         return {};                                              // (10)
      }

      void return_void() {}
      void unhandled_exception() {
         std::exit(1);
      }

      T current_value;
   };

};

template <typename T> 
struct task 
{
   struct promise_type 
   {
      std::variant<std::monostate, T, std::exception_ptr> result;
      std::coroutine_handle<> waiter;

      task get_return_object() { return { this }; }
      std::suspend_always initial_suspend() { return {}; }
      std::suspend_always final_suspend() { return {}; }
      template <typename U> void return_value(U&& value) {
         result.emplace<1>(std::forward<U>(value));
      }
      void set_exception(std::exception_ptr eptr) {
         result.emplace<2>(std::move(eptr));
      }
      bool await_ready() { return false; }
      void await_suspend(std::coroutine_handle<> CallerCoro) {
         coro.promise().waiter = CallerCoro;
         coro.resume();
      }
      //T await_resume() { return std::get<1>(m_coro.promise().m_result); }
      T await_resume() { 
         if (coro.promise().result.index() == 2)
         {
            std::rethrow_exception(std::get<2>(coro.promise().result));
         }
         return std::get<1>(coro.promise().result); 
      }
   };
private:
   std::coroutine_handle<promise_type> coro;
};

MyLazyFuture<int> createFuture() {                                         // (2)
   std::cout << "createFuture" << '\n';
   co_return 2021;
}

MyGenerator<int> getNext(int start = 10, int step = 10) 
{
   std::cout << "    getNext: start" << '\n';
   auto value = start;
   while (true) {                                                 // (11)
      std::cout << "    getNext: before co_yield" << '\n';
      co_yield value;                                             // (7)
      std::cout << "    getNext: after co_yield" << '\n';
      value += step;
   }
}

MyGenerator<uint> getRange(uint begin, uint count)
{
   std::cout << "    getRange: start" << '\n';
   for (auto value{ begin }; value < count; value++)
   {
      co_yield value;
   }
   //co_yield begin + count;
}

//MyGenerator<int> next_value()
//{
//   using namespace std::chrono_literals;
//   co_await std::chrono::seconds(1 + rand() % 5);
//   co_return rand();
//}

template<std::movable T>
class Generator {
public:
   struct promise_type {
      Generator<T> get_return_object() {
         return Generator{ Handle::from_promise(*this) };
      }
      static std::suspend_always initial_suspend() noexcept {
         return {};
      }
      static std::suspend_always final_suspend() noexcept {
         return {};
      }
      std::suspend_always yield_value(T value) noexcept {
         current_value = std::move(value);
         return {};
      }

      void return_void() {}

      // Disallow co_await in generator coroutines.
      void await_transform() = delete;
      [[noreturn]]
      static void unhandled_exception() {
         throw;
      }

      std::optional<T> current_value;
   };

   using Handle = std::coroutine_handle<promise_type>;

   explicit Generator(const Handle coroutine) :
      m_coroutine{ coroutine }
   {}

   Generator() = default;
   ~Generator() {
      if (m_coroutine) {
         m_coroutine.destroy();
      }
   }

   Generator(const Generator&) = delete;
   Generator& operator=(const Generator&) = delete;

   Generator(Generator&& other) noexcept :
      m_coroutine{ other.m_coroutine }
   {
      other.m_coroutine = {};
   }
   Generator& operator=(Generator&& other) noexcept {
      if (this != &other) {
         if (m_coroutine) {
            m_coroutine.destroy();
         }
         m_coroutine = other.m_coroutine;
         other.m_coroutine = {};
      }
      return *this;
   }

   // Range-based for loop support.
   class Iter {
   public:
      void operator++() {
         m_coroutine.resume();
      }
      const T& operator*() const {
         return *m_coroutine.promise().current_value;
      }
      bool operator==(std::default_sentinel_t) const {
         return !m_coroutine || m_coroutine.done();
      }

      explicit Iter(const Handle coroutine) :
         m_coroutine{ coroutine }
      {}

   private:
      Handle m_coroutine;
   };

   Iter begin() {
      if (m_coroutine) {
         m_coroutine.resume();
      }
      return Iter{ m_coroutine };
   }
   std::default_sentinel_t end() {
      return {};
   }

private:
   Handle m_coroutine;
};

template<std::integral T>
Generator<T> range(T first, const T last) {
   while (first < last) {
      co_yield first++;
   }
}

int main(int argc, char* argv[])
{
   QApplication app(argc, argv);

   std::cout << "Hello World!\n";

   for (const char i : range(65, 91))
   {
      std::cout << i << ' ';
   }
   std::cout << '\n';

   auto gen = getNext();
   ;
   for (int i = 0; i <= 2; ++i) {
      auto val = gen.getNextValue();                              // (12)
      std::cout << "main: " << val << '\n';                       // (14)
   }

   auto fut = createFuture();                                         // (1)
   auto res = fut.get();                                              // (8)
   std::cout << "res: " << res << '\n';

   return app.exec();
}

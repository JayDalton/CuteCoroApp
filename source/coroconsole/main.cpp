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
            std::rethrow_exception(std::get<2>(coro.promise().result);
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

void setupXMLContent()
{
   QByteArray result;
   QXmlStreamWriter writer(&result);

   //writer.setAutoFormatting(true);
   //writer.setAutoFormattingIndent(+1); // +: spaces | -: tabs
   //writer.writeStartDocument();// '<?xml version="1.0" encoding="UTF-8"?>'

   //writer.writeStartElement("http://plansee-net.com/PP", "MT_PD_SEND_CONFIRMATION_req");
   //writer.writeTextElement("Sub_system", "3500_PCAM");
   //writer.writeStartElement("Feedback");

   //writer.writeTextElement("Order_Number", "");
   //writer.writeTextElement("Operation_Number", "");
   //writer.writeTextElement("Sequence", "");
   //writer.writeTextElement("Process_Feedback", "");
   //writer.writeTextElement("ID_Card", "");
   //writer.writeTextElement("Record_Type", "");
   //writer.writeTextElement("Recording_Date", "");
   //writer.writeTextElement("Recording_Time", "");
   //writer.writeTextElement("Workcenter", "");

   //writer.writeEndDocument();

   writer.setAutoFormatting(true);
   writer.writeStartDocument("1.0", true);

   writer.writeStartElement("toolinv");
   writer.writeStartElement("machinevalues");

   writer.writeTextElement("magazine", "1");
   writer.writeTextElement("magazineplace", "14");

   writer.writeStartElement("control");

   writer.writeStartElement("param");
   writer.writeTextElement("id", "$TC_TPC2");
   writer.writeTextElement("value", "1867");
   writer.writeEndElement();

   writer.writeStartElement("param");
   writer.writeTextElement("id", "$TC_TPC8");
   writer.writeTextElement("value", "1");
   writer.writeEndElement();

   writer.writeEndDocument();

   QString stringify(result);
   qWarning() << stringify;
}

int main(int argc, char* argv[])
{
   std::cout << "Hello World!\n";

   setupXMLContent();

   auto fut = createFuture();                                         // (1)
   auto res = fut.get();                                              // (8)
   std::cout << "res: " << res << '\n';

   QVector<std::uint8_t> values{ 13, 54, 123, 32};
   auto variant = QVariant::fromValue(values);

   if (variant.canConvert<QVector<std::uint8_t>>())
   {
      auto vector = variant.value<QVector<std::uint8_t>>();
      for (const auto value : vector)
      {
         std::cout << value << std::endl;
      }
   }

   if (variant.canConvert<QVariantList>()) 
   {
      QSequentialIterable iterable = variant.value<QSequentialIterable>();
      // Can use C++11 range-for:
      for (const QVariant& v : iterable) {
         qDebug() << v;
      }
   }

   std::cout << std::endl;

   std::cout << std::setprecision(10);

   std::cout << "std::numbers::e: " << std::numbers::e << std::endl;
   std::cout << "std::numbers::log2e: " << std::numbers::log2e << std::endl;
   std::cout << "std::numbers::log10e: " << std::numbers::log10e << std::endl;
   std::cout << "std::numbers::pi: " << std::numbers::pi << std::endl;
   std::cout << "std::numbers::inv_pi: " << std::numbers::inv_pi << std::endl;
   std::cout << "std::numbers::inv_sqrtpi: " << std::numbers::inv_sqrtpi << std::endl;
   std::cout << "std::numbers::ln2: " << std::numbers::ln2 << std::endl;
   std::cout << "std::numbers::sqrt2: " << std::numbers::sqrt2 << std::endl;
   std::cout << "std::numbers::sqrt3: " << std::numbers::sqrt3 << std::endl;
   std::cout << "std::numbers::inv_sqrt3: " << std::numbers::inv_sqrt3 << std::endl;
   std::cout << "std::numbers::egamma: " << std::numbers::egamma << std::endl;
   std::cout << "std::numbers::phi: " << std::numbers::phi << std::endl;

   std::cout << std::endl;

   std::vector<std::string> strings{ "abc", "xyz", "fdg", "abc", "cool" };
   std::string s = std::accumulate(strings.begin(), strings.end(), std::string{});
   std::cout << s << std::endl;

   std::vector<int> numbers{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
   std::cout << fmt::format("The answer is {} \n", numbers);

   auto results = numbers
      | std::views::filter([](int n) { return n % 2 == 0; })
      | std::views::transform([](int n) { return n * 2; })
      | std::views::reverse
      | std::views::take(3);

   for (auto v : results)
   {
      std::cout << v << " ";
   }

   std::cout << "\nEnde Program!\n";

   auto arr1 = std::string("C-String Literal");
   if (arr1.starts_with("hello"))
   {
   }

   return 0;
}

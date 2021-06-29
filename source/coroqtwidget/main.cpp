#include "common.h"
#include "commoncute.h"
#include "commoncoro.h"

#include "CuteColorApp.h"

#include <coroutine>
#include <iostream>
#include <optional>
 
template<std::movable T>
class Generator {
public:
    struct promise_type {
        Generator<T> get_return_object() {
            return Generator{Handle::from_promise(*this)};
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
        m_coroutine{coroutine}
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
        m_coroutine{other.m_coroutine}
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
            m_coroutine{coroutine}
        {}
 
    private:
        Handle m_coroutine;
    };
 
    Iter begin() {
        if (m_coroutine) {
            m_coroutine.resume();
        } 
        return Iter{m_coroutine};
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
 
int main(int argc, char *argv[])
{
    for (const char i : range(65, 91)) 
    {
        std::cout << i << ' ';
    }
    std::cout << '\n';

   QApplication app(argc, argv);

   ColorRect cr;
   cr.setWindowTitle("Color Cycler");
   cr.show();

   // change widget color every 500ms
   auto timer{ std::make_unique<QTimer>(&app) };
   auto ro = [&]() -> qtcoro::return_object<> {
      while (true) {
         co_await qtcoro::make_awaitable_signal(timer.get(), &QTimer::timeout);
         cr.changeColor();
      }
   }();

   timer->start(500);

   //// draw lines from clicks
   //auto ptclick_ro = [&]() -> qtcoro::return_object<> {
   //   while (true) {
   //      QPointF first_point = co_await qtcoro::make_awaitable_signal(&cr, &ColorRect::click);
   //      QPointF second_point = co_await qtcoro::make_awaitable_signal(&cr, &ColorRect::click);
   //      cr.setLine(first_point, second_point);
   //   }
   //}();

   //// listen for line creation (tests the tuple code)
   //auto line_ro = [&]() -> qtcoro::return_object<> {
   //   while (true) {
   //      auto [p1, p2] = co_await qtcoro::make_awaitable_signal(&cr, &ColorRect::lineCreated);
   //      std::cout << "we drew a line from (";
   //      std::cout << p1.x() << ", " << p1.y() << ") to (";
   //      std::cout << p2.x() << ", " << p2.y() << ")\n";
   //   }
   //}();

   return app.exec();
}

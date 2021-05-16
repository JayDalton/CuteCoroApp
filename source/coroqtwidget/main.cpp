#include "common.h"
#include "commoncute.h"
#include "commoncoro.h"

#include "CuteColorApp.h"

int main(int argc, char *argv[])
{
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

   // draw lines from clicks
   auto ptclick_ro = [&]() -> qtcoro::return_object<> {
      while (true) {
         QPointF first_point = co_await qtcoro::make_awaitable_signal(&cr, &ColorRect::click);
         QPointF second_point = co_await qtcoro::make_awaitable_signal(&cr, &ColorRect::click);
         cr.setLine(first_point, second_point);
      }
   }();

   // listen for line creation (tests the tuple code)
   auto line_ro = [&]() -> qtcoro::return_object<> {
      while (true) {
         auto [p1, p2] = co_await qtcoro::make_awaitable_signal(&cr, &ColorRect::lineCreated);
         std::cout << "we drew a line from (";
         std::cout << p1.x() << ", " << p1.y() << ") to (";
         std::cout << p2.x() << ", " << p2.y() << ")\n";
      }
   }();

   return app.exec();
}

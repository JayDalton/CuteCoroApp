#include "common.h"

#include "CuteWidgetApp.h"

#include "stdafx.h"

#include "QtCoro.h"

#include <iomanip>
#include <iostream>
#include <numbers>

#include <QtWidgets/QApplication>

#include <optional>
#include <string>
#include <QWidget>
#include <QLine>

class ColorRect : public QWidget
{
   Q_OBJECT

public:
   ColorRect(QWidget *parent = 0);

public slots:
   void changeColor();
   void setLine(QPointF, QPointF);

signals:
   void click(QPointF);
   void lineCreated(QPointF, QPointF);

protected:
   void mousePressEvent(QMouseEvent *) override;
   void paintEvent(QPaintEvent *event) override;

private:
   void setColor(std::string const&);
   std::vector<std::string> m_colorList;
   std::size_t              m_curColor;
   std::optional<QLineF>    m_line;
};

int main(int argc, char *argv[])
{
   std::cout << "Hello World!\n";

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

   auto x = 22;
   auto y = std::to_string(9);
   auto a = std::stoi(std::to_string(++x) + y);
   std::cout << y << ++x;


   QApplication app(argc, argv);
   //CuteWidgetApp w;
   //w.show();

   // a really simple widget
   ColorRect cr;
   cr.setWindowTitle("Color Cycler");
   cr.show();

   //// change widget color every 500ms
   //QTimer* changeTimer = new QTimer(&app);
   //QObject::connect(changeTimer, &QTimer::timeout, [&]() { cr.changeColor(); });
   //changeTimer->start(500);

   //// draw lines from clicks
   //bool got_first_point{ false };
   //QPointF first_point;

   //QObject::connect(&cr, &ColorRect::click, [&](QPointF p) 
   //   {
   //      if (got_first_point) 
   //      {
   //         // draw
   //         cr.setLine(first_point, p);
   //         got_first_point = false;
   //      }
   //      else {
   //         first_point = p;
   //         got_first_point = true;
   //      }
   //   }
   //);

   // change widget color every 500ms
   QTimer* changeTimer = new QTimer(&app);
   auto ro = [&]() -> qtcoro::return_object<> {
      while (true) {
         co_await qtcoro::make_awaitable_signal(changeTimer, &QTimer::timeout);
         cr.changeColor();
      }
   }();

   changeTimer->start(500);

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

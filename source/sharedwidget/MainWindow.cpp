#include "MainWindow.h"

#include <QtWidgets>

MainWindow::MainWindow()
{
   setMinimumSize(QSize(400, 900));
   setWindowTitle(QApplication::applicationName());

   auto handleQueue = [&](std::stop_token stop)
   {
      std::random_device rd;
      std::mt19937 eng(rd());
      std::uniform_int_distribution<> dist(1, 1000);

      while (!stop.stop_requested())
      {
         // between 1 and 1000ms per our distribution
         std::chrono::milliseconds duration(dist(eng));
         std::this_thread::sleep_for(duration);

         if (auto data = m_export.take(stop))
         {
            auto value{ data.value().m_value };
            m_import.push({value * 2, duration});
            update();
         }
      }
   };

   m_threads.push_back(std::jthread(handleQueue));
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
   if (event->key() == Qt::Key_Escape)
   {
      return QApplication::quit();
   }

   m_export.push(QueueData{ event->key() });
}

void MainWindow::paintEvent(QPaintEvent* event)
{
   QMainWindow::paintEvent(event);

   QPainter display{ this };
   display.setPen(QPen{ Qt::green, 10.0 });

   auto flags {Qt::AlignLeft};
   QRectF window{ rect() };
   QRectF logger{};

   /// draw background and first line
   display.fillRect(window, Qt::black);
   display.drawText(window, flags, "Inputs:", &logger);

   for (const QueueData& data : m_import.values())
   {
      /// calculate available drawable area
      window = QRectF{ logger.bottomLeft(), window.bottomRight() };

      /// draw value in new line
      display.drawText(window, flags, data.toString(), &logger);
   }
}

#include "MainWindow.h"

#include <QtWidgets>

MainWindow::MainWindow()
{
   setMinimumSize(QSize(400, 900));
   setWindowTitle(QApplication::applicationName());

   auto handleValues = [&](std::stop_token stop)
   {
      std::random_device rd;
      std::mt19937 eng(rd());
      std::uniform_int_distribution<> dist(1, 1000);

      while (!stop.stop_requested())
      {
         // between 1 and 1000ms per our distribution
         std::chrono::milliseconds duration(dist(eng));
         std::this_thread::sleep_for(duration);

         std::unique_lock lock(m_mutexExport);
         if (!cv.wait(lock, stop, [&] { return !m_exportQueue.empty(); }))
         {
            qWarning() << "cancled";
            return;
         }

         std::scoped_lock guard(m_mutexImport);
         const auto& data{ m_exportQueue.front() };

         m_importQueue.push_front(QueueData{
            data.m_value * 2, duration 
         });

         if (m_size < m_importQueue.size())
         {
            m_importQueue.resize(m_size);
         }
         m_exportQueue.pop_front();
         update();
      }
   };

   m_threads.push_back(std::jthread(handleValues));
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
   if (event->key() == Qt::Key_Escape)
   {
      return QApplication::quit();
   }

   std::scoped_lock lock(m_mutexExport);
   m_exportQueue.push_back(QueueData{ event->key() });
   cv.notify_one();
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

   std::unique_lock lock(m_mutexImport);
   for (const QueueData& data : m_importQueue)
   {
      /// calculate available drawable area
      window = QRectF{ logger.bottomLeft(), window.bottomRight() };

      /// draw value in new line
      display.drawText(window, flags, data.toString(), &logger);
   }
}

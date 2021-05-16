#include "MainWindow.h"

#include <QtWidgets>

MainWindow::MainWindow()
{
   setMinimumSize(QSize(400, 300));
   setWindowTitle(QApplication::applicationName());

   /// start other thread
   m_thread = new QThread();
   m_calculator = new QObject();
   m_calculator->moveToThread(m_thread);
   m_thread->start();
}

MainWindow::~MainWindow()
{
   /// quit thread-eventloop
   /// stop adding new items/signals
   m_thread->quit();

   /// if closing is time critical
   /// wait a while and quit manually
   if (!m_thread->wait(2'000))
   {
      /// Thread didn't exit in time, terminate it!
      /// this is dangerous for possible data loss
      m_thread->terminate();
      m_thread->wait();
   }
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
   const auto key{ event->key() };

   if (key == Qt::Key_Escape)
   {
      return QApplication::quit();
   }

   callThread([=]() {

      /// add captured input to FIFO 1
      /// do heavy time consuming processing...
      const auto value{ key * 2 };
      QThread::msleep(1'000);

      callBack([=]() {

         // append value to cache
         m_cache.push_front(QueueData{ value });
         if (m_size < m_cache.size())
         {
            m_cache.resize(m_size);
         }

         update(); // repaint
      });
   });
}

void MainWindow::paintEvent(QPaintEvent* event)
{
   /// let Qt handle it's event
   QMainWindow::paintEvent(event);

   /// define painter
   QPainter display{ this };
   display.setPen(QPen{ Qt::green, 10.0 });

   /// define some attributes
   auto flags {Qt::AlignLeft};
   QRectF window{ rect() };
   QRectF logger{};

   /// draw background and first line
   display.fillRect(window, Qt::black);
   display.drawText(window, flags, "Inputs:", &logger);

   for (const QueueData& data : m_cache)
   {
      /// calculate available drawable area
      window = QRectF{ logger.bottomLeft(), window.bottomRight() };

      /// draw value in new line
      display.drawText(window, flags, data.toString(), &logger);
   }
}



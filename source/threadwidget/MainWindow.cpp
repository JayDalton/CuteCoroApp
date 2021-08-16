#include "MainWindow.h"

#include <QtWidgets>

MainWindow::MainWindow()
{
   setMinimumSize(QSize(400, 300));
   setWindowTitle(QApplication::applicationName());

   /// instantiate worker without parent
   /// for moving to other thread
   m_calculator = new Calculator();

   /// FIFO 1 - queued connections
   connect(this, &MainWindow::inputCaptured, this, [&](auto data) 
      {

      },
      Qt::QueuedConnection);

   /// FIFO 2 - queued connections
   //connect(m_calculator, &Calculator::dataCalculated, this, [this](auto data)
   //{
   //   m_cache.push_front(data);
   //   if (m_size < m_cache.size())
   //   {
   //      m_cache.resize(m_size);
   //   }
   //   update();
   //}, Qt::QueuedConnection);

   m_threads.push_back(std::jthread([&] {
       
      }));

}

void MainWindow::waitForData(std::stop_token stop)
{
   std::unique_lock lock(m_mutex);
   if (!vc.wait(lock, stop, [&] { return !m_import.empty(); } ))
   {
      // op was canceled
   }

   auto res = m_import.front();
   m_import.pop_front();
   // return res;
}


void thread_func(std::stop_token st, std::string arg1, int arg2)
{
   while (!st.stop_requested())
   {
      //do_stuff(arg1, arg2);
   }
}

MainWindow::~MainWindow()
{
   std::jthread t(thread_func, "", 42);
   //do_stuff();
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
   /// handles exit code
   if (event->key() == Qt::Key_Escape)
   {
      /// start quitting application
      return QApplication::quit();
   }

   std::unique_lock lock(m_mutex);


   /// add captured input to FIFO 1
   emit inputCaptured(QueueData{ event->key() });
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

   for (const QueueData& data : m_import)
   {
      /// calculate available drawable area
      window = QRectF{ logger.bottomLeft(), window.bottomRight() };

      /// draw value in new line
      display.drawText(window, flags, QString::number(data.m_value), &logger);
   }
}

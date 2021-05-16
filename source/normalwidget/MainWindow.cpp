
#include <vector>

#include <QtWidgets>
#include <QListWidget>

#include "MainWindow.h"

MainWindow::MainWindow()
{
   setMinimumSize(QSize(400, 300));
   setWindowTitle(QApplication::applicationName());

   /// instantiate worker without parent
   /// for moving to other thread
   m_calculator = new Calculator();
   m_thread = new QThread();

   /// FIFO 1 - queued connections
   /// enable cross-thread communication via signals and slots
   /// because the worker will be on another thread, Qt::AutoConnection changes to
   /// Qt::QueuedConnection without explicit setting and could be omitted
   /// -> signals data to other thread
   connect(this, &MainWindow::inputCaptured, 
      m_calculator, &Calculator::calculateData, 
      Qt::QueuedConnection);

   /// FIFO 2 - queued connections
   /// enable cross-thread communication via signals and slots
   /// because the worker will be on another thread, Qt::AutoConnection changes to
   /// Qt::QueuedConnection without explicit setting and could be omitted
   /// -> add processed value to local cache and call repaint
   connect(m_calculator, &Calculator::dataCalculated, this, [this](auto data)
   {
      m_cache.push_front(data);
      if (m_size < m_cache.size())
      {
         m_cache.resize(m_size);
      }
      update();
   }, Qt::QueuedConnection);

   /// ensure worker will deleted if thread ends
   connect(m_thread, &QThread::finished, m_calculator, &QObject::deleteLater);

   /// move calculator-worker to other thread
   m_calculator->moveToThread(m_thread);

   /// start other thread
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
   /// handles exit code
   if (event->key() == Qt::Key_Escape)
   {
      /// start quitting application
      return QApplication::quit();
   }

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

   for (const QueueData& data : m_cache)
   {
      /// calculate available drawable area
      window = QRectF{ logger.bottomLeft(), window.bottomRight() };

      /// draw value in new line
      display.drawText(window, flags, data.toString(), &logger);
   }
}



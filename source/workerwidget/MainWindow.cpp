#include "MainWindow.h"

#include <QtWidgets>

MainWindow::MainWindow()
{
   setMinimumSize(QSize(400, 300));
   setWindowTitle(QApplication::applicationName());

   connect(this, &MainWindow::inputCaptured,
      &m_calculator, &Calculator::calculateData,
      Qt::QueuedConnection);

   auto AppendValue = [&](auto data)
   {
      m_cache.push_front(data);
      if (m_size < m_cache.size())
      {
         m_cache.resize(m_size);
      }
      update();
   };

   connect(&m_calculator, &Calculator::dataCalculated, 
      this, AppendValue, Qt::QueuedConnection);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
   const auto key{ event->key() };

   if (key == Qt::Key_Escape)
   {
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



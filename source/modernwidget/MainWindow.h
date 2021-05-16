#pragma once

#include <QDebug>
#include <QMainWindow>

#include <forward_list>

struct QueueData final
{
   signed m_value{ 0 };
   QString toString() const
   {
      return QString::number(m_value);
   }
};

class MainWindow final : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow();
   ~MainWindow() override;

   void changeColor();

protected:
   void keyPressEvent(QKeyEvent *event) override;
   void paintEvent(QPaintEvent* event) override;

private:
   template <typename CF>
   void callThread(const CF& callFunction)
   {
      QMetaObject::invokeMethod(m_calculator, 
         [=]() { callFunction(); },
         Qt::QueuedConnection);
   }

   template <typename NF>
   void callBack(const NF& notifyFunction)
   {
      QMetaObject::invokeMethod(this, 
         [=]() { notifyFunction(); }, 
         Qt::QueuedConnection);
   }

private:
   /// thread where the work happens
   QObject* m_calculator{ nullptr };
   QThread* m_thread{ nullptr };

   uint m_colorIndex{ 0 };
   Qt::GlobalColor m_color{ Qt::black };

   static constexpr std::size_t m_size{ 42 };
   std::list<QueueData> m_cache;
};


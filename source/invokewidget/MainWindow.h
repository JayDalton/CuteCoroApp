#pragma once

#include <QDebug>
#include <QMainWindow>

#include <forward_list>
#include "Calculator.h"

/**
 * @brief The MainWindow class lives in the main-thread and handles input capturing
 * and visualization for captured and processed values in different ListViews
 */
class MainWindow final : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow();
   ~MainWindow() override;

protected:
   void keyPressEvent(QKeyEvent *event) override;
   void paintEvent(QPaintEvent* event) override;

signals:
   void inputCaptured(QueueData data);

private:
   //template <typename NF>
   //void notifyListeners(const NF& notifyFunction)
   //{
   //   QMetaObject::invokeMethod(this, [=]() { notifyFunction(); }, Qt::QueuedConnection);
   //}

   //template <typename CF>
   //void call(const CF& callFunction)
   //{
   //   QMetaObject::invokeMethod(m_threadContext, [=]() { callFunction(); });
   //}

private:
   /// thread where the work happens
   QObject* m_threadContext{ nullptr };
   QThread* m_thread{ nullptr };

   /// worker class, used in Thread
   Calculator* m_calculator{ nullptr };

   static constexpr std::size_t m_size{ 42 };
   std::list<QueueData> m_cache;
};


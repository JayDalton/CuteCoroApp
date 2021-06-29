#pragma once

#include <QDebug>
#include <QMainWindow>

#include <forward_list>

#include "Calculator.h"

class MainWindow final : public QMainWindow
{
   Q_OBJECT

public:
   explicit MainWindow();
   ~MainWindow() override = default;

signals:
   void inputCaptured(QueueData data);

protected:
   void keyPressEvent(QKeyEvent *event) override;
   void paintEvent(QPaintEvent* event) override;

private:
   /// thread where the work happens
   Calculator m_calculator;

   static constexpr std::size_t m_size{ 42 };
   std::list<QueueData> m_cache;
};


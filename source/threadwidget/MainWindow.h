#pragma once

#include <QDebug>
#include <QString>
#include <QMainWindow>

#include <condition_variable>
#include <chrono>
#include <deque>
#include <iostream>
#include <sstream>
#include <forward_list>
#include <mutex>
#include <optional>
#include <string>
#include <shared_mutex>
#include <thread>
#include <vector>
#include <random>
#include <format>


//#include "Calculator.h"

struct QueueData final
{
   signed m_value{ 0 };
   std::chrono::milliseconds m_delay;

   QString toString() const
   {
      return QString::fromStdString(std::format(
         "ID {:5} in {:>8}", m_value, m_delay));
   }
};

class MainWindow final : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow();
   ~MainWindow() override = default;

protected:
   void keyPressEvent(QKeyEvent *event) override;
   void paintEvent(QPaintEvent* event) override;

private:
   static constexpr std::size_t m_size{ 42 };

   std::shared_mutex m_mutexExport;
   std::shared_mutex m_mutexImport;

   std::deque<QueueData> m_exportQueue;
   std::deque<QueueData> m_importQueue;

   std::condition_variable_any cv;

   std::vector<std::jthread> m_threads;
};


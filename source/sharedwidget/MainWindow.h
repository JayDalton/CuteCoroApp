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

template<typename T>
class Queue 
{
   std::deque<T> m_queue;
   std::mutex m_mutex;
   std::condition_variable_any m_cv;
   std::size_t m_size{ 42 };

public:
   Queue() = default;
   Queue(const Queue<T>&) = delete;
   Queue& operator=(const Queue<T>&) = delete;

   void push_back(const T& item) 
   {
      std::scoped_lock lock(m_mutex);
      m_queue.push_back(item);
      if (m_size < m_queue.size())
      {
         m_queue.resize(m_size);
      }
      m_cv.notify_one();
   }

   std::optional<T> pop_front(std::stop_token stop) 
   {
      std::unique_lock lock(m_mutex);
      if (!m_cv.wait(lock, stop, [&] { return !m_queue.empty(); }))
      {
         qWarning() << "cancled";
         return {};
      }

      const T data{ m_queue.front() };
      m_queue.pop_front();
      return data;
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

   Queue<QueueData> m_export;
   Queue<QueueData> m_import;

   std::condition_variable_any cv;

   std::vector<std::jthread> m_threads;
};


#pragma once

#include <QDebug>
#include <QString>
#include <QMainWindow>

#include <condition_variable>
#include <chrono>
#include <deque>
#include <queue>
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
   std::list<T> m_content;
   mutable std::mutex m_mutex;
   const std::size_t m_size{ 42 };
   std::condition_variable_any m_signal;

public:
   Queue() = default;
   Queue(const Queue<T>&) = delete;
   Queue& operator=(const Queue<T>&) = delete;

   void push(T&& item) 
   {
      std::lock_guard lock(m_mutex);
      while (m_size <= m_content.size())
      {
         m_content.pop_front();
      }
      m_content.push_back(item);
      m_signal.notify_one();
   }

   std::vector<T> values() const
   {
      std::scoped_lock lock(m_mutex);
      return { m_content.cbegin(), m_content.cend() };
   }

   std::optional<T> take(std::stop_token stop)
   {
      std::unique_lock lock(m_mutex);
      if (!m_signal.wait(lock, stop, [&] { return !m_content.empty(); }))
      {
         qWarning() << "cancled";
         return {};
      }

      const T data{ m_content.front() };
      m_content.pop_front();
      return data;
   }
};

class MainWindow final : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow();

protected:
   void keyPressEvent(QKeyEvent *event) override;
   void paintEvent(QPaintEvent* event) override;

private:
   Queue<QueueData> m_export;
   Queue<QueueData> m_import;
   std::jthread m_thread;
};


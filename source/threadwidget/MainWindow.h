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
   std::thread::id m_exportID;
   std::thread::id m_importID;

   friend std::ostream& operator<<(std::ostream& out, const QueueData& data)
   {
      out << std::to_string(data.m_value);
      out << ": Export " << data.m_exportID;
      out << ": Import " << data.m_importID;
      return out;
   }

   QString toString() const
   {
      std::ostringstream stream;
      stream << std::to_string(m_value);
      stream << ": Export " << m_exportID;
      stream << ": Import " << m_importID;
      return QString::fromStdString(stream.str());
      //return std::format("", m_value, m_exportID, m_importID);
   }
};

template <typename T>
class Stack
{
   struct node {
      T value;
      std::shared_ptr<node> next;
      node() {} 
      node(T&& nv) : value(std::move(nv)) {}
   };

   std::atomic<std::shared_ptr<node>> head;

public:
   Stack() : head(nullptr) {}
   ~Stack() { while (head.load()) pop(); }

   void push(T val)
   {
      auto new_node = std::make_shared<node>(std::move(val));
      new_node->next = head.load();
      while (!head.compare_exchange_weak(new_node->next, new_node))
      {

      }
   }

   T pop()
   {
      while (auto old_head = head.load())
      {
         if (head.compare_exchange_strong(old_head, old_head->next))
         {
            return std::move(old_head->value);
         }
      }
      throw std::runtime_error("Stack empty");
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

   std::mutex m_mutexExport;
   std::mutex m_mutexImport;

   std::deque<QueueData> m_export;
   std::deque<QueueData> m_import;

   std::condition_variable_any cv;

   std::vector<std::jthread> m_threads;
};


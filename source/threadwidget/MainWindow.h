#pragma once

#include <QDebug>
#include <QMainWindow>

#include <condition_variable>
#include <deque>
#include <forward_list>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

//#include "Calculator.h"

struct QueueData final
{
   signed m_value{ 0 };
   std::string toString() const
   {
      return std::to_string(m_value);
   }
};

struct Calculator
{
   Calculator() {}

   void calculateData(QueueData data)
   {

   }

private:
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
      auto old_head = head.load();
      while (old_head)
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
   ~MainWindow() override;

protected:
   void keyPressEvent(QKeyEvent *event) override;
   void paintEvent(QPaintEvent* event) override;

signals:
   void inputCaptured(QueueData data);

private:
   void waitForData(std::stop_token token);

private:
   /// worker class, used in Thread
   Calculator* m_calculator{ nullptr };

   static constexpr std::size_t m_size{ 42 };
   //std::list<QueueData> m_cache;

   std::mutex m_mutex;
   std::deque<QueueData> m_export;
   std::deque<QueueData> m_import;
   std::condition_variable_any vc;

   std::vector<std::jthread> m_threads;
};


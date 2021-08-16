#pragma once

#include <optional>
#include <string>
#include <thread>
#include <vector>

//struct QueueData final
//{
//   signed m_value{ 0 };
//   std::string toString() const
//   {
//      return std::to_string(m_value);
//   }
//};
//
///**
// * @brief The Calculator class is a worker class that define the work
// * and is instance will be moved in extra thread. Communication handled
// * by signals / slots mechanism, espacially
// */
//class Calculator
//{
//public:
//   explicit Calculator();
//
//   void calculateData(QueueData data);
//
////signals:
////   void dataCalculated(QueueData data);
//
//private:
//   //std::optional m_;
//   std::jthread m_thread;
//   std::vector<std::jthread> m_threads;
//};


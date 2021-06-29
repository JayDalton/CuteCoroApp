#include "Calculator.h"

Calculator::Calculator()
{
   // create timer, socket, etc.
   m_thread.reset(new QThread); // no parent !!!
   moveToThread(m_thread.get());
   m_thread->start();
}

Calculator::~Calculator()
{
   QMetaObject::invokeMethod(this, "cleanup");
   m_thread->wait();
}

void Calculator::calculateData(QueueData data)
{
   /// do heavy time consuming processing...
   /// multiply value by two and signal new value
   /// add calculated value to FIFO 2

   const auto value{ data.m_value * 2 };

   QThread::msleep(1'000);

   emit dataCalculated(QueueData{ value });
}

void Calculator::cleanup()
{
   // delete timer, socket...
   m_thread->quit();
}

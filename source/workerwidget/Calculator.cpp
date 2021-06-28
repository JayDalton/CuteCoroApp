#include "Calculator.h"

#include <QThread>

//Calculator::Calculator(QObject* parent)
//   : QObject(parent)
//{
//
//}

void Calculator::calculateData(QueueData data)
{
   /// do heavy time consuming processing...
   /// multiply value by two and signal new value
   /// add calculated value to FIFO 2

   const auto value{ data.m_value * 2 };

   QThread::msleep(1'000);

   emit dataCalculated(QueueData{ value });
}

#include "Calculator.h"

Calculator::Calculator(QObject* parent)
   : QObject(parent)
{

}

void Calculator::calculateData(QueueData data)
{
   /// do heavy time consuming processing...
   /// multiply value by two and signal new value
   /// add calculated value to FIFO 2
   emit dataCalculated(QueueData{ data.m_value * 2 });
}

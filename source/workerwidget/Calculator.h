#pragma once

#include <QDebug>
#include <QObject>
#include <QThread>


struct QueueData final
{
   signed m_value{ 0 };
   QString toString() const
   {
      return QString::number(m_value);
   }
};
Q_DECLARE_METATYPE(QueueData) 

/**
 * @brief The Calculator class is a worker class that define the work
 * and is instance will be moved in extra thread. Communication handled
 * by signals / slots mechanism, espacially
 */
class Calculator final : public QObject
{
   Q_OBJECT

public:
   explicit Calculator()
   {
      // create timer, socket, etc.
      m_thread.reset(new QThread); // no parent !!!
      moveToThread(m_thread.get());
      m_thread->start();
   }

   ~Calculator() override {
      QMetaObject::invokeMethod(this, "cleanup");
      m_thread->wait();
   }

   void calculateData(QueueData data);

signals:
   void dataCalculated(QueueData data);

private slots:
   void cleanup() {
      // delete timer, socket...
      m_thread->quit();
   }

private:
   std::unique_ptr<QThread> m_thread;
};


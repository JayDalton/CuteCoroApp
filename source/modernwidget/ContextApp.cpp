#include "ContextApp.h"

#include "commoncoro.h"

ContextApp::ContextApp(int &argc, char **argv)
   : QApplication(argc, argv)
{
   QCoreApplication::setOrganizationName("Organization Name");
   QCoreApplication::setApplicationName("Application Name");
   QCoreApplication::setApplicationVersion(QT_VERSION_STR);

   m_window.show();

   m_timer.setSingleShot(false);
   m_timer.setInterval(std::chrono::milliseconds(500));

   // change widget color every 500ms
   auto ro = [&]() -> qtcoro::return_object<> {
      while (true) {
         co_await qtcoro::make_awaitable_signal(&m_timer, &QTimer::timeout);
         m_window.changeColor();
      }
   }();

   m_timer.start();
}

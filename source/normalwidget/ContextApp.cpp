#include "ContextApp.h"

ContextApp::ContextApp(int &argc, char **argv)
   : QApplication(argc, argv)
{
   QCoreApplication::setOrganizationName("Organization Name");
   QCoreApplication::setApplicationName("Application Name");
   QCoreApplication::setApplicationVersion(QT_VERSION_STR);

   m_window.show();
}

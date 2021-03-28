#include "CuteWidgetApp.h"

#include "ui_CuteWidgetApp.h"

CuteWidgetApp::CuteWidgetApp(QWidget* parent)
   : QMainWindow(parent)
{
   m_ui = std::make_unique<Ui::CuteWidgetAppClass>();
   m_ui->setupUi(this);
}

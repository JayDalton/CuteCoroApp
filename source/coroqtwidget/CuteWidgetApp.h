#pragma once

#include "common.h"
#include "commoncute.h"

namespace Ui { class CuteWidgetAppClass; };

class CuteWidgetApp : public QMainWindow
{
   Q_OBJECT

public:
   CuteWidgetApp(QWidget* parent = nullptr);

private:
   std::unique_ptr<Ui::CuteWidgetAppClass> m_ui;
};

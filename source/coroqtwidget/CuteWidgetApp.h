#pragma once

#include "common.h"
#include "commoncute.h"

#include "ui_CuteWidgetApp.h"

class CuteWidgetApp : public QMainWindow
{
    Q_OBJECT

public:
    CuteWidgetApp(QWidget *parent = nullptr);

private:
    Ui::CuteWidgetAppClass ui;
};

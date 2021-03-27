#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_CuteWidgetApp.h"

class CuteWidgetApp : public QMainWindow
{
    Q_OBJECT

public:
    CuteWidgetApp(QWidget *parent = Q_NULLPTR);

private:
    Ui::CuteWidgetAppClass ui;
};

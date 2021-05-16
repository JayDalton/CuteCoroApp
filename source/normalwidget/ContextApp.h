#pragma once

#include <QApplication>

#include "MainWindow.h"

class ContextApp final : public QApplication
{
public:
   explicit ContextApp(int &argc, char **argv);
   ~ContextApp() override = default;

private:
   MainWindow m_window;
};


#include "CuteColorApp.h"

#include <QMouseEvent>
#include <QPainter>

ColorRect::ColorRect(QWidget* parent)
   : QWidget{ parent }, m_curColor{ 0 }
   , m_colorList{
      {"#111111", "#113311",
      "#111133", "#331111",
      "#333311", "#331133",
      "#661111", "#116611",
      "#111166", "#663311",
      "#661133", "#336611",
      "#331166", "#113366"} }

{
}

void ColorRect::setColor(std::string const& col)
{
   setStyleSheet(("background-color:" + col).c_str());
}

void ColorRect::changeColor()
{
   m_curColor++;
   if (m_curColor >= m_colorList.size())
   {
      m_curColor = 0;
   }
   setColor(m_colorList[m_curColor]);
}

void ColorRect::mousePressEvent(QMouseEvent* e)
{
   emit click(e->windowPos());
}

void ColorRect::keyPressEvent(QKeyEvent* event)
{
   const auto key{ event->key() };

   if (key == Qt::Key_Escape)
   {
      return QApplication::quit();
   }

   QWidget::keyPressEvent(event);
}

void ColorRect::paintEvent(QPaintEvent*)
{
   if (m_line)
   {
      QPainter painter(this);
      painter.setPen(QPen{ QColor{"yellow"} });
      painter.drawLine(*m_line);
   }
}

void ColorRect::setLine(QPointF p1, QPointF p2)
{
   m_line = QLineF{ p1, p2 };
   emit lineCreated(p1, p2);
}

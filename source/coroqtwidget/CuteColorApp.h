#pragma once

#include "common.h"
#include "commoncute.h"

class ColorRect : public QWidget
{
   Q_OBJECT

public:
   ColorRect(QWidget* parent = nullptr);

public slots:
   void changeColor();
   void setLine(QPointF, QPointF);

signals:
   void click(QPointF);
   void lineCreated(QPointF, QPointF);

protected:
   void mousePressEvent(QMouseEvent*) override;
   void paintEvent(QPaintEvent* event) override;

private:
   void setColor(std::string const&);

   std::vector<std::string> m_colorList;
   std::optional<QLineF> m_line;
   std::size_t m_curColor;
};


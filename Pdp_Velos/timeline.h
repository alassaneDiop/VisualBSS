#ifndef TIMELINE_H
#define TIMELINE_H

#include <QWidget>
#include "qpoint.h"
#include "qpainter.h"
#include "qpen.h"
#include "qbrush.h"
#include "qdebug.h"

class TimeLine : public QWidget
{
    Q_OBJECT
public:
    TimeLine(QWidget *parent = 0);
    int getTimeLineLength();
    int getTimeLineOffsetX();

protected:
    void paintEvent(QPaintEvent *event);

private:
    const int m_numberOfInterval = 8;
    const int m_widgetHeight = 30;
    const int m_lineHeight = 10;
    const int m_textOffsetX = -4;
    const int m_textOffsetY = 15;
    const int m_timeInterval = (24 / (m_numberOfInterval - 1));
    int m_lineInterval;

    const int m_timeLineOffsetX = 10;
    int m_timeLineWidth;
};

#endif // TIMELINE_H

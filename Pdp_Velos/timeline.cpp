#include "timeline.h"

TimeLine::TimeLine(QWidget *parent)
{
    this->setFixedHeight(m_widgetHeight);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::white);
    this->setAutoFillBackground(true);
    this->setPalette(pal);
}

int TimeLine::getTimeLineLength()
{
    return m_timeLineWidth;
}

int TimeLine::getTimeLineOffsetX()
{
    return m_timeLineOffsetX;
}

void TimeLine::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    m_timeLineWidth = this->width() - 2 * m_timeLineOffsetX;
    m_lineInterval = m_timeLineWidth / m_numberOfInterval;

    for (int i = 0; i <= m_numberOfInterval; ++i)
    {
        QPoint p1(m_timeLineOffsetX + i * m_lineInterval, 0);
        QPoint p2(m_timeLineOffsetX + i * m_lineInterval, m_lineHeight);
        painter.begin(this);
        painter.drawLine(p1, p2);
        painter.drawText(
                    QPoint(m_timeLineOffsetX + m_textOffsetX + i * m_lineInterval, m_lineHeight + m_textOffsetY),
                    QString::number((m_timeInterval * i) % 24));
        painter.end();
    }

    painter.begin(this);
    painter.drawLine(QPoint(m_timeLineOffsetX + 0, 0), QPoint(m_timeLineWidth + 3, 0));
    painter.end();
}


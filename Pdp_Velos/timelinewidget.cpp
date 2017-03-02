#include "timelinewidget.h"
#include <QPoint>
#include <QPainter>

TimeLineWidget::TimeLineWidget(QWidget *parent)
{
    this->setFixedHeight(m_widgetHeight);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::white);
    this->setAutoFillBackground(true);
    this->setPalette(pal);
}

int TimeLineWidget::getLength() const
{
    return m_timeLineLength;
}

int TimeLineWidget::getOffsetX() const
{
    return m_timeLineOffsetX;
}

void TimeLineWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    m_timeLineLength = this->width() - (2 * m_timeLineOffsetX);
    m_lineInterval = m_timeLineLength / m_numberOfInterval;

    for (int i = 0; i <= m_numberOfInterval; ++i)
    {
        QPoint p1(m_timeLineOffsetX + (i * m_lineInterval), 0);
        QPoint p2(m_timeLineOffsetX + (i * m_lineInterval), m_lineHeight);
        painter.begin(this);
        painter.drawLine(p1, p2);
        QString number = QString::number((m_timeInterval * i) % m_hourInterval);

        painter.drawText(
                    QPoint(m_timeLineOffsetX + m_textOffsetX + (i * m_lineInterval),
                           m_lineHeight + m_textOffsetY),
                    number);

        painter.end();
    }

    painter.begin(this);
    painter.drawLine(QPoint(m_timeLineOffsetX, 0),
                     QPoint(m_timeLineLength, 0));
    painter.end();
}


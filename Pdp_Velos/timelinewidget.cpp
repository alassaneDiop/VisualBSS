#include "timelinewidget.h"
#include <QPoint>
#include <QPainter>


TimeLineWidget::TimeLineWidget(QWidget *parent)
{
    Q_UNUSED(parent);

    this->setFixedHeight(m_widgetHeight);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::white);
    this->setAutoFillBackground(true);
    this->setPalette(pal);
}

// TODO: supprimer
//int TimeLineWidget::length() const
//{
//    return m_timeLineLength;
//}

void TimeLineWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter;
    m_timeLineLength = this->width() - (2 * bss::TIMELINE_OFFSET_X);
    m_lineInterval = m_timeLineLength / m_numberOfInterval;

    for (int i = 0; i <= m_numberOfInterval; ++i)
    {
        QPoint p1(bss::TIMELINE_OFFSET_X + (i * m_lineInterval), 0);
        QPoint p2(bss::TIMELINE_OFFSET_X + (i * m_lineInterval), m_lineHeight);
        painter.begin(this);
        painter.drawLine(p1, p2);
        QString number = QString::number((m_timeInterval * i) % bss::NB_OF_HOUR);

        painter.drawText(
                    QPoint(bss::TIMELINE_OFFSET_X + m_textOffsetX + (i * m_lineInterval),
                           m_lineHeight + m_textOffsetY),
                    number);

        painter.end();
    }

    painter.begin(this);
    painter.drawLine(QPoint(bss::TIMELINE_OFFSET_X, 0),
                     QPoint(m_timeLineLength, 0));
    painter.end();
}


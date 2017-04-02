/* Copyright 2017
 * All rights reserved to "Université de Bordeaux"
 *
 * This file is part of VisualBSS.
 * VisualBSS is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VisualBSS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with VisualBSS.  If not, see <http://www.gnu.org/licenses/>.
 * */

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
        QString number = QString::number((m_timeInterval * i) % bss::NB_OF_HOURS);

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


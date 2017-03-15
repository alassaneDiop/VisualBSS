#ifndef TIMELINE_H
#define TIMELINE_H

#include <QWidget>

#include "config.h"

namespace bss {
class TimeLineWidget;
}

class TimeLineWidget : public QWidget
{
    Q_OBJECT
public:
    TimeLineWidget(QWidget *parent = 0);

    // TODO: supprimer
    int length() const;

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    const int m_numberOfInterval = 8;

    const int m_widgetHeight = 30;
    const int m_lineHeight = 10;

    const int m_textOffsetX = -4;
    const int m_textOffsetY = 15;

    const int m_timeInterval = bss::NB_OF_HOUR / (m_numberOfInterval - 1);

    int m_lineInterval;
    int m_timeLineLength;
};

#endif // TIMELINE_H

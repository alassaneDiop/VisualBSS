#ifndef TIMELINE_H
#define TIMELINE_H

#include <QWidget>

namespace bss {
class TimeLineWidget;
}

class TimeLineWidget : public QWidget
{
    Q_OBJECT
public:
    TimeLineWidget(QWidget *parent = 0);
    int length() const;
    int offsetX() const;

protected:
    void paintEvent(QPaintEvent *event);

private:
    const unsigned int m_hourInterval = 24;
    const int m_numberOfInterval = 8;
    const int m_widgetHeight = 30;
    const int m_lineHeight = 10;
    const int m_textOffsetX = -4;
    const int m_textOffsetY = 15;
    const int m_timeInterval = (m_hourInterval / (m_numberOfInterval - 1));
    int m_lineInterval;

    const int m_timeLineOffsetX = 10;
    int m_timeLineLength;
};

#endif // TIMELINE_H

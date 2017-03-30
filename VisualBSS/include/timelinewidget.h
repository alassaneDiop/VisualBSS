#ifndef TIMELINE_H
#define TIMELINE_H

#include <QWidget>

#include "config.h"


namespace bss {
class TimeLineWidget;
}

/**
 * @brief The TimeLineWidget class Custom widget which draw timeline.
 */
class TimeLineWidget : public QWidget
{
    Q_OBJECT
public:
    TimeLineWidget(QWidget *parent = 0);

    /**
     * @brief length Getter on widget length.
     * @return the length of the widget.
     */
    int length() const;

protected:
    /**
     * @brief paintEvent Fonction that draws all elements (text and lines).
     * @param event
     */
    virtual void paintEvent(QPaintEvent *event) override;

private:
    /**
     * @brief m_numberOfInterval Number of interval to draw line with text hours.
     */
    const int m_numberOfInterval = 8;

    /**
     * @brief m_widgetHeight Height widget.
     */
    const int m_widgetHeight = 30;

    /**
     * @brief m_lineHeight Vertical height of the line with the text just
     * below.
     */
    const int m_lineHeight = 10;

    /**
     * @brief m_textOffsetX To center the text with the line we substract by
     * 4 pixels.
     */
    const int m_textOffsetX = -4;

    /**
     * @brief m_textOffsetY The text will be placed 5 pixels under the line.
     */
    const int m_textOffsetY = m_lineHeight + 5;

    /**
     * @brief m_timeInterval
     */
    const int m_timeInterval = bss::NB_OF_HOURS / (m_numberOfInterval - 1);

    /**
     * @brief m_lineInterval
     */
    int m_lineInterval;

    /**
     * @brief m_timeLineLength Calculated line length.
     */
    int m_timeLineLength;
};

#endif // TIMELINE_H

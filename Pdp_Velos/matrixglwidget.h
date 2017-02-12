#ifndef MATRIXOPENGLWIDGET_H
#define MATRIXOPENGLWIDGET_H

#include "qopenglwidget.h"
#include "qopenglfunctions.h"
#include "qmath.h"
#include "qpainter.h"
#include "qelapsedtimer.h"
#include "QWheelEvent"
#include "qdebug.h"
#include "qscrollarea.h"
#include "trip.h"
#include "station.h"

class MatrixGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    MatrixGLWidget(QWidget* p = 0);
    ~MatrixGLWidget();

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void wheelEvent(QWheelEvent* event);
    void mouseMoveEvent (QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
//    void keyPressEvent(QKeyEvent* event);
//    void keyReleaseEvent(QKeyEvent* event);

public:
    void drawDragSelection();
    void drawDirections();

public slots:
    void onDataLoaded(const QList<Station>& stations, const QList<Trip>& trips);

private:
    // Display
    const int m_numberOfInterval = 24;
    const int m_matrixOffsetX = 10;
    int m_matrixViewWidth;
    int m_stationCircleSize = 10;

    bool m_leftMouseButtonPressed;
    QPointF m_previousMousePos;
    int m_translationValue;

    QPointF m_topLeftSelectionRectangle;
    QPointF m_bottomRightSelectionRectanle;

    bool m_drawRectangle;
    int m_dragSelectionBorderWidth;

    QList<Station> m_stations;
    QList<Trip> m_trips;
};
#endif // MATRIXOPENGLWIDGET_H

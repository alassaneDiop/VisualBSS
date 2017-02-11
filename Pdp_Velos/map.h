#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include "qopenglwidget.h"
#include "qopenglfunctions.h"
#include "qmath.h"
#include "qpainter.h"
#include "qelapsedtimer.h"
#include "QWheelEvent"
#include "qdebug.h"

class Map : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    Map(QWidget* p = 0);
    ~Map();
protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void wheelEvent(QWheelEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);

public:
    void drawStations();
    void drawTrips();


    const float Pi = 3.141592654f;

    float degToRad(float x)
    {
        return x / 180 * Pi;
    }

    float radToDeg(float x)
    {
        return x / Pi * 180;
    }

    int cartesianX(float r, float inclination, float azimut)
    {
        return r * sin(inclination) * cos(azimut);
    }

    int cartesianY(float r, float inclination)
    {
        return r * cos(inclination);
    }

private:
    float m_zoom;
    bool m_leftMouseButtonPressed;
    float m_translationOffsetX;
    float m_translationOffsetY;
    QPointF m_previousMousePos;
};

#endif // OPENGLWIDGET_H

#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>


class Station;
class Trip;
class Point;
class QOpenGLShaderProgram;
class QOpenGLBuffer;
class QWheelEvent;

class MapGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    MapGLWidget(QWidget* p = 0);
    ~MapGLWidget();
protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void wheelEvent(QWheelEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

public:

public slots:
    void onDataLoaded(const QList<Station>& stations, const QList<Trip>& trips);

private:
    void drawStations();
    void drawTrips();


    float m_zoom;
    bool m_leftMouseButtonPressed;
    float m_translationOffsetX;
    float m_translationOffsetY;
    QPointF m_previousMousePos;

    QVector<float> m_tripsVertices;
    int m_tripsVerticesCount;

    QVector<float> m_stationsVertices;
    int m_stationsVerticesCount;

    QOpenGLVertexArrayObject m_tripsVAO;
    QOpenGLBuffer m_tripsVBO;

    QOpenGLVertexArrayObject m_stationsVAO;
    QOpenGLBuffer m_stationsVBO;

    QOpenGLShaderProgram* m_shaderProgramStations;

};

#endif // OPENGLWIDGET_H

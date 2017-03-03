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
    virtual void initializeGL() override;
    virtual void resizeGL(int width, int height) override;
    virtual void paintGL() override;
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

public:

public slots:
    void onDataLoaded(const QList<Station>& stations, const QList<Trip>& trips);

private:
    void drawStations();
    void drawTrips();
    void calculateBoundingBoxStations(const QList<Station>& stations);
    void calculateTranlsation();
    void calculalteZoom();

    const float m_translationSensibility = 500.f;
    QRectF m_boundingBoxStations;

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

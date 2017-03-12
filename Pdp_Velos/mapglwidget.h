#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QWheelEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include "stationrenderer.h"
#include "triprenderer.h"


class Point;
class QOpenGLShaderProgram;
class QOpenGLBuffer;

class MapGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    MapGLWidget(QWidget* p = 0);
    ~MapGLWidget();

    void loadTripsData(const QVector<float>& data, unsigned int verticesCount);
    void loadStationsData(const QVector<float>& data, unsigned int verticesCount);
    void centerView(const QVector<float>& data);


protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int width, int height) override;
    virtual void paintGL() override;
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void drawStations();
    void drawTrips();
    void calculateTranslation();
    void calculateZoom();
    void calculateBoundingBoxStations(const QVector<float>& data);

//    bool createStationsShader();
//    bool createTripsShader();


private:
    //FIXME: calculer en fonction de la largeur hauteur du widget
    const float m_translationSensibility = 200.f;

    QRectF m_boundingBoxStations;

    float m_zoom;
    bool m_leftMouseButtonPressed;
    float m_translationOffsetX;
    float m_translationOffsetY;
    QPointF m_previousMousePos;

    QOpenGLShaderProgram* m_shaderProgramStations;
    QOpenGLShaderProgram* m_shaderProgramTrips;

    StationRenderer*    m_stationRenderer;
    TripRenderer*       m_tripRenderer;

    bool m_stationsLoaded;
    bool m_tripsLoaded;
    bool m_isStationsVAOCreated;
    bool m_isTripsVAOCreated;

    bool m_isTripsShaderCreated;
    bool m_isStationsShaderCreated;
};

#endif // OPENGLWIDGET_H

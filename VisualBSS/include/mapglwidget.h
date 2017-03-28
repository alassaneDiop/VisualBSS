#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QWheelEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>



namespace bss {
class MapGLWidget;
}

class TripRenderer;
class StationRenderer;
class PointF;
class QOpenGLShaderProgram;

/**
 * @brief The MapGLWidget class
 */
class MapGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    MapGLWidget(QWidget* p = 0);
    ~MapGLWidget();

    /**
     * @brief clear All stations and tips on the widget
     */
    void clear();

    /**
     * @brief loadTripsData Create VAO if not created and send all vertices to be
     * rendered
     * @param data Contains all trips vertices
     * @param verticesCount Nomber of vertices to draw
     */
    void loadTripsData(const QVector<float>& data, unsigned int verticesCount);

    /**
     * @brief loadStationsData Create VAO if not created and send all vertices to be
     * rendered
     * @param data Contains all stations vertices
     * @param verticesCount Nomber of vertices to draw
     */
    void loadStationsData(const QVector<float>& data, unsigned int verticesCount);

    /**
     * @brief centerView Center the view on the stations and trips with an appropriate
     * zoom
     * @param data Iterate over all data to calculate the centered view
     */
    void centerView(const QVector<float>& data);


protected:
    /**
     * @brief initializeGL Initialize OpenGL function and check shaders errors
     */
    virtual void initializeGL() override;

    /**
     * @brief resizeGL
     * @param width
     * @param height
     */
    virtual void resizeGL(int width, int height) override;

    /**
     * @brief paintGL Main loop to draw all Stations and Trips
     */
    virtual void paintGL() override;

    /**
     * @brief wheelEvent
     * @param event
     */
    virtual void wheelEvent(QWheelEvent* event) override;

    /**
     * @brief mouseMoveEvent
     * @param event
     */
    virtual void mouseMoveEvent(QMouseEvent* event) override;

    /**
     * @brief mousePressEvent
     * @param event
     */
    virtual void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief mouseReleaseEvent
     * @param event
     */
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

private:
    /**
     * @brief drawStations Send uniform variables and draw stations.
     */
    void drawStations();

    /**
     * @brief drawTrips Send uniform variables and draw trips.
     */
    void drawTrips();

    /**
     * @brief calculateTranslation Calculate the x and y value of translation
     * to center the view correctly.
     */
    void calculateTranslation();

    /**
     * @brief calculateZoom Calculate the zoom value to see all stations and trips
     * on screen.
     */
    void calculateZoom();

    /**
     * @brief calculateBoundingBoxStations Set m_boundingBoxStations to the correct
     * bounding box value.
     * @param data Represents all the vertices to compute the bounding box
     */
    void calculateBoundingBoxStations(const QVector<float>& data);

    /**
     * @brief initializeShaderTrips Load, compile and link trips shaders, return
     * false if something went wrong, true otherwise.
     * @return
     */
    bool initializeShaderTrips();

    /**
     * @brief initializeShaderStations Load, compile and link stations shaders, return
     * false if something went wrong, true otherwise.
     * @return
     */
    bool initializeShaderStations();



    /**
     * @brief m_translationSensibility
     */
    float m_translationSensibility;

    /**
     * @brief m_boundingBoxStations
     */
    QRectF m_boundingBoxStations;

    /**
     * @brief m_zoom
     */
    float m_zoom;

    /**
     * @brief m_leftMouseButtonPressed
     */
    bool m_leftMouseButtonPressed;

    /**
     * @brief m_translationX
     */
    float m_translationX;

    /**
     * @brief m_translationY
     */
    float m_translationY;

    /**
     * @brief m_previousMousePos
     */
    QPointF m_previousMousePos;

    /**
     * @brief m_shaderProgramStations
     */
    QOpenGLShaderProgram* m_shaderProgramStations;

    /**
     * @brief m_shaderProgramTrips
     */
    QOpenGLShaderProgram* m_shaderProgramTrips;

    /**
     * @brief m_stationRenderer
     */
    StationRenderer* m_stationRenderer;

    /**
     * @brief m_tripRenderer
     */
    TripRenderer* m_tripRenderer;

    /**
     * @brief m_drawStations
     */
    bool m_drawStations;

    /**
     * @brief m_drawTrips
     */
    bool m_drawTrips;

    /**
     * @brief m_isStationsVAOCreated
     */
    bool m_isStationsVAOCreated;

    /**
     * @brief m_isTripsVAOCreated
     */
    bool m_isTripsVAOCreated;

    /**
     * @brief m_isTripsShaderCreated
     */
    bool m_isTripsShaderCreated;

    /**
     * @brief m_isStationsShaderCreated
     */
    bool m_isStationsShaderCreated;

    /**
     * @brief m_stationsPosition
     */
    QVector<float> m_stationsPosition;

signals:
    void onShaderError(const QString& message);
};

#endif // OPENGLWIDGET_H

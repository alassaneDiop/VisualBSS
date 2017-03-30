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
 * @brief The MapGLWidget class This class is a view, and draw in OpenGL context
 * trips and stations.
 */
class MapGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    MapGLWidget(QWidget* p = 0);
    ~MapGLWidget();

    /**
     * @brief clear Clears all stations and trips of the widget.
     */
    void clear();

    /**
     * @brief loadTripsData Creates VAO if its not created and send all vertices
     * to be rendered.
     * @param data Contains all trips vertices.
     * @param verticesCount Number of vertices to draw.
     */
    void loadTripsData(const QVector<float>& data, unsigned int verticesCount);

    /**
     * @brief loadStationsData Creates VAO if its not created and send all
     * vertices to be rendered.
     * @param data Contains all stations vertices.
     * @param verticesCount Number of vertices to draw.
     */
    void loadStationsData(const QVector<float>& data, unsigned int verticesCount);

    /**
     * @brief centerView Centers the view on stations and trips with an
     * appropriate zoom.
     * @param data Iterates over all data to calculate the centered view.
     */
    void centerView(const QVector<float>& data);


protected:
    /**
     * @brief initializeGL Initialize OpenGL functions and check shaders errors.
     */
    virtual void initializeGL() override;

    /**
     * @brief resizeGL If the widget is resized, it updates the new width and
     * height
     * @param width New width.
     * @param height New height;
     */
    virtual void resizeGL(int width, int height) override;

    /**
     * @brief paintGL Main loop, draws all Stations and Trips, its where
     * drawStations and drawTrips are called.
     */
    virtual void paintGL() override;

    /**
     * @brief wheelEvent Called every time the user moves the mouse wheel.
     * @param event Gives informations about the action.
     */
    virtual void wheelEvent(QWheelEvent* event) override;

    /**
     * @brief mouseMoveEvent Called every time the user moves the mouse.
     * @param event Gives informations about the action.
     */
    virtual void mouseMoveEvent(QMouseEvent* event) override;

    /**
     * @brief mousePressEvent Called every time the user presses mouse buttons.
     * @param event Gives informations about the action.
     */
    virtual void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief mouseReleaseEvent Called every time the user releases mouse
     * buttons.
     * @param event Gives informations about the action.
     */
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

private:
    /**
     * @brief drawStations Sends variables to shaders and draw stations
     * if m_drawStations is true.
     */
    void drawStations();

    /**
     * @brief drawTrips Sends variables to shaders and draw stations
     * if m_drawTrips is true.
     */
    void drawTrips();

    /**
     * @brief calculateTranslation Calculates the x and y value of translation
     * to center the view.
     */
    void calculateTranslation();

    /**
     * @brief calculateZoom Calculates the zoom value to see all stations and
     * trips on screen.
     */
    void calculateZoom();

    /**
     * @brief calculateBoundingBoxStations Calculates the bounding box of all
     * data passed in parameter and sets m_boundingBoxStations to the correct
     * bounding box value.
     * @param data Represents all the vertices to compute the bounding box
     */
    void calculateBoundingBoxStations(const QVector<float>& data);

    /**
     * @brief initializeShaderTrips Loads, compiles and links trips shaders,
     * return <i>false</i> if something went wrong, <i>true</i> otherwise.
     * @return <i>false</i> if loading, compilation or linkage went wrong,
     * <i>true</i> if success.
     */
    bool initializeShaderTrips();

    /**
     * @brief initializeShaderStations Loads, compiles and links stations
     * shaders, return <i>falses</i> if something went wrong, <i>true</i>
     * otherwise.
     * @return <i>false</i> if loading, compilation or linkage went wrong,
     * <i>true</i> if success.
     */
    bool initializeShaderStations();



    /**
     * @brief m_translationSensibility Sensibility of the translation (on X and Y)
     * when user does drag and move in the widget.
     */
    float m_translationSensibility;

    /**
     * @brief m_boundingBoxStations Its the bounding box of of all trips or/and
     * stations drawn in the widget.
     */
    QRectF m_boundingBoxStations;

    /**
     * @brief m_zoom Its the zoom value, greater is the value, more the zoom
     * on the map is important. Value can't be under 0.
     */
    float m_zoom;

    /**
     * @brief m_leftMouseButtonPressed If the left mouse button is pressed
     * <i>true</i>, <i>false</i> if its released.
     */
    bool m_leftMouseButtonPressed;

    /**
     * @brief m_translationX Its the translation value on X axis.
     */
    float m_translationX;

    /**
     * @brief m_translationY Its the translation value on Y axis
     */
    float m_translationY;

    /**
     * @brief m_previousMousePos Used to calculate the translation, in
     * <i>MapGLWidget::mouseMoveEvent</i> fonction. Its the previous position
     * of the cursor of the mouse.
     */
    QPointF m_previousMousePos;

    /**
     * @brief m_shaderProgramStations Its the shader for all stations.
     */
    QOpenGLShaderProgram* m_shaderProgramStations;

    /**
     * @brief m_shaderProgramTrips Its the shader for all stations.
     */
    QOpenGLShaderProgram* m_shaderProgramTrips;

    /**
     * @brief m_stationRenderer Object that draws all stations on the widget.
     */
    StationRenderer* m_stationRenderer;

    /**
     * @brief m_tripRenderer Object that draws all trips on the widget.
     */
    TripRenderer* m_tripRenderer;

    /**
     * @brief m_drawStations If its set to <i>true</i> this means data has
     * been sent and the program has to draw stations, if <i>false</i> we dont
     * draw stations.
     */
    bool m_drawStations;

    /**
     * @brief m_drawTrips If its set to <i>true</i> this means data has
     * been sent and the program has to draw trips, if <i>false</i> we dont
     * draw trips.
     */
    bool m_drawTrips;

    /**
     * @brief m_isStationsVAOCreated If <i>true</i> the stations VAO is created,
     * <i>false</i> otherwise.
     */
    bool m_isStationsVAOCreated;

    /**
     * @brief m_isTripsVAOCreated If <i>true</i> the trips VAO is created,
     * <i>false</i> otherwise.
     */
    bool m_isTripsVAOCreated;

    /**
     * @brief m_isTripsShaderCreated If <i>true</i> the trips shaders are created,
     * <i>false</i> otherwise.
     */
    bool m_isTripsShadersCreated;

    /**
     * @brief m_isStationsShaderCreated If <i>true</i> the stations shaders are
     * created, <i>false</i> otherwise.
     */
    bool m_isStationsShadersCreated;

    /**
     * @brief m_stationsPosition Stations positions, we need them to know wich
     * station is selected on the map.
     * This features is not implemented.
     */
    QVector<float> m_stationsPosition;

signals:
    /**
     * @brief onShaderError If an error occured during loading, compilation or
     * linking, this signal will be emitted.
     * @param message Message informations of errors.
     */
    void onShaderError(const QString& message);
};

#endif // OPENGLWIDGET_H

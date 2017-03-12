#include "mapglwidget.h"
#include <QGuiApplication>

#include <QElapsedTimer>
#include <QList>
#include <QPainter>
#include <QOpenGLShaderProgram>
#include <QDebug>
#include <QtGlobal>


#include "stationrenderer.h"


MapGLWidget::MapGLWidget(QWidget* p) : QOpenGLWidget(p)
{
    m_zoom = 1.0f;
    m_leftMouseButtonPressed = false;
    m_translationOffsetX = 0.f;
    m_translationOffsetY = 0.f;

    m_stationsLoaded = false;
    m_tripsLoaded = false;

    m_isStationsVAOCreated = false;
    m_isTripsVAOCreated = false;

    m_shaderProgramStations = Q_NULLPTR;
    m_shaderProgramTrips = Q_NULLPTR;

    m_stationRenderer = Q_NULLPTR;
    m_tripRenderer = Q_NULLPTR;

    m_stationRenderer = new StationRenderer();
    m_tripRenderer = new TripRenderer();
}

MapGLWidget::~MapGLWidget()
{
    if (m_shaderProgramStations)
        delete m_shaderProgramStations;

    if (m_shaderProgramTrips)
        delete m_shaderProgramTrips;

    if (m_stationRenderer)
        delete m_stationRenderer;

    if (m_tripRenderer)
        delete m_tripRenderer;
}

void MapGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    this->makeCurrent();

    glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    glEnable(GL_MULTISAMPLE);

    m_shaderProgramStations = new QOpenGLShaderProgram(this->context());
    m_shaderProgramStations->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/shaders/map.vert");
    m_shaderProgramStations->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/shaders/stations.frag");
    m_shaderProgramStations->link();
    m_shaderProgramStations->bind();
    m_shaderProgramStations->release();

    m_shaderProgramTrips = new QOpenGLShaderProgram();
    m_shaderProgramTrips->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/shaders/map.vert");
    //    m_shaderProgramTrips->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/Shaders/shaders/curves.geom");
    m_shaderProgramTrips->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/shaders/trips.frag");
    m_shaderProgramTrips->link();
    m_shaderProgramTrips->bind();
    m_shaderProgramTrips->release();

    qDebug() << "initializeGL OpenGL version:" << this->format().version();
}

void MapGLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}

void MapGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_stationsLoaded)
        drawStations();

    if (m_tripsLoaded)
        drawTrips();
}

void MapGLWidget::drawStations()
{
    m_shaderProgramStations->bind();

    int zoomLoc = m_shaderProgramStations->uniformLocation("zoom");
    glUniform1f(zoomLoc, m_zoom);
    int translationLoc = m_shaderProgramStations->uniformLocation("translation");
    glUniform2f(translationLoc, m_translationOffsetX, m_translationOffsetY);

    m_stationRenderer->draw();

    m_shaderProgramStations->release();
}

void MapGLWidget::drawTrips()
{
    m_shaderProgramTrips->bind();

    int zoomLoc = m_shaderProgramTrips->uniformLocation("zoom");
    glUniform1f(zoomLoc, m_zoom);
    int translationLoc = m_shaderProgramTrips->uniformLocation("translation");
    glUniform2f(translationLoc, m_translationOffsetX, m_translationOffsetY);

    m_tripRenderer->draw();

    m_shaderProgramTrips->release();
}

void MapGLWidget::loadStationsData(const QVector<float>& data, unsigned int verticesCount)
{
    if (!m_isStationsVAOCreated)
    {
        m_isStationsVAOCreated = true;
        m_stationRenderer->createVAO();
        m_stationRenderer->initGLFunc();
    }

    m_stationsLoaded = true;
    m_stationRenderer->sendData(data, verticesCount);

    update();
}

void MapGLWidget::loadTripsData(const QVector<float>& data, unsigned int verticesCount)
{
    if (!m_isTripsVAOCreated)
    {
        m_isTripsVAOCreated = true;
        m_tripRenderer->initGLFunc();
        m_tripRenderer->createVAO();
    }

    m_tripsLoaded = true;
    m_tripRenderer->sendData(data, verticesCount);

    update();
}

void MapGLWidget::centerView(const QVector<float>& data)
{
    calculateBoundingBoxStations(data);
    calculateTranslation();
    calculateZoom();
}

void MapGLWidget::calculateBoundingBoxStations(const QVector<float>& data)
{
    float minLatitude = 90;
    float maxLatitude = -90.f;
    float minLongitude = 180.f;
    float maxLongitude = -180.f;

    // Iterate on x
    for (int i = 0; i < data.size(); i += 2)
    {
        minLongitude = qMin(minLongitude, data[i]);
        maxLongitude = qMax(maxLongitude, data[i]);
    }

    // Iterate on y
    for (int i = 1; i < data.size(); i += 2)
    {
        minLatitude = qMin(minLatitude, data[i]);
        maxLatitude = qMax(maxLatitude, data[i]);
    }


    m_boundingBoxStations = QRectF(minLongitude, maxLatitude,
                                   maxLongitude - minLongitude,
                                   maxLatitude - minLatitude);
    //    qDebug() << m_boundingBoxStations;
}

void MapGLWidget::calculateTranslation()
{
    QPointF center = m_boundingBoxStations.center();

    m_translationOffsetX = -center.x();
    m_translationOffsetY = center.y();
}

void MapGLWidget::calculateZoom()
{
    float x = qMax(m_boundingBoxStations.width(), m_boundingBoxStations.height());

    if (x != 0)
    {
        // OpenGL coordinates from -1 to 1
        const int coordinateSystemLength = 2;

        // FIXME: comprendre pourquoi le zoom n'est pas centré sur le centre
        // de la bounding box
        // Cette valeur permet de reduire le zoom et affiche toute la bounding box
        const float debugValue = 2.f;
        m_zoom = coordinateSystemLength / x / debugValue;
    }
}

void MapGLWidget::wheelEvent(QWheelEvent* event)
{
    m_zoom += event->delta();

    const float minZoom = 5.f;

    if (m_zoom < minZoom)
        m_zoom = minZoom;

    qDebug() << "zoom" << m_zoom;

    qDebug() <<  "Wheel event";
    update();
    event->accept();
}

void MapGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_leftMouseButtonPressed)
    {
        QPointF currentPos = event->pos();
        m_translationOffsetX += 1.f / m_zoom * (currentPos.x() - m_previousMousePos.x()) / m_translationSensibility;
        m_translationOffsetY += 1.f / m_zoom * (currentPos.y() - m_previousMousePos.y()) / m_translationSensibility;

        m_previousMousePos = event->pos();
        update();
        event->accept();
    }
}

void MapGLWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        QGuiApplication::setOverrideCursor(Qt::ClosedHandCursor);
        m_leftMouseButtonPressed = true;
        m_previousMousePos = event->pos();
        qDebug() <<  "press";
        event->accept();
    }
}

void MapGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        QGuiApplication::restoreOverrideCursor();
        m_leftMouseButtonPressed = false;
        qDebug() <<  "release";
        event->accept();
    }
}

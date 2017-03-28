#include "mapglwidget.h"
#include <QGuiApplication>
#include <QOpenGLShaderProgram>
#include <QDebug>
#include <QtGlobal>

#include "stationrenderer.h"
#include "triprenderer.h"

MapGLWidget::MapGLWidget(QWidget* p) : QOpenGLWidget(p)
{
    m_zoom = 1.0f;
    m_translationSensibility = this->width();
    m_leftMouseButtonPressed = false;
    m_translationX = 0.f;
    m_translationY = 0.f;

    m_drawStations = false;
    m_drawTrips = false;

    m_isStationsVAOCreated = false;
    m_isTripsVAOCreated = false;

    m_shaderProgramStations = Q_NULLPTR;
    m_shaderProgramTrips = Q_NULLPTR;

    m_stationRenderer = new StationRenderer();
    m_tripRenderer = new TripRenderer();
}

MapGLWidget::~MapGLWidget()
{
    delete m_shaderProgramStations;
    delete m_shaderProgramTrips;
    delete m_stationRenderer;
    delete m_tripRenderer;
}



void MapGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    this->makeCurrent();

    glClearColor(0.2f, 0.2f, 0.2f, 1.f);

    if (!initializeShaderStations())
        onShaderError(m_shaderProgramStations->log());

    if (!initializeShaderTrips())
        onShaderError(m_shaderProgramTrips->log());

    qDebug() << "MapGLWidget::initializeGL() OpenGL version:" << this->format().version();
}

void MapGLWidget::resizeGL(int width, int height)
{
    m_translationSensibility = width;
    glViewport(0, 0, width, height);
}

void MapGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    drawStations();
    drawTrips();
}



void MapGLWidget::drawStations()
{
    if (m_drawStations)
    {
        m_shaderProgramStations->bind();

        const int zoomLoc = m_shaderProgramStations->uniformLocation("zoom");
        glUniform1f(zoomLoc, m_zoom);

        const int translationLoc = m_shaderProgramStations->uniformLocation("translation");
        glUniform2f(translationLoc, m_translationX, m_translationY);

        m_stationRenderer->draw();

        m_shaderProgramStations->release();
    }
}

void MapGLWidget::drawTrips()
{
    if (m_drawTrips)
    {
        m_shaderProgramTrips->bind();

        const int zoomLoc = m_shaderProgramTrips->uniformLocation("zoom");
        glUniform1f(zoomLoc, m_zoom);

        const int translationLoc = m_shaderProgramTrips->uniformLocation("translation");
        glUniform2f(translationLoc, m_translationX, m_translationY);

        m_tripRenderer->draw();

        m_shaderProgramTrips->release();
    }
}

void MapGLWidget::clear()
{
    m_drawStations = false;
    m_drawTrips = false;
    update();
}

void MapGLWidget::loadStationsData(const QVector<float>& data, unsigned int verticesCount)
{
    if (!m_isStationsVAOCreated)
    {
        m_isStationsVAOCreated = true;
        m_stationRenderer->initGLFunc();
        m_stationRenderer->createVAO();
    }

    m_drawStations = true;
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

    m_drawTrips = true;
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

    // 2 for x,y and 3 for R,G,B
    const int vertexSize = 5;

    // Find min and max longitude (X)
    for (int i = 0; i < data.size(); i += vertexSize)
    {
        minLongitude = qMin(minLongitude, data[i]);
        maxLongitude = qMax(maxLongitude, data[i]);
    }

    // Find min and max latitude (Y)
    for (int i = 1; i < data.size(); i += vertexSize)
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

    m_translationX = -center.x();
    m_translationY = center.y();
}

void MapGLWidget::calculateZoom()
{
    float x = qMax(m_boundingBoxStations.width(), m_boundingBoxStations.height());

    if (x != 0)
    {
        // OpenGL coordinates system from -1 to 1
        const int coordinateSystemLength = 2;

        // FIXME: DAMIEN: comprendre pourquoi le zoom n'est pas centré sur le centre
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

    update();
    event->accept();
}

void MapGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_leftMouseButtonPressed)
    {
        QPointF currentPos = event->pos();

        // FIXME: DAMIEN : refactoriser plus court
        m_translationX += 1.f / m_zoom * (currentPos.x() - m_previousMousePos.x()) / m_translationSensibility;
        m_translationY += 1.f / m_zoom * (currentPos.y() - m_previousMousePos.y()) / m_translationSensibility;

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
        //qDebug() <<  "left mouse button pressed";
        event->accept();
    }
}

void MapGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        QGuiApplication::restoreOverrideCursor();
        m_leftMouseButtonPressed = false;
        //        qDebug() <<  "left mouse button released";
        event->accept();
    }
}

bool MapGLWidget::initializeShaderTrips()
{
    bool initializationIsOk = true;

    m_shaderProgramTrips = new QOpenGLShaderProgram();
    initializationIsOk &= m_shaderProgramTrips->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/shaders/trips.vert");
    initializationIsOk &= m_shaderProgramTrips->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/Shaders/shaders/trips.geom");
    initializationIsOk &= m_shaderProgramTrips->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/shaders/trips.frag");
    initializationIsOk &= m_shaderProgramTrips->link();
    initializationIsOk &= m_shaderProgramTrips->bind();
    m_shaderProgramTrips->release();

    return initializationIsOk;
}

bool MapGLWidget::initializeShaderStations()
{
    bool initializationIsOk = true;

    m_shaderProgramStations = new QOpenGLShaderProgram(this->context());
    initializationIsOk &= m_shaderProgramStations->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/shaders/stations.vert");
    initializationIsOk &= m_shaderProgramStations->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/shaders/stations.frag");
    initializationIsOk &= m_shaderProgramStations->link();
    initializationIsOk &= m_shaderProgramStations->bind();
    m_shaderProgramStations->release();

    return initializationIsOk;
}

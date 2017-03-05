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

    m_stationRenderer = new StationRenderer();
}

MapGLWidget::~MapGLWidget()
{
    m_tripsVBO.destroy();
    m_tripsVAO.destroy();
    delete m_shaderProgramStations;
    delete m_shaderProgramTrips;
    delete m_stationRenderer;
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

    m_tripsVBO.create();
    m_tripsVBO.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_tripsVBO.bind();

    m_tripsVAO.create();
    m_tripsVAO.bind();
    m_tripsVAO.release();
    m_tripsVBO.release();
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

    drawStations();
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

    m_tripsVAO.bind();
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_tripsVBO.IndexBuffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_tripsVBO.IndexBuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    int zoomLoc = m_shaderProgramTrips->uniformLocation("zoom");
    glUniform1f(zoomLoc, m_zoom);
    int translationLoc = m_shaderProgramTrips->uniformLocation("translation");
    glUniform2f(translationLoc, m_translationOffsetX, m_translationOffsetY);
    glDrawArrays(GL_LINES, 0, m_tripsVerticesCount);
    m_tripsVAO.release();

    m_shaderProgramTrips->release();
}


void MapGLWidget::loadStationsData(const QVector<float> data, unsigned int verticesCount)
{
    initializeOpenGLFunctions();
    m_stationRenderer->createVAO();
    m_stationRenderer->sendData(data, verticesCount);
    update();
}

void MapGLWidget::loadTripsData(const QVector<float> data, unsigned int verticesCount)
{
    initializeOpenGLFunctions();
    m_tripsVerticesCount= verticesCount;

    glBindBuffer(GL_ARRAY_BUFFER, m_tripsVBO.IndexBuffer);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    update();
}

void MapGLWidget::centerView(const QVector<float> data)
{
    calculateBoundingBoxStations(data);
    calculateTranslation();
    calculateZoom();
}

void MapGLWidget::calculateBoundingBoxStations(const QVector<float> data)
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
        int coordinateSystemLength = 2;

        // FIXME: comprendre pourquoi le zoom n'est pas centré sur le centre
        // de la bounding box
        // Cette valeur permet de reduire le zoom et affiche toute la bounding box
        int debugValue = 1.2;
        m_zoom = coordinateSystemLength / x / debugValue;
    }
}

void MapGLWidget::wheelEvent(QWheelEvent* event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
    m_zoom += numSteps * 1.01f;

    if (m_zoom < 0.125f)
        m_zoom = 0.125f;

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

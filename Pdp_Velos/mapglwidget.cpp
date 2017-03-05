#include "mapglwidget.h"
#include <QGuiApplication>

#include <QElapsedTimer>
#include <QList>
#include <QPainter>
#include <QOpenGLShaderProgram>
#include <QDebug>
#include <QtGlobal>

#include "station.h"
#include "trip.h"


MapGLWidget::MapGLWidget(QWidget* p) : QOpenGLWidget(p)
{
    m_zoom = 1.0f;
    m_leftMouseButtonPressed = false;
    m_translationOffsetX = 0.f;
    m_translationOffsetY = 0.f;
}

MapGLWidget::~MapGLWidget()
{
    m_tripsVBO.destroy();
    m_tripsVAO.destroy();
    m_stationsVBO.destroy();
    m_stationsVAO.destroy();
    delete m_shaderProgramStations;
    delete m_shaderProgramTrips;
}

void MapGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    this->makeCurrent();
    glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    glEnable(GL_MULTISAMPLE);

    m_shaderProgramStations = new QOpenGLShaderProgram(this->context());
    m_shaderProgramStations->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/Shaders/map.vert");
    m_shaderProgramStations->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/Shaders/stations.frag");
    m_shaderProgramStations->link();
    m_shaderProgramStations->bind();

    m_stationsVBO.create();
    m_stationsVBO.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_stationsVBO.bind();

    m_stationsVAO.create();
    m_stationsVAO.bind();
    m_stationsVAO.release();
    m_stationsVBO.release();
    m_shaderProgramStations->release();


    m_shaderProgramTrips = new QOpenGLShaderProgram();
    m_shaderProgramTrips->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/Shaders/map.vert");
    //    m_shaderProgramTrips->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/Shaders/Shaders/curves.geom");
    m_shaderProgramTrips->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/Shaders/trips.frag");
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

    m_stationsVAO.bind();
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_stationsVBO.IndexBuffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    int zoomLoc = m_shaderProgramStations->uniformLocation("zoom");
    glUniform1f(zoomLoc, m_zoom);
    int translationLoc = m_shaderProgramStations->uniformLocation("translation");
    glUniform2f(translationLoc, m_translationOffsetX, m_translationOffsetY);
    glPointSize(5.f);
    glDrawArrays(GL_POINTS, 0, m_stationsVerticesCount);
    m_stationsVAO.release();

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
    glPointSize(5.f);
    glDrawArrays(GL_LINES, 0, m_tripsVerticesCount);
    m_tripsVAO.release();

    m_shaderProgramTrips->release();
}

void MapGLWidget::loadTripsAndStations(QVector<const Trip*>& trips, QVector<const Station*>& stations)
{    
    // for x and y;
    int tupleSize = 2;

    //*************************************************************************
    // STATIONS
    // ************************************************************************

    m_stationsVertices.reserve(stations.size() * tupleSize);
    m_stationsVertices.clear();
    m_stationsVerticesCount = stations.size();

    for (const Station* s : stations)
    {
        m_stationsVertices.push_back(s->longitude() / 180.f);
        m_stationsVertices.push_back(s->latitude() / 90.f);
    }

    //        qDebug() << m_stationsVertices[i++] << m_stationsVertices[i++];

    calculateBoundingBoxStations();
    calculateTranlsation();
    calculalteZoom();

    initializeOpenGLFunctions();
    this->makeCurrent();

    m_stationsVBO.bind();
    glBindBuffer(GL_ARRAY_BUFFER, m_stationsVBO.IndexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_stationsVertices.size() * sizeof(float),
                 m_stationsVertices.data(), GL_STATIC_DRAW);
    m_stationsVBO.release();

    //*************************************************************************
    // END STATIONS
    // ************************************************************************

    //*************************************************************************
    // TRIPS
    // ************************************************************************

    // 1 trip has 2 points (start/end)
    int vertexPerTrip = 2;
    int tupleColor = 3;

    m_tripsVertices.reserve(trips.size() * tupleSize * vertexPerTrip * tupleColor);
    m_tripsVerticesCount = trips.size() * vertexPerTrip;

    for (const Trip* t : trips)
    {
<<<<<<< .mine
        /*m_tripsVertices.push_back(trips.at(i).getStartStation()->getLongitude() / 180.f);
        m_tripsVertices.push_back(trips.at(i).getStartStation()->getLatitude() / 90.f);
        m_tripsVertices.push_back(trips.at(i).getEndStation()->getLongitude() / 180.f);
        m_tripsVertices.push_back(trips.at(i).getEndStation()->getLatitude() / 90.f);*/
=======
        m_tripsVertices.append((float)(t->getStartStation()->getLongitude() / 180.f));
        m_tripsVertices.append((float)(t->getStartStation()->getLatitude() / 90.f));

        m_tripsVertices.append(1.f);
        m_tripsVertices.append(0.f);
        m_tripsVertices.append(0.f);

        m_tripsVertices.append((float)(t->getEndStation()->getLongitude() / 180.f));
        m_tripsVertices.append((float)(t->getEndStation()->getLatitude() / 90.f));

        m_tripsVertices.append(0.f);
        m_tripsVertices.append(0.f);
        m_tripsVertices.append(1.f);

        //        qDebug() << m_tripsVertices[i++] << m_tripsVertices[i++] << m_tripsVertices[i++] << m_tripsVertices[i++];
>>>>>>> .r41
    }

    m_tripsVBO.bind();
    glBindBuffer(GL_ARRAY_BUFFER, m_tripsVBO.IndexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_tripsVertices.size() * sizeof(float),
                 m_tripsVertices.data(), GL_STATIC_DRAW);
    m_tripsVBO.release();

    //*************************************************************************
    // END TRIPS
    // ************************************************************************
    update();
}

void MapGLWidget::calculateBoundingBoxStations()
{
    float minLatitude = 90;
    float maxLatitude = -90.f;
    float minLongitude = 180.f;
    float maxLongitude = -180.f;

    for (int i = 0; i < m_stationsVertices.size(); ++i)
    {
        if (i % 2 == 0)
        {
            minLongitude = qMin(minLongitude, m_stationsVertices[i]);
            maxLongitude = qMax(maxLongitude, m_stationsVertices[i]);
        }
        else
        {
            minLatitude = qMin(minLatitude, m_stationsVertices[i]);
            maxLatitude = qMax(maxLatitude, m_stationsVertices[i]);
        }
    }

    m_boundingBoxStations = QRectF(minLongitude, maxLatitude,
                                   maxLongitude - minLongitude,
                                   maxLatitude - minLatitude);
    //    qDebug() << m_boundingBoxStations;
}

void MapGLWidget::calculateTranlsation()
{
    QPointF center = m_boundingBoxStations.center();

    m_translationOffsetX = -center.x();
    m_translationOffsetY = center.y();
}

void MapGLWidget::calculalteZoom()
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

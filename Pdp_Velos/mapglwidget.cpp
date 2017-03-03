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
}

void MapGLWidget::initializeGL()
{
    if (!this->isValid())
    {
        initializeOpenGLFunctions();
        qDebug() << "initializeGL OpenGL version:" << this->format().version();
    }

    glClearColor(0.2f, 0.2f, 0.2f, 1.f);

    m_shaderProgramStations = new QOpenGLShaderProgram(this->context());
    m_shaderProgramStations->addShaderFromSourceFile(QOpenGLShader::Vertex, "../vert.vert");
    m_shaderProgramStations->addShaderFromSourceFile(QOpenGLShader::Fragment, "../frag.frag");
    m_shaderProgramStations->link();
    m_shaderProgramStations->bind();


    m_tripsVBO.create();
    m_tripsVBO.bind();
    m_tripsVBO.setUsagePattern(QOpenGLBuffer::StaticDraw);

    m_tripsVAO.create();
    m_tripsVAO.bind();
    m_tripsVAO.release();
    m_tripsVBO.release();

    m_stationsVBO.create();
    m_stationsVBO.bind();
    m_stationsVBO.setUsagePattern(QOpenGLBuffer::StaticDraw);

    m_stationsVAO.create();
    m_stationsVAO.bind();
    m_stationsVAO.release();
    m_stationsVBO.release();

    m_shaderProgramStations->release();


    if (this->isValid())
    {
        qDebug() << "initializeGL OpenGL version:" << this->format().version();
    }
}

void MapGLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}

void MapGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    drawStations();
    //    drawTrips();
}

void MapGLWidget::drawStations()
{
    m_stationsVAO.bind();
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_stationsVBO.IndexBuffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    m_stationsVAO.release();

    m_shaderProgramStations->bind();
    m_stationsVAO.bind();
    qDebug() << "zoom" << m_zoom;
    //        int zoomLoc = m_shaderProgramStations->uniformLocation("zoom");
    //        glUniform1f(zoomLoc, m_zoom);
    //        int translationLoc = m_shaderProgramStations->uniformLocation("translation");
    //        glUniform2f(translationLoc, m_translationOffsetX, m_translationOffsetY);
    glPointSize(5.f);
    glDrawArrays(GL_POINTS, 0, m_stationsVerticesCount);
    m_stationsVAO.release();
    m_shaderProgramStations->release();
}

void MapGLWidget::drawTrips()
{
    m_tripsVAO.bind();
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_tripsVBO.IndexBuffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    m_tripsVAO.release();

    m_shaderProgramStations->bind();
    m_tripsVAO.bind();
    int zoomLoc = m_shaderProgramStations->uniformLocation("zoom");
    glUniform1f(zoomLoc, m_zoom);
    int translationLoc = m_shaderProgramStations->uniformLocation("translation");
    glUniform2f(translationLoc, m_translationOffsetX / 500, m_translationOffsetY / 500);
    glDrawArrays(GL_LINES, 0, m_tripsVerticesCount);
    m_tripsVAO.release();
    m_shaderProgramStations->release();
}

void MapGLWidget::onDataLoaded(const QList<Station>& stations, const QList<Trip>& trips)
{
    m_stationsVertices.reserve(stations.size() * 2);
    m_stationsVertices.clear();
    m_stationsVerticesCount = stations.size();

    for (int i = 0; i < stations.size(); ++i)
    {
        m_stationsVertices.push_back(stations.at(i).getLongitude() / 180.f);
        m_stationsVertices.push_back(stations.at(i).getLatitude() / 90.f);
    }

    if (!this->isValid())
    {
        initializeOpenGLFunctions();
        this->makeCurrent();
        qDebug() << "onDataLoaded OpenGL version:" << this->format().version();
    }

    //    calculateBoundingBoxStations(stations);
    //    calculateTranlsation();
    //    calculalteZoom();

    m_stationsVBO.bind();
    glBindBuffer(GL_ARRAY_BUFFER, m_stationsVBO.IndexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_stationsVertices.size() * sizeof(float), m_stationsVertices.data(), GL_STATIC_DRAW);
    m_stationsVBO.release();

    qDebug() << "Number of stations" << stations.size();

    m_tripsVertices.reserve(trips.size() * 2 * 2);
    m_tripsVerticesCount = trips.size() * 2;

    for (int i = 0; i < trips.size(); ++i)
    {
        m_tripsVertices.push_back(trips.at(i).getStartStation()->getLongitude() / 180.f);
        m_tripsVertices.push_back(trips.at(i).getStartStation()->getLatitude() / 90.f);
        m_tripsVertices.push_back(trips.at(i).getEndStation()->getLongitude() / 180.f);
        m_tripsVertices.push_back(trips.at(i).getEndStation()->getLatitude() / 90.f);
    }

    m_tripsVBO.bind();
    glBindBuffer(GL_ARRAY_BUFFER, m_tripsVBO.IndexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_tripsVertices.size() * sizeof(float), m_tripsVertices.data(), GL_STATIC_DRAW);
    m_tripsVBO.release();

    qDebug() << "Number of trips" << trips.size();
    update();
}

void MapGLWidget::calculateBoundingBoxStations(const QList<Station>& stations)
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

    m_boundingBoxStations = QRectF(minLongitude, maxLatitude, maxLongitude - minLongitude, maxLatitude - minLatitude);
    qDebug() << minLongitude<< maxLatitude<< maxLongitude<< minLatitude;
}

void MapGLWidget::calculateTranlsation()
{
    QPointF center = m_boundingBoxStations.center();

    m_translationOffsetX = -center.x();
    m_translationOffsetY = center.y();
}

void MapGLWidget::calculalteZoom()
{
    float x = qMin(m_boundingBoxStations.width(), m_boundingBoxStations.height());

    qDebug() << "Bounding box" << m_boundingBoxStations << x;

    if (x != 0)
    {
        m_zoom = 2 / x/100;
    }
    qDebug() << "zoom" << m_zoom;
}

void MapGLWidget::wheelEvent(QWheelEvent* event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
    m_zoom += numSteps * 1.01f;

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
        qDebug() <<  "move";
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

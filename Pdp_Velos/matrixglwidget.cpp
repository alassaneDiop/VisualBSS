#include "matrixglwidget.h"

#include <QGuiApplication>
#include <QtMath>
#include <QDebug>
#include <QWheelEvent>
#include <QPainter>
#include <QElapsedTimer>

#include "trip.h"
#include "station.h"


MatrixGLWidget::MatrixGLWidget(QWidget* p) : QOpenGLWidget(p)
{
    m_matrixViewWidth = this->width() - 2 * m_matrixOffsetX;

    m_drawRectangle = false;
    m_leftMouseButtonPressed = false;
    m_translationValue = 0;
    m_dragSelectionBorderWidth = 1;
}

MatrixGLWidget::~MatrixGLWidget()
{

}

void MatrixGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1.f, 1.f, 1.f, 1.f);
    qDebug() << "MatrixGLWidget::initializeGL() OpenGL version:" << this->format().version();
}

void MatrixGLWidget::resizeGL(int width, int height)
{
    m_matrixViewWidth = this->width() - 2 * m_matrixOffsetX;
}

void MatrixGLWidget::paintGL()
{
//    drawDirections();
//    drawDragSelection();
    QPainter p;
    p.begin(this);
    p.fillRect(QRect(0, 0, 10, 10), QBrush(QColor(10, 10, 210, 128)));
    p.end();
    update();
}

void MatrixGLWidget::drawDragSelection()
{
    QPainter painter;
    painter.begin(this);

    QRect rectangle(m_topLeftSelectionRectangle.x(),
                    m_topLeftSelectionRectangle.y() + m_translationValue,
                    m_bottomRightSelectionRectangle.x() - m_topLeftSelectionRectangle.x(),
                    m_bottomRightSelectionRectangle.y() - m_topLeftSelectionRectangle.y());

    if (m_drawRectangle)
    {
        QElapsedTimer timer;
        timer.start();
        painter.fillRect(rectangle, QBrush(QColor(10, 10, 210, 128)));

        QPen pen(Qt::black, m_dragSelectionBorderWidth);
        painter.setPen(pen);

        painter.drawRect(rectangle);
        qDebug() << "drawDragSelection: The slow operation took" << timer.elapsed() << "milliseconds";
    }
    painter.end();
}

void MatrixGLWidget::drawDirections()
{
    QPainter painter(this);
    int intervalLength = m_matrixViewWidth / m_numberOfInterval;

    QElapsedTimer timer;
    timer.start();

    for (QPoint i : m_ellipses)
    {
        painter.setBrush(QColor(100, 100, 100, 128));
        painter.drawEllipse(QPoint(m_matrixOffsetX + i.x() * intervalLength + (m_stationCircleSize / 2),
                                   m_translationValue + i.y() + m_stationCircleSize),
                            m_stationCircleSize, m_stationCircleSize);
    }


    qDebug() << "drawDirections: The slow operation took" << timer.elapsed() << "milliseconds";
}

void MatrixGLWidget::loadTripsAndStations(QVector<const Trip*>& trips, QVector<const Station*>& stations)
{
    for (const Station* s: stations)
    {
        m_stations.append(s);
    }
    qDebug() << "m_stations size " << m_stations.size();

    for (const Trip* t: trips)
    {
        m_trips.append(t);
    }

    qDebug() << "m_trip size" << m_trips.size();
    initPoint();
    update();
}

void MatrixGLWidget::wheelEvent(QWheelEvent* event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
    m_translationValue += numSteps * 10;

    if (m_translationValue > 0)
        m_translationValue = 0;

    event->accept();
    update();
    qDebug() <<  "Wheel event";
}

void MatrixGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_leftMouseButtonPressed)
    {
        m_previousMousePos = event->pos();
        m_previousMousePos.setY(m_previousMousePos.y() - m_translationValue);
        m_bottomRightSelectionRectangle = event->pos();
        m_bottomRightSelectionRectangle.setY(
                    m_bottomRightSelectionRectangle.y() - m_translationValue);

        std::vector<QPoint> out = hit();
        qDebug() <<  "out size : " << out.size();

        update();
        event->accept();
    }
    qDebug() <<  "move";
}

void MatrixGLWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        QGuiApplication::setOverrideCursor(Qt::CrossCursor);
        m_drawRectangle = true;
        m_leftMouseButtonPressed = true;
        m_previousMousePos = event->pos();
        m_previousMousePos.setY(m_previousMousePos.y() - m_translationValue);
        m_topLeftSelectionRectangle = event->pos();
        m_topLeftSelectionRectangle.setY(
                    m_topLeftSelectionRectangle.y() - m_translationValue);
        event->accept();
        qDebug() <<  "left mouse button pressed";
    }
    else if (event->button() == Qt::RightButton)
    {
        m_drawRectangle = false;
        event->accept();
        update();
        qDebug() <<  "right mouse button pressed";
    }
}

void MatrixGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        QGuiApplication::restoreOverrideCursor();
        m_leftMouseButtonPressed = false;
        event->accept();
        qDebug() <<  "left mouse button released";
    }
}

void MatrixGLWidget::initPoint()
{
    int j = 0;
    for (const Station* s : m_stations)
    {
        j++;
        for (int i = 0; i < 24; ++i)
        {
            m_ellipses.push_back(QPoint(i, j * 30));
        }
    }
}

std::vector<QPoint> MatrixGLWidget::hit()
{
    int intervalLength = m_matrixViewWidth / m_numberOfInterval;
    std::vector<QPoint> outEllipses;

    QRect rectangle(m_topLeftSelectionRectangle.x(),
                    m_topLeftSelectionRectangle.y() + m_translationValue,
                    m_bottomRightSelectionRectangle.x() - m_topLeftSelectionRectangle.x(),
                    m_bottomRightSelectionRectangle.y() - m_topLeftSelectionRectangle.y());

    for (QPoint i : m_ellipses)
    {
        int x = m_matrixOffsetX + i.x() * intervalLength + (m_stationCircleSize / 2);
        int y = m_translationValue + i.y() + m_stationCircleSize;

        if(rectangle.contains(x,y)) outEllipses.push_back(i);
    }
    return outEllipses;
}

#include "map.h"
#include <QGuiApplication>

Map::Map(QWidget* p) : QOpenGLWidget(p)
{
    m_zoom = 1.f;
    m_leftMouseButtonPressed = false;
    m_translationOffsetX = 0;
    m_translationOffsetY = 0;
}

Map::~Map()
{

}

void Map::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    qDebug() << "OpenGL version:" << this->format().version();
}

void Map::resizeGL(int width, int height)
{

}

void Map::paintGL()
{
    drawStations();
    drawTrips();
}

void Map::wheelEvent(QWheelEvent* event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
//    m_zoom += numSteps * .01f;

    qDebug() <<  "Wheel event";
    update();
    event->accept();
}

void Map::mouseMoveEvent(QMouseEvent* event)
{
    if (m_leftMouseButtonPressed)
    {
        QPointF currentPos = event->pos();
        m_translationOffsetX += 1.f / m_zoom * (currentPos.x() - m_previousMousePos.x());
        m_translationOffsetY += 1.f / m_zoom * (currentPos.y() - m_previousMousePos.y());

        m_previousMousePos = event->pos();
        update();
        event->accept();
        qDebug() <<  "move";
    }
}

void Map::mousePressEvent(QMouseEvent* event)
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

void Map::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        QGuiApplication::restoreOverrideCursor();
        m_leftMouseButtonPressed = false;
        qDebug() <<  "release";
        event->accept();
    }
}

void Map::mouseDoubleClickEvent(QMouseEvent* event)
{
    qDebug() << "double click";
    event->accept();
}

void Map::drawStations()
{
    QPainter painter(this);

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < 100; ++i)
    {
        for (int j = 0; j < 20; ++j)
        {
            painter.setBrush(Qt::gray);
            painter.drawEllipse(QPoint(m_translationOffsetX + 30 + i * 20, m_translationOffsetY + 30 + j * 20), 10, 10);
        }
    }
    qDebug() << "drawStations: The slow operation took" << timer.elapsed() << "milliseconds";
}

void Map::drawTrips()
{
    QPainter painter(this);
    QLinearGradient linearGrad;
    painter.setRenderHint(QPainter::Antialiasing);
    linearGrad.setColorAt(0, QColor(255, 0, 0));
    linearGrad.setColorAt(1, QColor(255, 255, 0));

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < 250; ++i)
    {
        for (int j = 0; j < 20; ++j)
        {
            QPointF p1((m_translationOffsetX + 5 + i * 5),
                       (m_translationOffsetY + 5 + j * 50));
            QPointF p2((m_translationOffsetX + 5 + i * 5),
                       (m_translationOffsetY + 45 + j * 50));
            linearGrad.setStart(p1);
            linearGrad.setFinalStop(p2);
            QPen pen(QBrush(linearGrad), 1);
            pen.setCapStyle(Qt::RoundCap);
            painter.setPen(pen);
            painter.drawLine(p1, p2);
        }
    }
    qDebug() << "drawTrips: The slow operation took" << timer.elapsed() << "milliseconds";
}

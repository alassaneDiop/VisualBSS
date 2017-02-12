#include "mapglwidget.h"
#include <QGuiApplication>



MapGLWidget::MapGLWidget(QWidget* p) : QOpenGLWidget(p)
{
    m_zoom = 1.f;
    m_leftMouseButtonPressed = false;
    m_translationOffsetX = 0;
    m_translationOffsetY = 0;
}

MapGLWidget::~MapGLWidget()
{

}

void MapGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    qDebug() << "OpenGL version:" << this->format().version();
}

void MapGLWidget::resizeGL(int width, int height)
{

}

void MapGLWidget::paintGL()
{
    drawStations();
    drawTrips();
}

void MapGLWidget::drawStations()
{
    QPainter painter(this);

    QElapsedTimer timer;
    timer.start();

//    painter.setBrush(Qt::red);
//    painter.drawEllipse(
//                QPointF(m_translationOffsetX + 40, m_translationOffsetY + -73), 50, 50);

    foreach (QPointF p, m_stationsPos)
    {
//        qDebug() << "stations pos" << p << m_translationOffsetX << m_translationOffsetY;
        painter.setBrush(Qt::gray);
        painter.drawEllipse(
                    QPointF(m_translationOffsetX + p.x(), m_translationOffsetY + p.y()), 10, 10);

    }
    qDebug() << "drawStations: The slow operation took" << timer.elapsed() << "milliseconds";
}

void MapGLWidget::drawTrips()
{
    QPainter painter(this);
    QLinearGradient linearGrad;
    painter.setRenderHint(QPainter::Antialiasing);
    linearGrad.setColorAt(0, QColor(255, 0, 0));
    linearGrad.setColorAt(1, QColor(255, 255, 0));

    QElapsedTimer timer;
    timer.start();

    foreach (QRectF r, m_trips)
    {
        QPointF p1(m_translationOffsetX + r.topLeft().x(),
                   m_translationOffsetY + r.topLeft().y());
        QPointF p2(m_translationOffsetX + r.bottomRight().x(),
                   m_translationOffsetY + r.bottomRight().x());
        linearGrad.setStart(p1);
        linearGrad.setFinalStop(p2);
        QPen pen(QBrush(linearGrad), 1);
        pen.setCapStyle(Qt::RoundCap);
        painter.setPen(pen);
        painter.drawLine(p1, p2);
    }

/// trajets hard codées, juste pour le teste ed'affichage
//        QPainter painter(this);
//        QLinearGradient linearGrad;
//        painter.setRenderHint(QPainter::Antialiasing);
//        linearGrad.setColorAt(0, QColor(255, 0, 0));
//        linearGrad.setColorAt(1, QColor(255, 255, 0));

//        QElapsedTimer timer;
//        timer.start();

//        for (int i = 0; i < 250; ++i)
//        {
//            for (int j = 0; j < 20; ++j)
//            {
//                QPointF p1((m_translationOffsetX + 5 + i * 5),
//                           (m_translationOffsetY + 5 + j * 50));
//                QPointF p2((m_translationOffsetX + 5 + i * 5),
//                           (m_translationOffsetY + 45 + j * 50));
//                linearGrad.setStart(p1);
//                linearGrad.setFinalStop(p2);
//                QPen pen(QBrush(linearGrad), 1);
//                pen.setCapStyle(Qt::RoundCap);
//                painter.setPen(pen);
//                painter.drawLine(p1, p2);
//            }
//        }

    qDebug() << "drawTrips: The slow operation took" << timer.elapsed() << "milliseconds";
}

void MapGLWidget::onDataLoaded(const QList<Station>& stations, const QList<Trip>& trips)
{
    foreach (Station s, stations)
    {
        m_stationsPos.append(QPointF(s.getLatitude(), s.getLongitude()));
    }
    qDebug() << "m_stations size " << m_stationsPos.size();

    foreach (Trip t, trips)
    {
        m_trips.append(QRect(t.getStartStation().getLatitude(), t.getStartStation().getLongitude(),
                        t.getEndStation().getLatitude(), t.getEndStation().getLatitude()));
    }

    qDebug() << "m_trip size" << m_trips.size();
    update();
}

void MapGLWidget::wheelEvent(QWheelEvent* event)
{
//    int numDegrees = event->delta() / 8;
//    int numSteps = numDegrees / 15;
//        m_zoom += numSteps * .01f;

    qDebug() <<  "Wheel event";
    update();
    event->accept();
}

void MapGLWidget::mouseMoveEvent(QMouseEvent* event)
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

void MapGLWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    qDebug() << "double click";
    event->accept();
}



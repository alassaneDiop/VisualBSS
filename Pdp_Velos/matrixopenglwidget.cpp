#include "matrixopenglwidget.h"

#include <QGuiApplication>
#include <qstring.h>

static void DEBUG(QString m)
{
#ifdef QT_DEBUG
    qDebug() << m.toStdString().c_str();
#endif
}

MatrixOpenGLWidget::MatrixOpenGLWidget(QWidget* p) : QOpenGLWidget(p)
{
    m_matrixViewWidth = this->width() - 2 * m_matrixOffsetX;

    m_drawRectangle = false;
    m_leftMouseButtonPressed = false;
    m_translationValue = 0;
    m_dragSelectionBorderWidth = 1;
}

MatrixOpenGLWidget::~MatrixOpenGLWidget()
{

}

void MatrixOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1.f, 1.f, 1.f, 1.f);
}

void MatrixOpenGLWidget::resizeGL(int width, int height)
{
    m_matrixViewWidth = this->width() - 2 * m_matrixOffsetX;
}

void MatrixOpenGLWidget::paintGL()
{
    drawDirections();
    drawDragSelection();
    DEBUG("HELLO WORLD");
}

void MatrixOpenGLWidget::drawDragSelection()
{
    QPainter painter;
    painter.begin(this);

    QRect rectangle(m_topLeftSelectionRectangle.x(),
                    m_topLeftSelectionRectangle.y() + m_translationValue,
                    m_bottomRightSelectionRectanle.x() - m_topLeftSelectionRectangle.x(),
                    m_bottomRightSelectionRectanle.y() - m_topLeftSelectionRectangle.y());

    if (m_drawRectangle)
    {
        painter.fillRect(rectangle, QBrush(QColor(10, 10, 210, 128)));

        QPen pen(Qt::black, m_dragSelectionBorderWidth);
        painter.setPen(pen);

        painter.drawRect(rectangle);
    }
    painter.end();
}

void MatrixOpenGLWidget::drawDirections()
{
    QPainter painter(this);
    int intervalLength = m_matrixViewWidth / m_numberOfInterval;

    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10; ++i)
    {
        for (int j = 0; j < 24; ++j)
        {
            painter.setBrush(QColor(100, 100, 100, 128));
            painter.drawEllipse(QPoint(
                                    m_matrixOffsetX + j * intervalLength + (m_stationCircleSize / 2),
                                    m_translationValue + i * 30 + m_stationCircleSize),
                                m_stationCircleSize, m_stationCircleSize);
        }
    }
    qDebug() << "drawDirections: The slow operation took" << timer.elapsed() << "milliseconds";
}

void MatrixOpenGLWidget::wheelEvent(QWheelEvent* event)
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

void MatrixOpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_leftMouseButtonPressed)
    {
        m_previousMousePos = event->pos();
        m_previousMousePos.setY(m_previousMousePos.y() - m_translationValue);
        m_bottomRightSelectionRectanle = event->pos();
        m_bottomRightSelectionRectanle.setY(
                    m_bottomRightSelectionRectanle.y() - m_translationValue);
        update();
        event->accept();
    }
    qDebug() <<  "move";
}

void MatrixOpenGLWidget::mousePressEvent(QMouseEvent* event)
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

void MatrixOpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        QGuiApplication::restoreOverrideCursor();
        m_leftMouseButtonPressed = false;
        event->accept();
        qDebug() <<  "left mouse button released";
    }
}

void MatrixOpenGLWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    qDebug() << "double click";
}


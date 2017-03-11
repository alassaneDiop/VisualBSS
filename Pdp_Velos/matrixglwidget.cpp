#include "matrixglwidget.h"

#include <QGuiApplication>
#include <QtMath>
#include <QDebug>
#include <QWheelEvent>
#include <QPainter>
#include <QElapsedTimer>
#include <QOpenGLShaderProgram>

#include "trip.h"
#include "station.h"


MatrixGLWidget::MatrixGLWidget(QWidget* p) : QOpenGLWidget(p)
{
    // Multiply by 2 to get offset on left and right
    m_matrixViewWidth = this->width() - 2 * m_matrixOffsetX;

    m_drawRectangle = false;
    m_leftMouseButtonPressed = false;
    m_translationOffsetY = 0;
    m_dragSelectionBorderWidth = 1;

    m_selectorRenderer = new SelectorRenderer();
}

MatrixGLWidget::~MatrixGLWidget()
{
    if (m_shaderProgramSelector)
        delete m_shaderProgramSelector;
    if (m_shaderProgramGlyph)
        delete m_shaderProgramGlyph;

    if (m_selectorRenderer)
        delete m_selectorRenderer;
}

void MatrixGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    this->makeCurrent();

    m_selectorRenderer->prepareData(6, 6 * 2);

    glClearColor(m_backgroundColor.red(),
                 m_backgroundColor.green(),
                 m_backgroundColor.blue(),
                 1.f);

    // TODO: faire des tests sur les shaders, chargement, linkage
    m_shaderProgramSelector = new QOpenGLShaderProgram();
    m_shaderProgramSelector->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/shaders/selector.vert");
    m_shaderProgramSelector->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/shaders/selector.frag");
    m_shaderProgramSelector->link();
    m_shaderProgramSelector->bind();
    m_shaderProgramSelector->release();

    m_shaderProgramGlyph = new QOpenGLShaderProgram();
    //    m_shaderProgramGlyph->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/shaders/glyph.vert");
    //    m_shaderProgramGlyph->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/shaders/glyph.frag");
    //    m_shaderProgramGlyph->link();
    //    m_shaderProgramGlyph->bind();
    //    m_shaderProgramGlyph->release();

    qDebug() << "MatrixGLWidget::initializeGL() OpenGL version:" << this->format().version();
}

void MatrixGLWidget::resizeGL(int width, int height)
{
    m_matrixViewWidth = this->width() - 2 * m_matrixOffsetX;
}

void MatrixGLWidget::paintGL()
{
    initializeOpenGLFunctions();
    //    drawGlyphs();
    drawSelector();
}

void MatrixGLWidget::drawSelector()
{
    initializeOpenGLFunctions();
    if (m_drawRectangle)
    {
        m_shaderProgramSelector->bind();

        int translationLoc = m_shaderProgramSelector->uniformLocation("translation");
        glUniform1f(translationLoc, m_translationOffsetY);
        m_selectorRenderer->draw();

        m_shaderProgramSelector->release();
    }
}

void MatrixGLWidget::drawGlyphs()
{
    QPainter painter(this);
    int intervalLength = m_matrixViewWidth / m_numberOfInterval;

    QElapsedTimer timer;
    timer.start();

    for (QPoint i : m_ellipses)
    {
        painter.setBrush(QColor(100, 100, 100, 128));
        painter.drawEllipse(QPoint(m_matrixOffsetX + i.x() * intervalLength + (m_stationCircleSize / 2),
                                   m_translationOffsetY + i.y() + m_stationCircleSize),
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
    m_translationOffsetY += event->delta();

    if (m_translationOffsetY > 0)
        m_translationOffsetY = 0;

    event->accept();
    update();
    qDebug() <<  "Wheel event";
}

void MatrixGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_leftMouseButtonPressed)
    {
        m_previousMousePos = event->pos();
        m_previousMousePos.setY(m_previousMousePos.y() - m_translationOffsetY);
        m_bottomRightSelectionRectangle = event->pos();
        m_bottomRightSelectionRectangle.setY(
                    m_bottomRightSelectionRectangle.y() - m_translationOffsetY);

        float topLeftX = m_topLeftSelectionRectangle.x() / this->width() * 2 - 1;
        float topLeftY = m_topLeftSelectionRectangle.y() / this->height() * 2 - 1;

        float bottomRightX = m_bottomRightSelectionRectangle.x() / this->width() * 2 - 1;
        float bottomRightY = m_bottomRightSelectionRectangle.y() / this->height() * 2 - 1;

        QVector<float> data;
        data.append(topLeftX);
        data.append(-topLeftY);

        data.append(bottomRightX);
        data.append(-topLeftY);

        data.append(topLeftX);
        data.append(-bottomRightY);

        data.append(topLeftX);
        data.append(-bottomRightY);

        data.append(bottomRightX);
        data.append(-bottomRightY);

        data.append(bottomRightX);
        data.append(-topLeftY);

        m_selectorRenderer->updateData(data);

        QVector<QPoint> out = hit();
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
        m_previousMousePos.setY(m_previousMousePos.y() - m_translationOffsetY);
        m_topLeftSelectionRectangle = event->pos();
        m_topLeftSelectionRectangle.setY(
                    m_topLeftSelectionRectangle.y() - m_translationOffsetY);
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

QVector<QPoint> MatrixGLWidget::hit()
{
    int intervalLength = m_matrixViewWidth / m_numberOfInterval;
    QVector<QPoint> outEllipses;

    QRect rectangle(m_topLeftSelectionRectangle.x(),
                    m_topLeftSelectionRectangle.y() + m_translationOffsetY,
                    m_bottomRightSelectionRectangle.x() - m_topLeftSelectionRectangle.x(),
                    m_bottomRightSelectionRectangle.y() - m_topLeftSelectionRectangle.y());

    for (QPoint i : m_ellipses)
    {
        int x = m_matrixOffsetX + i.x() * intervalLength + (m_stationCircleSize / 2);
        int y = m_translationOffsetY + i.y() + m_stationCircleSize;

        if(rectangle.contains(x,y)) outEllipses.push_back(i);
    }
    return outEllipses;
}

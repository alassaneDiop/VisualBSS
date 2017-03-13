#include "matrixglwidget.h"

#include <QGuiApplication>
#include <QDebug>
#include <QWheelEvent>
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
    m_translationOffsetY = 0.f;

    m_isGlyphsVAOCreated = false;
    m_glyphsLoaded = false;

    m_selectorRenderer = Q_NULLPTR;
    m_glyphRenderer = Q_NULLPTR;

    m_selectorRenderer = new SelectorRenderer();
    m_glyphRenderer = new GlyphRenderer();
}

MatrixGLWidget::~MatrixGLWidget()
{
    if (m_shaderProgramSelector)
        delete m_shaderProgramSelector;
    if (m_shaderProgramGlyph)
        delete m_shaderProgramGlyph;

    if (m_selectorRenderer)
        delete m_selectorRenderer;

    if (m_glyphRenderer)
        delete m_glyphRenderer;
}

void MatrixGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    this->makeCurrent();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_selectorRenderer->createVAO();
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
    m_shaderProgramGlyph->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/shaders/glyph.vert");
    m_shaderProgramGlyph->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/shaders/glyph.frag");
    m_shaderProgramGlyph->link();
    m_shaderProgramGlyph->bind();
    m_shaderProgramGlyph->release();

    qDebug() << "MatrixGLWidget::initializeGL() OpenGL version:" << this->format().version();
}

void MatrixGLWidget::resizeGL(int width, int height)
{
    Q_UNUSED(width);
    Q_UNUSED(height);

    m_matrixViewWidth = this->width() - 2 * m_matrixOffsetX;
}

void MatrixGLWidget::paintGL()
{
    if (m_glyphsLoaded)
        drawGlyphs();

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
    m_shaderProgramGlyph->bind();

    int translationLoc = m_shaderProgramGlyph->uniformLocation("translation");
    glUniform1f(translationLoc, m_translationOffsetY);

    m_glyphRenderer->draw();

    m_shaderProgramGlyph->release();
}

void MatrixGLWidget::loadGlyphsData(const QVector<float> &data, unsigned int verticesCount)
{
    if (!m_isGlyphsVAOCreated)
    {
        m_isGlyphsVAOCreated = true;
        m_glyphRenderer->initGLFunc();
        m_glyphRenderer->createVAO();
    }
    m_glyphsLoaded = true;
    m_glyphRenderer->sendData(data, verticesCount);
}

void MatrixGLWidget::wheelEvent(QWheelEvent* event)
{
    int i = 1;
    if (event->delta() < 0)
        i *= -1;

    const float scrollValue = 0.05f;
    m_translationOffsetY += i * scrollValue;

    if (m_translationOffsetY > 0)
        m_translationOffsetY = 0;

    //qDebug() << m_translationOffsetY;

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

#include "matrixglwidget.h"

#include <QGuiApplication>
#include <QDebug>
#include <QWheelEvent>
#include <QElapsedTimer>
#include <QOpenGLShaderProgram>

#include "config.h"


MatrixGLWidget::MatrixGLWidget(QWidget* p) : QOpenGLWidget(p)
{
    // Multiply by 2 to get offset on left and right
    m_matrixViewWidth = this->width() - 2 * bss::TIMELINE_OFFSET_X;

    m_drawSelector = false;
    m_leftMouseButtonPressed = false;
    m_translationY = 0.f;

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

    if (!initializeShaderGlyphs())
        emit onShaderError(m_shaderProgramGlyph->log());

    if (!initializeShaderSelector())
        emit onShaderError(m_shaderProgramSelector->log());

    qDebug() << "MatrixGLWidget::initializeGL() OpenGL version:" << this->format().version();
}

void MatrixGLWidget::resizeGL(int width, int height)
{
    Q_UNUSED(width);
    Q_UNUSED(height);

    m_matrixViewWidth = this->width() - 2 * bss::TIMELINE_OFFSET_X;
}

void MatrixGLWidget::paintGL()
{
    drawGlyphs();
    drawSelector();
}


void MatrixGLWidget::drawSelector()
{
    if (m_drawSelector)
    {
        m_shaderProgramSelector->bind();

        const int translationLoc = m_shaderProgramSelector->uniformLocation("translation");
        glUniform1f(translationLoc, m_translationY);
        m_selectorRenderer->draw();

        m_shaderProgramSelector->release();
    }
}

void MatrixGLWidget::drawGlyphs()
{
    if (m_glyphsLoaded)
    {
        m_shaderProgramGlyph->bind();

        const int translationLoc = m_shaderProgramGlyph->uniformLocation("translation");
        glUniform1f(translationLoc, m_translationY);

        m_glyphRenderer->draw();

        m_shaderProgramGlyph->release();
    }
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

    update();
}

void MatrixGLWidget::wheelEvent(QWheelEvent* event)
{
    int i = 1;
    if (event->delta() < 0)
        i *= -1;

    const float scrollValue = ((float)bss::GLYPH_HEIGHT) / height();

    m_translationY += i * scrollValue;

    if (m_translationY > 0)
        m_translationY = 0;

    //qDebug() << m_translationOffsetY;

    event->accept();
    update();
    //qDebug() <<  "Wheel event";
}

void MatrixGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_leftMouseButtonPressed)
    {
        m_previousMousePos = event->pos();
        m_previousMousePos.setY(m_previousMousePos.y() - m_translationY);
        m_bottomRightSelectionRectangle = event->pos();
        m_bottomRightSelectionRectangle.setY(
                    m_bottomRightSelectionRectangle.y() - m_translationY);

        updateSelector();

        tripsInSelector();

        update();
        event->accept();
    }
    //qDebug() <<  "move";
}

void MatrixGLWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        QGuiApplication::setOverrideCursor(Qt::CrossCursor);
        m_drawSelector = true;
        m_leftMouseButtonPressed = true;

        m_previousMousePos = event->pos();
        m_previousMousePos.setY(m_previousMousePos.y() - m_translationY);

        m_topLeftSelectionRectangle = event->pos();
        m_topLeftSelectionRectangle.setY(m_topLeftSelectionRectangle.y()
                                         - m_translationY);
        event->accept();
        //qDebug() <<  "left mouse button pressed";
    }
    else if (event->button() == Qt::RightButton)
    {
        m_drawSelector = false;
        event->accept();
        update();
        //qDebug() <<  "right mouse button pressed";
    }
}

void MatrixGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        QGuiApplication::restoreOverrideCursor();
        m_leftMouseButtonPressed = false;
        event->accept();
        //qDebug() <<  "left mouse button released";
    }
}

// FIXME: DAMIEN: refactoriser et changer le type
QPair<QPair<char, char>, QPair<int, int>> MatrixGLWidget::tripsInSelector() const
{

    const int width = this->width();
    const float oneHour = width / bss::NB_OF_HOURS;

    QPair<char, char> timeInterval;
    timeInterval.first = (char)(m_topLeftSelectionRectangle.x() / oneHour);
    timeInterval.second = (char)(m_bottomRightSelectionRectangle.x() / oneHour);

    char tmp1 = timeInterval.first;
    char tmp2 = timeInterval.second;

    timeInterval.first = qMin(tmp1, tmp2);
    timeInterval.second = qMax(tmp1, tmp2);

    timeInterval.first = qMax((char)0, timeInterval.first);
    timeInterval.second = qMin((int)timeInterval.second, bss::NB_OF_HOURS);

    qDebug() << "Time interval"<< (int)timeInterval.first << (int)timeInterval.second;


    // FIXME: DAMIEN : bug
    // FIND STATIONS
    const int glyphIntervalY = bss::GLYPH_HEIGHT + bss::SPACE_BETWEEN_GLYPHS;

    const float sizeInterval = this->height() / ((glyphIntervalY + bss::GLYPH_HEIGHT) * 2 - 1);

    const float normalizedTranslationY = m_translationY * height() / 2.f;

    QPair<int, int> stationsInterval;
    stationsInterval.first = (m_topLeftSelectionRectangle.y() - normalizedTranslationY)
            / sizeInterval;

    stationsInterval.second = (m_bottomRightSelectionRectangle.y() - normalizedTranslationY)
            / sizeInterval;

    int tmp3 = stationsInterval.first;
    int tmp4 = stationsInterval.second;

    stationsInterval.first = qMin(tmp3, tmp4);
    stationsInterval.second = qMax(tmp3, tmp4);
    stationsInterval.first = qMax(0, stationsInterval.first);

    qDebug() << "Stations interval"<< stationsInterval.first << stationsInterval.second;

    QPair<QPair<char, char>, QPair<int, int>> trips;
    trips.first.first = timeInterval.first;
    trips.first.second = timeInterval.second;

    trips.second.first = stationsInterval.first;
    trips.second.second = stationsInterval.second;

    return trips;
}

void MatrixGLWidget::updateSelector() const
{
    QPointF topLeft;
    topLeft.setX(m_topLeftSelectionRectangle.x() / this->width() * 2 - 1);

    topLeft.setY((m_topLeftSelectionRectangle.y() / this->height() * 2 - 1)
                 - m_translationY);

    QPointF bottomRight;
    bottomRight.setX(m_bottomRightSelectionRectangle.x() / this->width() * 2 - 1);

    bottomRight.setY((m_bottomRightSelectionRectangle.y() / this->height() * 2 - 1)
                     - m_translationY);

    QVector<float> data;
    data += QVector<float>({ (float)topLeft.x(), (float)-topLeft.y(), (float)bottomRight.x() });
    data += QVector<float>({ (float)-topLeft.y(), (float)topLeft.x(), (float)-bottomRight.y() });
    data += QVector<float>({ (float)topLeft.x(), (float)-bottomRight.y(), (float)bottomRight.x() });
    data += QVector<float>({ (float)-bottomRight.y(), (float)bottomRight.x(), (float)-topLeft.y() });

    m_selectorRenderer->updateData(data);
}

bool MatrixGLWidget::initializeShaderSelector()
{
    bool initializationIsOk = true;

    m_shaderProgramSelector = new QOpenGLShaderProgram();
    initializationIsOk &= m_shaderProgramSelector->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/shaders/selector.vert");
    initializationIsOk &= m_shaderProgramSelector->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/shaders/selector.frag");
    initializationIsOk &= m_shaderProgramSelector->link();
    initializationIsOk &= m_shaderProgramSelector->bind();
    m_shaderProgramSelector->release();

    return initializationIsOk;
}

bool MatrixGLWidget::initializeShaderGlyphs()
{
    bool initializationIsOk = true;

    m_shaderProgramGlyph = new QOpenGLShaderProgram();
    initializationIsOk &= m_shaderProgramGlyph->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/shaders/glyph.vert");
    initializationIsOk &= m_shaderProgramGlyph->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/shaders/glyph.frag");
    initializationIsOk &= m_shaderProgramGlyph->link();
    initializationIsOk &= m_shaderProgramGlyph->bind();
    m_shaderProgramGlyph->release();

    return initializationIsOk;
}

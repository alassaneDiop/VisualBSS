/* Copyright 2017
 * All rights reserved to "Université de Bordeaux"
 *
 * This file is part of VisualBSS.
 * VisualBSS is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VisualBSS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with VisualBSS.  If not, see <http://www.gnu.org/licenses/>.
 * */

#include "matrixglwidget.h"

#include <QGuiApplication>
#include <QDebug>
#include <QWheelEvent>
#include <QElapsedTimer>
#include <QOpenGLShaderProgram>
#include <QPair>

#include "config.h"
#include "selectorrenderer.h"
#include "glyphrenderer.h"


MatrixGLWidget::MatrixGLWidget(QWidget* p) : QOpenGLWidget(p)
{
    // Multiply by 2 to get offset on left and right
    m_matrixViewWidth = this->width() - 2 * bss::TIMELINE_OFFSET_X;

    m_drawSelector = false;
    m_leftMouseButtonPressed = false;
    m_translationY = 0.f;

    m_isGlyphsVAOCreated = false;
    m_drawGlyphs = false;

    m_selectorRenderer = new SelectorRenderer();
    m_glyphRenderer = new GlyphRenderer();
}

MatrixGLWidget::~MatrixGLWidget()
{
    delete m_shaderProgramSelector;
    delete m_shaderProgramGlyph;
    delete m_selectorRenderer;
    delete m_glyphRenderer;
}


void MatrixGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    this->makeCurrent();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_selectorRenderer->initGLFunc();
    m_selectorRenderer->createVAO();

    glClearColor(m_backgroundColor.red(),
                 m_backgroundColor.green(),
                 m_backgroundColor.blue(),
                 1.f);

    if (!initializeShaderGlyphs())
        emit onShaderError(m_shaderProgramGlyph->log());

    if (!initializeShaderSelector())
        emit onShaderError(m_shaderProgramSelector->log());

    m_frameCount = 0;
    m_lastTime = 0;

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
    glClear(GL_COLOR_BUFFER_BIT);

    /// BEGIN Used for performance tests
    //    if (m_frameCount == 0)
    //        m_time.start();
    /// END Used for performance tests

    drawGlyphs();
    drawSelector();

    /// BEGIN Used for performance tests
    //    m_frameCount++;

    //    if (m_time.elapsed() - m_lastTime >= 1000)
    //    {
    //        qDebug() << "MatrixGLWidget drawn in:" << double(1000) / m_frameCount << "ms";
    //        m_frameCount = 0;
    //        m_lastTime++;
    //    }

    //    if (m_drawGlyphs)
    //        update();
    /// END Used for performance tests
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
    if (m_drawGlyphs)
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

    m_drawGlyphs = true;
    m_glyphRenderer->sendData(data, verticesCount);

    update();
}

void MatrixGLWidget::clear()
{
    m_drawGlyphs = false;
    m_drawSelector = false;
    update();
}

void MatrixGLWidget::wheelEvent(QWheelEvent* event)
{
    int i = 1;
    if (event->delta() < 0)
        i *= -1;

    const float scrollValue = ((float)bss::GLYPH_HEIGHT) / height();

    m_translationY += i * scrollValue * bss::MATRIX_SCROLL_COEFF;

    if (m_translationY > 0)
        m_translationY = 0;

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

        SelectionTimeStations s = tripsInSelector();
        if (!(s == m_selectionnedTrips))
        {
            m_selectionnedTrips = s;
            emit selectionChanged(s.fromHour, s.toHour, s.fromStationIndex, s.toStationIndex);
        }

        event->accept();
        //qDebug() <<  "left mouse button released";
    }
}

QPair<int, int> MatrixGLWidget::timeIntervalSelected() const
{
    const int width = this->width();
    const float oneHour = width / bss::NB_OF_HOURS;

    QPair<int, int> timeInterval;
    timeInterval.first = m_topLeftSelectionRectangle.x() / oneHour;
    timeInterval.second = m_bottomRightSelectionRectangle.x() / oneHour;

    char tmp1 = timeInterval.first;
    char tmp2 = timeInterval.second;

    timeInterval.first = qMin(tmp1, tmp2);
    timeInterval.second = qMax(tmp1, tmp2);

    timeInterval.first = qMax(0, timeInterval.first);
    timeInterval.second = qMin(timeInterval.second, bss::NB_OF_HOURS);
    //qDebug() << "Time interval"<< (int)timeInterval.first << (int)timeInterval.second;

    return timeInterval;
}


// FIXME : Doesn't return corrects values
QPair<int, int> MatrixGLWidget::stationIntervalSelected() const
{
    const int glyphIntervalY = bss::GLYPH_HEIGHT + bss::SPACE_BETWEEN_GLYPHS;

    // m_translationY from OpenGL coordinates to raster coordinates
    const float normalizedTranslationY = m_translationY * height() / 2.f;

    QPair<int, int> stationsInterval;
    stationsInterval.first = (m_topLeftSelectionRectangle.y() - normalizedTranslationY)
            / glyphIntervalY;

    stationsInterval.second = (m_bottomRightSelectionRectangle.y() - normalizedTranslationY)
            / glyphIntervalY;

    int tmp1 = stationsInterval.first;
    int tmp2 = stationsInterval.second;

    stationsInterval.first = qMin(tmp1, tmp2);
    stationsInterval.second = qMax(tmp1, tmp2);
    stationsInterval.first = qMax(0, stationsInterval.first);

    //qDebug() << "Stations interval"<< stationsInterval.first << stationsInterval.second;

    return stationsInterval;
}

SelectionTimeStations MatrixGLWidget::tripsInSelector() const
{
    QPair<int, int> timeInterval = timeIntervalSelected();
    QPair<int, int> stationsInterval = stationIntervalSelected();

    SelectionTimeStations s;
    s.fromHour = timeInterval.first;
    s.toHour = timeInterval.second;
    s.fromStationIndex = stationsInterval.first;
    s.toStationIndex = stationsInterval.second;

    return s;
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

    QRectF data = QRectF(topLeft, bottomRight);
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

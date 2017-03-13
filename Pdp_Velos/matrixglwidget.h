#ifndef MATRIXOPENGLWIDGET_H
#define MATRIXOPENGLWIDGET_H

#include <QWheelEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QColor>

#include "selectorrenderer.h"
#include "glyphrenderer.h"

namespace bss {
class MatrixGLWidget;
}

class QMouseEvent;
class Station;
class Trip;
class QPoint;
class QOpenGLShaderProgram;

class MatrixGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    MatrixGLWidget(QWidget* p = 0);
    ~MatrixGLWidget();

    void loadGlyphsData(const QVector<float>& data, unsigned int verticesCount);


protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int width, int height) override;
    virtual void paintGL() override;
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void mouseMoveEvent (QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    void initPoint();
    QVector<QPoint> hit();

private:
    void drawSelector();
    void drawGlyphs();

private:
    const int   m_numberOfInterval = 24;
    const int   m_matrixOffsetX = 10;
    int         m_matrixViewWidth;
    int         m_stationCircleSize = 10;

    QVector<QPoint> m_ellipses;

    bool    m_leftMouseButtonPressed;
    QPointF m_previousMousePos;
    float   m_translationOffsetY;

    QPointF     m_topLeftSelectionRectangle;
    QPointF     m_bottomRightSelectionRectangle;

    bool    m_drawRectangle;

    QList<const Station*>   m_stations;
    QList<const Trip*>      m_trips;

    const QColor            m_backgroundColor = QColor(Qt::white);

    bool                    m_glyphsLoaded;

    SelectorRenderer*       m_selectorRenderer;
    GlyphRenderer*          m_glyphRenderer;
    QOpenGLShaderProgram*   m_shaderProgramSelector;
    QOpenGLShaderProgram*   m_shaderProgramGlyph;

    bool m_isGlyphsVAOCreated;
};
#endif // MATRIXOPENGLWIDGET_H

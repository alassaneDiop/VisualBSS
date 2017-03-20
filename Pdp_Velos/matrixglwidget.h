#ifndef MATRIXOPENGLWIDGET_H
#define MATRIXOPENGLWIDGET_H

#include "selectorrenderer.h"
#include "glyphrenderer.h"

#include <QWheelEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QColor>
#include <QVector>
#include <QPair>


namespace bss {
class MatrixGLWidget;
}

class QMouseEvent;
class QPointF;
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

private:
    void drawSelector();
    void drawGlyphs();
    void updateSelector() const;

    bool initializeShaderSelector();
    bool initializeShaderGlyphs();


    QPair<QPair<char, char>, QPair<int, int>> tripsInSelector() const;

    int m_matrixViewWidth;

    bool m_leftMouseButtonPressed;
    QPointF m_previousMousePos;
    float m_translationY;

    QPointF m_topLeftSelectionRectangle;
    QPointF m_bottomRightSelectionRectangle;

    bool m_drawSelector;

    const QColor m_backgroundColor = QColor(Qt::white);

    bool m_glyphsLoaded;

    SelectorRenderer* m_selectorRenderer;
    GlyphRenderer* m_glyphRenderer;
    QOpenGLShaderProgram* m_shaderProgramSelector;
    QOpenGLShaderProgram* m_shaderProgramGlyph;

    bool m_isGlyphsVAOCreated;


signals:
    void onSelectionChanged(short minTime, short maxTime, int fromStationIndex, int toStationIndex);

    void onShaderError(const QString& message);
};
#endif // MATRIXOPENGLWIDGET_H

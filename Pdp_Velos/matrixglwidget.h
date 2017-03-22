#ifndef MATRIXOPENGLWIDGET_H
#define MATRIXOPENGLWIDGET_H


#include <QWheelEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include <QColor>
#include <QPair>



namespace bss {
class MatrixGLWidget;
}

class GlyphRenderer;
class SelectorRenderer;
class QMouseEvent;
class QPointF;
class QOpenGLShaderProgram;

struct SelectionTimeSatations {
    char minTime;
    char maxTime;
    int fromStationIndex;
    int toStationIndex;

    inline bool operator ==(const SelectionTimeSatations& other)
    {
        return (minTime == other.minTime)
                && (maxTime == other.maxTime)
                && (fromStationIndex == other.fromStationIndex)
                && (toStationIndex == other.toStationIndex);
    }
};

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

    SelectionTimeSatations tripsInSelector() const;

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

    SelectionTimeSatations m_selectionnedTrips;

signals:
    void onSelectionChanged(char minTime, char maxTime, int fromStationIndex, int toStationIndex);

    void onShaderError(const QString& message);
};
#endif // MATRIXOPENGLWIDGET_H

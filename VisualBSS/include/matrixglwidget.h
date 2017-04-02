 #ifndef MATRIXOPENGLWIDGET_H
#define MATRIXOPENGLWIDGET_H


#include <QWheelEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include <QColor>
#include <QPair>
#include <QTime>


namespace bss {
class MatrixGLWidget;
}

class GlyphRenderer;
class SelectorRenderer;
class QMouseEvent;
class QPointF;
class QOpenGLShaderProgram;


/**
 * @brief The SelectionTimeStations struct Contains all informations to get wich
 * stations and trips are selected by the user with the selector.
 */
struct SelectionTimeStations
{
    /**
     * @brief fromHour The beginning hour.
     */
    int fromHour = 0;

    /**
     * @brief toHour The end hour.
     */
    int toHour = 0;

    /**
     * @brief fromStationIndex The first index station where
     * fromStationIndex <= toStationIndex.
     */
    int fromStationIndex = 0;

    /**
     * @brief toStationIndex The second index station where
     * toStationIndex >= fromStationIndex.
     */
    int toStationIndex = 0;


    inline bool operator ==(const SelectionTimeStations& other)
    {
        return (fromHour == other.fromHour)
                && (toHour == other.toHour)
                && (fromStationIndex == other.fromStationIndex)
                && (toStationIndex == other.toStationIndex);
    }
};


/**
 * @brief The MatrixGLWidget class
 */
class MatrixGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    MatrixGLWidget(QWidget* p = 0);
    ~MatrixGLWidget();

    /**
     * @brief loadGlyphsData Check if the VAO is created, if not it creates and
     * send data to the graphics card.
     * @param data Represents positions and colors of glyphs.
     * @param verticesCount Number of vertices to draw.
     */
    void loadGlyphsData(const QVector<float>& data, unsigned int verticesCount);

    /**
     * @brief clear Clear all glyphs in the view.
     */
    void clear();

protected:
    /**
     * @brief initializeGL Initialize OpenGL functions and check shaders errors.
     */
    virtual void initializeGL() override;

    /**
     * @brief resizeGL If the widget is resized, it updates the new width and
     * height
     * @param width New width.
     * @param height New height;
     */
    virtual void resizeGL(int width, int height) override;

    /**
     * @brief paintGL Main loop, draws all glyphs and selector.
     */
    virtual void paintGL() override;

    /**
     * @brief wheelEvent Called every time the user moves the mouse wheel.
     * @param event Gives informations about the action.
     */
    virtual void wheelEvent(QWheelEvent* event) override;

    /**
     * @brief mouseMoveEvent Called every time the user moves the mouse.
     * @param event Gives informations about the action.
     */
    virtual void mouseMoveEvent (QMouseEvent* event) override;

    /**
     * @brief mousePressEvent Called every time the user presses mouse buttons.
     * @param event Gives informations about the action.
     */
    virtual void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief mouseReleaseEvent Called every time the user releases mouse
     * buttons.
     * @param event Gives informations about the action.
     */
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

private:
    /**
     * @brief drawSelector Draws rectangle to select wich trips and stations
     * the user wants to display on the map.
     */
    void drawSelector();

    /**
     * @brief drawGlyphs Draws glyphs in the widget.
     */
    void drawGlyphs();

    /**
     * @brief updateSelector This function is called when the user change the
     * position or the size of the selector.
     */
    void updateSelector() const;

    /**
     * @brief initializeShaderSelector Loads, compiles and links stations
     * shaders, return <i>falses</i> if something went wrong, <i>true</i>
     * otherwise.
     * @return <i>false</i> if loading, compilation or linkage went wrong,
     * <i>true</i> if success.
     */
    bool initializeShaderSelector();

    /**
     * @brief initializeShaderGlyphsLoads, compiles and links stations
     * shaders, return <i>falses</i> if something went wrong, <i>true</i>
     * otherwise.
     * @return <i>false</i> if loading, compilation or linkage went wrong,
     * <i>true</i> if success.
     */
    bool initializeShaderGlyphs();

    /**
     * @brief timeIntervalSelected Calculate the time inerval when the user
     * selects glyphs in the widget. The function returns a pair of hours, the
     * first is beginning hour and the second is the end.
     * @return A pair of hour, first hour is the begnning and the second the
     * end.
     */
    QPair<int, int> timeIntervalSelected() const;

    /**
     * @brief stationIntervalSelected Calculate the index of stations that are
     * selected by the user.
     * @return A pair of stations index , the first is the lower index, the
     * second is the greater index.
     */
    QPair<int, int> stationIntervalSelected() const;

    /**
     * @brief tripsInSelector Use <i>timeIntervalSelected</i> and
     * <i>stationIntervalSelected</i> to return a structure that contains
     * hours interval and stations interval.
     * @return A struct that contains hours interval and stations interval.
     */
    SelectionTimeStations tripsInSelector() const;


    /**
     * @brief m_matrixViewWidth Width of the drawable space, its the width of
     * the widget less an offset (on left and right).
     */
    int m_matrixViewWidth;
    /**
     * @brief m_leftMouseButtonPressed If the left mouse button is pressed
     * <i>true</i>, <i>false</i> if its released.
     */
    bool m_leftMouseButtonPressed;

    /**
     * @brief m_previousMousePos Used to calculate the translation. Its the
     * previous position of the cursor of the mouse.
     */
    QPointF m_previousMousePos;

    /**
     * @brief m_translationY Its the translation value on Y axis.
     */
    float m_translationY;

    /**
     * @brief m_topLeftSelectionRectangle Top left position of the selector.
     */
    QPointF m_topLeftSelectionRectangle;

    /**
     * @brief m_bottomRightSelectionRectangle Bottom right position of the
     * selector.
     */
    QPointF m_bottomRightSelectionRectangle;

    /**
     * @brief m_drawSelector If sets to <i>true</i>, selector will be drawn, if
     * <i>false</i> will not.
     */
    bool m_drawSelector;

    /**
     * @brief m_backgroundColor Background color of the widget.
     */
    const QColor m_backgroundColor = QColor(Qt::white);

    /**
     * @brief m_drawGlyphs If sets to <i>true</i>, glyphs will be drawn, if
     * <i>false</i> will not.
     */
    bool m_drawGlyphs;

    /**
     * @brief m_selectorRenderer Obejct to render selector.
     */
    SelectorRenderer* m_selectorRenderer;

    /**
     * @brief m_glyphRenderer Object to render glyphs.
     */
    GlyphRenderer* m_glyphRenderer;

    /**
     * @brief m_shaderProgramSelector Its the shader for the selector.
     */
    QOpenGLShaderProgram* m_shaderProgramSelector;

    /**
     * @brief m_shaderProgramGlyph Its the shader for all glyphs.
     */
    QOpenGLShaderProgram* m_shaderProgramGlyph;

    /**
     * @brief m_isGlyphsVAOCreated If <i>true</i> the glyphs shaders are created,
     * <i>false</i> otherwise.
     */
    bool m_isGlyphsVAOCreated;

    /**
     * @brief m_selectionnedTrips Structure that contains all informations to get
     * stations and trips selected.
     */
    SelectionTimeStations m_selectionnedTrips;

    /**
     * @brief m_time Used to calculate time elapsed, for profiling fonctions.
     */
    QTime m_time;

    int m_frameCount;
    int m_lastTime;

signals:
    /**
     * @brief selectionChanged As soon as the <i>m_selectionnedTrips</i> value
     * changed, a signal is emitted to update glyphs and to draw them.
     * @param fromHour The beginning hour.
     * @param toHour The end hour.
     * @param fromStationIndex The first index station where
     * fromStationIndex <= toStationIndex.
     * @param toStationIndex The second index station where
     * toStationIndex >= fromStationIndex.
     */
    void selectionChanged(int fromHour, int toHour,
                          int fromStationIndex, int toStationIndex);

    /**
     * @brief onShaderError If an error occured during loading, compilation or
     * linking, this signal will be emitted.
     * @param message Message informations on errors.
     */
    void onShaderError(const QString& message);
};
#endif // MATRIXOPENGLWIDGET_H

#ifndef MATRIXOPENGLWIDGET_H
#define MATRIXOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>


class QWheelEvent;
class QMouseEvent;
class Station;
class Trip;
class QPoint;

class MatrixGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    MatrixGLWidget(QWidget* p = 0);
    ~MatrixGLWidget();

    void loadTripsAndStations(QVector<const Trip*>& trips, QVector<const Station*>& stations);

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
    void drawDragSelection();
    void drawDirections();

private:
    const int m_numberOfInterval = 24;
    const int m_matrixOffsetX = 10;
    int m_matrixViewWidth;
    int m_stationCircleSize = 10;

    QVector<QPoint> m_ellipses;

    bool m_leftMouseButtonPressed;
    QPointF m_previousMousePos;
    int m_translationValue;

    QPointF m_topLeftSelectionRectangle;
    QPointF m_bottomRightSelectionRectangle;

    bool m_drawRectangle;
    int m_dragSelectionBorderWidth;

    QList<const Station*> m_stations;
    QList<const Trip*> m_trips;



};
#endif // MATRIXOPENGLWIDGET_H

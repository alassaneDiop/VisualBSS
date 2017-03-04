#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "model.h"
#include "tripsfilter.h"
#include "tripsselector.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    Model* m_model;

    TripsFilter* m_filter;
    TripsSelector* m_selector;
    QSet<const Trip*> m_filteredTrips;
    QSet<const Trip*> m_selection;
    Station* m_highlight;

private slots:
    void onDataLoaded(QVector<const Trip*>& trips, QVector<const Station*>& stations);
    void onFailedToLoadData(const QString& filename);
    void onFilteredTripsChanged(const QList<const Trip*>& filteredTrips);
    void onSelectionChanged(const QList<const Trip*>& selection);
    void onHighlightChanged(const Station* highlight);

    void on_comboBox_period_currentIndexChanged(int index);
    void on_lineEdit_day_editingFinished();
    void on_comboBox_dayOfWeek_currentIndexChanged(int index);
    void on_checkBox_arrivals_stateChanged(int arg1);
    void on_checkBox_departures_stateChanged(int arg1);
    void on_checkBox_cycles_stateChanged(int arg1);
    void on_checkBox_duration_stateChanged(int arg1);
    void on_checkBox_distance_stateChanged(int arg1);
    void on_comboBox_order_currentIndexChanged(int index);
};

#endif // MAINWINDOW_H

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
    void onDataLoaded(const QList<const Station*>& stations);
    void onFilteredTripsChanged(const QList<const Trip*>& filteredTrips);
    void onSelectionChanged(const QList<const Trip*>& selection);
    void onHighlightChanged(const Station* highlight);
};

#endif // MAINWINDOW_H

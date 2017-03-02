#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QElapsedTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    showMaximized();
    ui->setupUi(this);

    showMaximized();
    ui->setupUi(this);

    m_model = &Model();
    connect(m_model, &Model::dataLoaded, this, &MainWindow::onDataLoaded);

    const QString filename = "../2013-07 - Citi Bike trip data.csv";
    QElapsedTimer timer;
    timer.start();
    const int result = m_model->loadData(filename);
    if (result >= 0)
        qDebug() << "Time to load" << result << "trips from the file in" << timer.elapsed() << "milliseconds";
    else
        qDebug() << "ERROR WHILE LOADING FILE" << filename;
}

MainWindow::~MainWindow()
{
    disconnect(m_model, &Model::dataLoaded, this, &MainWindow::onDataLoaded);
    delete ui;
}

void MainWindow::onDataLoaded(const QList<const Station*>& stations)
{
    // TODO : onDataLoaded
}

void MainWindow::onFilteredTripsChanged(const QList<const Trip*>& filteredTrips)
{
    // TODO : onFilteredTripsChanged
}

void MainWindow::onSelectionChanged(const QList<const Trip*>& selection)
{
    // TODO : onSelectionChanged
}

void MainWindow::onHighlightChanged(const Station* highlight)
{
    // TODO : onHighlightChanged
}

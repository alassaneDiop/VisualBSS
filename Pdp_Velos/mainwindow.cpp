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

    m_model = &Model();
    connect(m_model, &Model::dataLoaded, this, &MainWindow::onDataLoaded);
    connect(m_model, &Model::failedToLoadData, this, &MainWindow::onFailedToLoadData);

    const QString filename = "../2013-07 - Citi Bike trip data.csv";
    QElapsedTimer timer;
    timer.start();
    const int tripsCount = m_model->loadData(filename);
    if (tripsCount < 0)
        qWarning() << "Couldn't load data from file" << filename;
    else
        qInfo() << "Loaded" << tripsCount << "trips from file" << filename << "in" << timer.elapsed() << "milliseconds";
}

MainWindow::~MainWindow()
{
    m_model->disconnect();
    delete ui;
}

void MainWindow::onDataLoaded(const QList<const Station*>& stations)
{
    // TODO : onDataLoaded
}

void MainWindow::onFailedToLoadData(const QString& filename)
{
    // TODO : onFailedToLoadData
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

void MainWindow::on_comboBox_period_currentIndexChanged(int index)
{
    // TODO : on_comboBox_period_currentIndexChanged
}

void MainWindow::on_lineEdit_day_editingFinished()
{
    // TODO : on_lineEdit_day_editingFinished
}

void MainWindow::on_comboBox_dayOfWeek_currentIndexChanged(int index)
{
    // TODO : on_comboBox_dayOfWeek_currentIndexChanged
}

void MainWindow::on_checkBox_arrivals_stateChanged(int arg1)
{
    // TODO : on_checkBox_arrivals_stateChanged
}

void MainWindow::on_checkBox_departures_stateChanged(int arg1)
{
    // TODO : on_checkBox_departures_stateChanged
}

void MainWindow::on_checkBox_cycles_stateChanged(int arg1)
{
    // TODO : on_checkBox_cycles_stateChanged
}

void MainWindow::on_checkBox_duration_stateChanged(int arg1)
{
    // TODO : on_checkBox_duration_stateChanged
}

void MainWindow::on_checkBox_distance_stateChanged(int arg1)
{
    // TODO : on_checkBox_distance_stateChanged
}

void MainWindow::on_comboBox_order_currentIndexChanged(int index)
{
    // TODO : on_comboBox_order_currentIndexChanged
}

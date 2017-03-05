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

    m_model = new Model();
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
    delete m_model;
    delete ui;
}

void MainWindow::onDataLoaded(const QVector<Trip>& trips, const QVector<Station>& stations)
{
    qDebug() << "onDataLoaded" << "Trip number" << trips.size() << "Station number" << stations.size();

    // for x and y;
    int tuplePositionSize = 2;
    // 1 trip has 2 points (start/end)
    int pointPerTrip = 2;
    // R, G, B
    int tupleColorSize = 3;

    QVector<float> tripsVertices;
    tripsVertices.reserve(trips.size() * tuplePositionSize * pointPerTrip * tupleColorSize);
    int tripsVerticesCount = trips.size() * pointPerTrip;

    float maxLongitude = 180.f;
    float maxLatitude = 90.f;

    for (const Trip t : trips)
    {
        int startStationId = t.startStationId();
        const Station startStation = m_model->station(startStationId);

        tripsVertices.append((float) (startStation.longitude() / maxLongitude));
        tripsVertices.append((float) (startStation.latitude() / maxLatitude));

        tripsVertices.append(1.f);
        tripsVertices.append(0.f);
        tripsVertices.append(0.f);


        int endStationId = t.endStationId();
        const Station endStation = m_model->station(endStationId);

        tripsVertices.append((float) (endStation.longitude() / maxLongitude));
        tripsVertices.append((float) (endStation.latitude() / maxLatitude));

        tripsVertices.append(0.f);
        tripsVertices.append(0.f);
        tripsVertices.append(1.f);
    }

    QVector<float> stationsVertices;
    stationsVertices.reserve(stations.size() * tuplePositionSize);
    int stationsVerticesCount = stations.size();

    for (const Station s : stations)
    {
        stationsVertices.push_back(s.longitude() / maxLongitude);
        stationsVertices.push_back(s.latitude() / maxLatitude);
    }

    ui->mapwidget->loadStationsData(stationsVertices, stationsVerticesCount);
    ui->mapwidget->loadTripsData(tripsVertices, tripsVerticesCount);
    ui->mapwidget->centerView(stationsVertices);
}

void MainWindow::onFailedToLoadData(const QString& filename)
{
    // TODO : onFailedToLoadData
}

void MainWindow::onFilteredTripsChanged(const QVector<int>& filteredTrips)
{
    // TODO : onFilteredTripsChanged
}

void MainWindow::onSelectionChanged(const QVector<int>& selection)
{
    // TODO : onSelectionChanged
}

void MainWindow::onHighlightChanged(const int& highlight)
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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QElapsedTimer>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    showMaximized();
    ui->setupUi(this);

    m_model = new Model();

    // instantiate all useful objects
    m_filter = new TripsFilter();
    m_selector = new TripsSelector();
    m_stationsFilter = new StationsFilter();
    m_stationsSorter = new StationsSorter();

    connect(m_model, &Model::dataLoaded, this, &MainWindow::onDataLoaded);
    connect(m_model, &Model::failedToLoadData, this, &MainWindow::onFailedToLoadData);

    const QStringList args = QApplication::arguments();
    if (args.size() < 2)
        qInfo() << "You must specify a filename as parameter";
    else
    {
        const QString filename = args.at(1);
        QElapsedTimer timer;
        timer.start();
        const int tripsCount = m_model->loadData(filename);
        if (tripsCount < 0)
            qWarning() << "Couldn't load data from file" << filename;
        else
            qInfo() << "Loaded" << tripsCount << "trips from file" << filename << "in" << timer.elapsed() << "milliseconds";
    }
}

MainWindow::~MainWindow()
{
    m_model->disconnect();

    delete m_filter;
    delete m_selector;
    delete m_stationsFilter;
    delete m_stationsSorter;

    delete m_model;
    delete ui;
}

void MainWindow::filterTrips(const TripsFilterParams& params)
{
    if (m_filter)
    {
        m_filter->setParams(params);
        const QVector<bss::tripId> filteredTrips = m_filter->filter(m_model->constTrips());
        if (m_filteredTrips != filteredTrips)
        {
            m_filteredTrips = filteredTrips;
            onFilteredTripsChanged(filteredTrips);
        }
    }
}

void MainWindow::sortStations(const bss::SortOrder& orderParam)
{
    if (m_stationsSorter)
    {
        m_stationsSorter->setSortParam(orderParam);
        const QVector<bss::stationId> sortedStations = m_stationsSorter->sort(m_model->constStations());
        if (m_stationsOrder != sortedStations)
        {
            m_stationsOrder = sortedStations;
            onStationsOrderChanged(sortedStations);
        }
    }
}

void MainWindow::onDataLoaded(const QVector<Trip>& trips, const QVector<Station>& stations)
{
    qDebug() << "onDataLoaded" << "Trip number" << trips.size() << "Station number" << stations.size();

    // x and y;
    const unsigned short positionTupleSize = 2;

    // 1 trip has 2 points (start/end)
    const unsigned short pointPerTrip = 2;

    // R, G, B
    const unsigned short colorTupleSize = 3;

    QVector<float> tripsVertices;
    tripsVertices.reserve((trips.size() * positionTupleSize + trips.size() * colorTupleSize) * pointPerTrip);
    const int tripsVerticesCount = trips.size() * pointPerTrip;

    const float maxLongitude = 180.f;
    const float maxLatitude = 90.f;

    for (const Trip t : trips)
    {
        int startStationId = t.startStationId;
        const Station startStation = m_model->constStation(startStationId);

        tripsVertices.append((float) (startStation.longitude / maxLongitude));
        tripsVertices.append((float) (startStation.latitude / maxLatitude));

        // Departure RED
        tripsVertices.append(1.f);
        tripsVertices.append(0.f);
        tripsVertices.append(0.f);

        int endStationId = t.endStationId;
        const Station endStation = m_model->constStation(endStationId);

        tripsVertices.append((float) (endStation.longitude / maxLongitude));
        tripsVertices.append((float) (endStation.latitude / maxLatitude));

        // Arrival BLUE
        tripsVertices.append(0.f);
        tripsVertices.append(0.f);
        tripsVertices.append(1.f);
    }

    QVector<float> stationsVertices;
    const int stationsVerticesCount = stations.size();
    stationsVertices.reserve(stationsVerticesCount * positionTupleSize + stationsVerticesCount * colorTupleSize);

    for (const Station s : stations)
    {
        stationsVertices.append(s.longitude / maxLongitude);
        stationsVertices.append(s.latitude / maxLatitude);

        //        stationsVertices.append(1.f);
        //        stationsVertices.append(1.f);
        //        stationsVertices.append(0.3f);
    }

    // TEST D'AFFICHAGE DES GLYPHS
    // LARGEUR ET HAUTEUR
    float width = ui->timelinematrixwidget->width();
    float heigth = ui->timelinematrixwidget->height();
    float intervalX = ui->timelinematrixwidget->width() / 24.f;
    float intervalY = 10;
    int numberOfStations = 2000;

    const int offsetX = ui->timelinewidget->offsetX();
    float offsetY = 10;

    int numberOfHours = 24;

    QVector<float> glyphVertices;
    for (int i = 0; i < numberOfStations; ++i)
    {
        for (int j = 0; j < numberOfHours; ++j)
        {
            // VERTEX #1
            float positionX = offsetX + j * intervalX;
            float positionY = offsetY + i * intervalY;

            positionX = positionX / width * 2 - 1;
            positionY = positionY / heigth * 2 - 1;

            glyphVertices.append(positionX);
            glyphVertices.append(-positionY);

            // VERTEX #2
            positionX = offsetX + j * intervalX;
            positionY = offsetY + i * intervalY + 8;

            positionX = positionX / width * 2 - 1;
            positionY = positionY / heigth * 2 - 1;

            glyphVertices.append(positionX);
            glyphVertices.append(-positionY);
        }
    }

    qDebug() << "stations" << stationsVerticesCount << "trips" << tripsVerticesCount;

    bool drawTripsAndStations = true;

    if (drawTripsAndStations)
    {
        ui->mapwidget->loadStationsData(stationsVertices, stationsVerticesCount);
        ui->mapwidget->loadTripsData(tripsVertices, tripsVerticesCount);
        ui->mapwidget->centerView(stationsVertices);
    }
    else
    {
        ui->timelinematrixwidget->loadGlyphsData(glyphVertices, 2 * numberOfHours * numberOfStations);
    }
}

void MainWindow::onFailedToLoadData(const QString& filename)
{
    // TODO : onFailedToLoadData
}

void MainWindow::onFilteredTripsChanged(const QVector<bss::tripId>& filteredTrips)
{
    // TODO : onFilteredTripsChanged
}

void MainWindow::onSelectionChanged(const QVector<bss::tripId>& selection)
{
    // TODO : onSelectionChanged
}

void MainWindow::onStationsOrderChanged(const QVector<bss::stationId>& stationsOrder)
{
    // TODO : onStationsOrderChanged
}

void MainWindow::onHighlightChanged(const bss::stationId& highlight)
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
    if (m_filter)
    {
        TripsFilterParams params = m_filter->params();
        params.showArrivals = (arg1 == Qt::Checked);
        filterTrips(params);
    }
}

void MainWindow::on_checkBox_departures_stateChanged(int arg1)
{
    if (m_filter)
    {
        TripsFilterParams params = m_filter->params();
        params.showDepartures = (arg1 == Qt::Checked);
        filterTrips(params);
    }
}

void MainWindow::on_checkBox_cycles_stateChanged(int arg1)
{
    if (m_filter)
    {
        TripsFilterParams params = m_filter->params();
        params.showCycles = (arg1 == Qt::Checked);
        filterTrips(params);
    }
}

void MainWindow::on_checkBox_duration_stateChanged(int arg1)
{
    // TODO : on_checkBox_duration_stateChanged
}

void MainWindow::on_checkBox_distance_stateChanged(int arg1)
{
    if (m_filter)
    {
        TripsFilterParams params = m_filter->params();
        params.showDistance = (arg1 == Qt::Checked);
        filterTrips(params);
    }
}

void MainWindow::on_comboBox_order_currentIndexChanged(int index)
{
    if (m_stationsSorter)
    {
        // hard-coded array, order depends on UI's combobox
        const QVector<bss::SortOrder> sortParams = QVector<bss::SortOrder>(
        { bss::DISTANCE, bss::DURATION, bss::ARRIVALS, bss::DEPARTURES, bss::CYCLES });

        sortStations(sortParams.at(index));
    }
}

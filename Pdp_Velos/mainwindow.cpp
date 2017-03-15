#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtConcurrent>
#include <QFileDialog>


#include "config.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->menuBar->addMenu(createFilesMenu());

    // instantiate all useful objects
    m_model = new Model();
    m_filter = new TripsFilter();
    m_selector = new TripsSelector();
    m_stationsFilter = new StationsFilter();
    m_stationsSorter = new StationsSorter();
    m_futureWatcher = new QFutureWatcher<void>();

    // listening to himself (isn't there a better solution, like event queues ?)
    connect(this, &MainWindow::dataLoaded, this, &MainWindow::onDataLoaded, Qt::BlockingQueuedConnection);
    connect(this, &MainWindow::failedToLoadData, &MainWindow::onFailedToLoadData);
    connect(this, &MainWindow::dataUnloaded, &MainWindow::onDataUnloaded);

    connect(m_futureWatcher, &QFutureWatcher<void>::started, this, &MainWindow::onAsyncTaskStarted);
    connect(m_futureWatcher, &QFutureWatcher<void>::finished, this, &MainWindow::onAsyncTaskFinished);
}

MainWindow::~MainWindow()
{
    if (m_filter)
        delete m_filter;

    if (m_selector)
        delete m_selector;

    if (m_stationsFilter)
        delete m_stationsFilter;

    if (m_stationsSorter)
        delete m_stationsSorter;

    if (m_model)
        delete m_model;

    if (m_futureWatcher)
    {
        m_futureWatcher->waitForFinished();
        m_futureWatcher->disconnect();
        delete m_futureWatcher;
    }
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (m_futureWatcher->isRunning())
        event->ignore();
    else
        QMainWindow::closeEvent(event);
}



QMenu* MainWindow::createFilesMenu() const
{
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus, false);

    QAction* const openFilesAction = new QAction("Open");
    openFilesAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));

    QAction* const closeFilesAction = new QAction("Close all");
    closeFilesAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirClosedIcon));

    QMenu* const filesMenu = new QMenu("File");
    filesMenu->addAction(openFilesAction);
    filesMenu->addAction(closeFilesAction);

    connect(openFilesAction, &QAction::triggered, this, &MainWindow::onOpenFilesActionTriggered);
    connect(closeFilesAction, &QAction::triggered, this, &MainWindow::onCloseFilesActionTriggered);

    return filesMenu;
}



void MainWindow::filterTrips(const TripsFilterParams& params)
{
    // TODO : filterTrips
    QThread::sleep(6);

    if (m_filter)
    {
        const QVector<bss::tripId> filteredTrips = m_filter->filter(m_model->constTrips());
        if (m_filteredTrips != filteredTrips)
        {
            m_filteredTrips = filteredTrips;
            onFilteredTripsChanged(filteredTrips);
        }
    }
}

void MainWindow::sortStations(const bss::SortOrder& param)
{
    // TODO : filterTrips
    QThread::sleep(6);

    if (m_stationsSorter && m_model)
    {
        m_stationsSorter->setSortParam(param);
        const QVector<bss::stationId> sortedStations = m_stationsSorter->sort(m_model->constStations());
        if (m_stationsOrder != sortedStations)
        {
            m_stationsOrder = sortedStations;
            onStationsOrderChanged(sortedStations);
        }
    }
}



void MainWindow::onOpenFilesActionTriggered()
{
    const QString caption = "Choose files to open";
    const QString filter = "*.csv *.xml *.json";
    const QString dirPath = QDir::rootPath();
    const QStringList filenames = QFileDialog::getOpenFileNames(this, caption, dirPath, filter);


    if (m_futureWatcher && m_model && !filenames.isEmpty())
    {
        const auto task = [this](const QStringList& filenames)
        {
            for (const QString filename : filenames)
            {
                const DataLoadResult result = m_model->loadData(filename);
                if (result.info.ok)
                    emit dataLoaded(result.trips, result.stations);
                else
                    emit failedToLoadData(filename, result.info.errorString);
            }

        };
        m_futureWatcher->setFuture(QtConcurrent::run(task, filenames));
    }
}

void MainWindow::onCloseFilesActionTriggered()
{
    if (m_futureWatcher && m_model)
    {
        const auto task = [this]()
        {
            m_model->unloadData();
            emit dataUnloaded();
        };

        m_futureWatcher->setFuture(QtConcurrent::run(task));
    }
}



void MainWindow::onAsyncTaskStarted()
{
    ui->menuBar->setEnabled(false);
    ui->frame_controls->setEnabled(false);
}

void MainWindow::onAsyncTaskFinished()
{
    // restore menu bar and filter controls when an async task has finished running
    ui->menuBar->setEnabled(m_shouldEnableMenuBar);
    ui->frame_controls->setEnabled(m_shouldEnableControlsFrame);
}



void MainWindow::onDataLoaded(const QVector<Trip>& trips, const QVector<Station>& stations)
{
    m_shouldEnableControlsFrame = true;

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

        //        tripsVertices.append((float) (startStation.longitude / maxLongitude));
        //        tripsVertices.append((float) (startStation.latitude / maxLatitude));

        tripsVertices.append((float) (startStation.longitude / maxLongitude));
        tripsVertices.append((float) (startStation.latitude / maxLatitude));

        // incoming
        // red origin           1, 0, 0
        // yellow destination   1, 1, 0

        // outgoing
        // cyan origin          0, 1, 1
        // blue end             0, 0, 1
        tripsVertices.append(1.f);
        tripsVertices.append(0.f);
        tripsVertices.append(0.f);

        int endStationId = t.endStationId;
        const Station endStation = m_model->constStation(endStationId);

        tripsVertices.append((float) (endStation.longitude / maxLongitude));
        tripsVertices.append((float) (endStation.latitude / maxLatitude));

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

        stationsVertices.append(1.f);
        stationsVertices.append(1.f);
        stationsVertices.append(.3f);
    }

    // TEST D'AFFICHAGE DES GLYPHS
    // LARGEUR ET HAUTEUR
    const float width = ui->timelinematrixwidget->width();
    const float heigth = ui->timelinematrixwidget->height();
    const int numberOfStations = stations.size();

    // TODO: appeler un methode de timeline pour recuperer la valeur de numberOfHour
    const float intervalX = ui->timelinematrixwidget->width() / (bss::NB_OF_HOUR * 1.f);

    const int glyphHeight = bss::GLYPH_HEIGHT;
    const int glyphSpaceBetweenToLines = bss::SPACE_BETWEEN_GLYPHS;
    const int glyphIntervalY = glyphHeight + glyphSpaceBetweenToLines;

    QVector<float> glyphVertices;
    for (int i = 0; i < numberOfStations; ++i)
    {
        for (int j = 0; j < bss::NB_OF_HOUR; ++j)
        {
            //for (int k = 0; k < stations.size(); k++)
            {
                // JUST DRAWING LINE
                // VERTEX #1
                float positionX = bss::TIMELINE_OFFSET_X + j * intervalX;
                float positionY = i * glyphIntervalY;

                //float positionXT =

                positionX = positionX / width * 2 - 1;
                positionY = positionY / heigth * 2 - 1;

                glyphVertices.append(positionX);
                glyphVertices.append(-positionY);

                // VERTEX #2
                positionX = bss::TIMELINE_OFFSET_X + j * intervalX;
                positionY = i * glyphIntervalY + glyphHeight;

                positionX = positionX / width * 2 - 1;
                positionY = positionY / heigth * 2 - 1;

                glyphVertices.append(positionX);
                glyphVertices.append(-positionY);
            }
        }
    }

    ui->mapwidget->loadStationsData(stationsVertices, stationsVerticesCount);
    ui->mapwidget->loadTripsData(tripsVertices, tripsVerticesCount);
    ui->mapwidget->centerView(stationsVertices);
    ui->timelinematrixwidget->loadGlyphsData(glyphVertices, 2 * bss::NB_OF_HOUR * numberOfStations);
}

void MainWindow::onFailedToLoadData(const QString& filename, const QString& errorDesc)
{
    // TODO : onFailedToLoadData
}

void MainWindow::onDataUnloaded()
{
    m_shouldEnableControlsFrame = false;
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

void MainWindow::onTripsFilterParamsChanged(const TripsFilterParams& params)
{
    if (m_futureWatcher)
        m_futureWatcher->setFuture(QtConcurrent::run(this, &MainWindow::filterTrips, params));
}

void MainWindow::onStationsSorterParamChanged(const bss::SortOrder& param)
{
    if (m_futureWatcher)
        m_futureWatcher->setFuture(QtConcurrent::run(this, &MainWindow::sortStations, param));
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
    m_filterParams.showArrivals = (arg1 == Qt::Checked);
    onTripsFilterParamsChanged(m_filterParams);
}

void MainWindow::on_checkBox_departures_stateChanged(int arg1)
{
    m_filterParams.showDepartures = (arg1 == Qt::Checked);
    onTripsFilterParamsChanged(m_filterParams);
}

void MainWindow::on_checkBox_cycles_stateChanged(int arg1)
{
    m_filterParams.showCycles = (arg1 == Qt::Checked);
    onTripsFilterParamsChanged(m_filterParams);
}

void MainWindow::on_checkBox_duration_stateChanged(int arg1)
{
    // TODO : on_checkBox_duration_stateChanged
}

void MainWindow::on_checkBox_distance_stateChanged(int arg1)
{
    m_filterParams.showDistance = (arg1 == Qt::Checked);
    onTripsFilterParamsChanged(m_filterParams);
}

void MainWindow::on_comboBox_order_currentIndexChanged(int index)
{
    // hard-coded array, order depends on UI's combobox
    const QVector<bss::SortOrder> sortParams = QVector<bss::SortOrder>(
    { bss::DISTANCE, bss::DURATION, bss::ARRIVALS, bss::DEPARTURES, bss::CYCLES });

    onStationsSorterParamChanged(sortParams.at(index));
}

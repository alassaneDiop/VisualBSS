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
    const Qt::ConnectionType connectionType = (Qt::ConnectionType)(Qt::QueuedConnection | Qt::UniqueConnection);
    connect(m_futureWatcher, &QFutureWatcher<void>::started, this, &MainWindow::onAsyncTaskStarted, connectionType);
    connect(m_futureWatcher, &QFutureWatcher<void>::finished, this, &MainWindow::onAsyncTaskFinished, connectionType);

    connect(this, &MainWindow::dataLoaded, this, &MainWindow::onDataLoaded, connectionType);
    connect(this, &MainWindow::failedToLoadData, this, &MainWindow::onFailedToLoadData, connectionType);
    connect(this, &MainWindow::dataUnloaded, this, &MainWindow::onDataUnloaded, connectionType);

    const QObject* distanceRangeSlider = reinterpret_cast<QObject*>((QObject*)ui->rangeSlider_distance->rootObject());
    connect(distanceRangeSlider, SIGNAL(firstValueChanged(qreal)), SLOT(on_rangeSlider_distance_firstValueChanged(qreal)));
    connect(distanceRangeSlider, SIGNAL(secondValueChanged(qreal)), SLOT(on_rangeSlider_distance_secondValueChanged(qreal)));

    const QObject* durationRangeSlider = reinterpret_cast<QObject*>((QObject*)ui->rangeSlider_duration->rootObject());
    connect(durationRangeSlider, SIGNAL(firstValueChanged(qreal)), SLOT(on_rangeSlider_duration_firstValueChanged(qreal)));
    connect(durationRangeSlider, SIGNAL(secondValueChanged(qreal)), SLOT(on_rangeSlider_duration_secondValueChanged(qreal)));

    const QObject* odFlowRangeSlider = reinterpret_cast<QObject*>((QObject*)ui->rangeSlider_odFlow->rootObject());
    connect(odFlowRangeSlider, SIGNAL(firstValueChanged(qreal)), SLOT(on_rangeSlider_odFlow_firstValueChanged(qreal)));
    connect(odFlowRangeSlider, SIGNAL(secondValueChanged(qreal)), SLOT(on_rangeSlider_odFlow_secondValueChanged(qreal)));

    QObject* const directionRangeSlider = reinterpret_cast<QObject*>((QObject*)ui->rangeSlider_direction->rootObject());
    connect(directionRangeSlider, SIGNAL(firstValueChanged(qreal)), SLOT(on_rangeSlider_direction_firstValueChanged(qreal)));
    connect(directionRangeSlider, SIGNAL(secondValueChanged(qreal)), SLOT(on_rangeSlider_direction_secondValueChanged(qreal)));
    directionRangeSlider->setProperty("snapMode", "NoSnap");
    directionRangeSlider->setProperty("from", 0);
    directionRangeSlider->setProperty("to", 360);
    directionRangeSlider->setProperty("stepSize", 0.0001);


    /* It's needed to pass parameters of type QVector<T> to queued signals (it' apparently a Qt bug)
     * https://forum.qt.io/topic/2826/solved-cross-thread-signal-cannot-queue-arguments-of-type-qvector-qvector-int/3
     * By the way, the typedef suggested in the answer from the forum is not needed
     * */
    qRegisterMetaType<QVector<Trip>>("QVector<Trip>");
    qRegisterMetaType<QVector<Station>>("QVector<Station>");
}

MainWindow::~MainWindow()
{
    delete m_filter;
    delete m_selector;
    delete m_stationsFilter;
    delete m_stationsSorter;
    delete m_futureWatcher;
    delete m_model;
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
    connect(openFilesAction, &QAction::triggered, this, &MainWindow::onOpenFilesActionTriggered);

    QAction* const closeFilesAction = new QAction("Close all");
    closeFilesAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirClosedIcon));
    connect(closeFilesAction, &QAction::triggered, this, &MainWindow::onCloseFilesActionTriggered);

    QMenu* const filesMenu = new QMenu("File");
    filesMenu->addAction(openFilesAction);
    filesMenu->addAction(closeFilesAction); 

    return filesMenu;
}

template<typename T>
void MainWindow::runAsync(const QFuture<T>& future)
{
    m_futureWatcher->setFuture(future);
}

bool MainWindow::loadData(const QStringList& filenames)
{
    bool hasLoadedData = false;
    for (const QString filename : filenames)
    {
        const DataLoadResult loadResult = m_model->loadData(filename);
        if (!loadResult.info.ok)
            emit failedToLoadData(filename, loadResult.info.errorString);
        else
        {
            hasLoadedData = true;
            emit dataLoaded(loadResult.trips, loadResult.stations);
        }
    }
    return hasLoadedData;
}

bool MainWindow::unloadData()
{
    if (!m_model->unloadData())
        return false;
    else
    {
        emit dataUnloaded();
        return true;
    }
}

void MainWindow::filterTrips(const TripsFilterParams& params)
{
    m_filter->setParams(params);
    const QVector<bss::tripId> filteredTrips = m_filter->filter(m_model->constTrips());
    if (m_filteredTrips != filteredTrips)
    {
        m_filteredTrips = filteredTrips;
        onFilteredTripsChanged(filteredTrips);
    }
}

void MainWindow::sortStations(const bss::SortOrder& param)
{
    m_stationsSorter->setSortParam(param);
    const QVector<bss::stationId> sortedStations = m_stationsSorter->sort(m_model->constStations());
    if (m_stationsOrder != sortedStations)
    {
        m_stationsOrder = sortedStations;
        onStationsOrderChanged(sortedStations);
    }
}



void MainWindow::onOpenFilesActionTriggered()
{
    const QString caption = "Choose files to open";
    const QString filter = "*.csv *.xml *.json";
    const QString dirPath = QDir::rootPath();
    const QStringList filenames = QFileDialog::getOpenFileNames(this, caption, dirPath, filter);

    runAsync(QtConcurrent::run(this, &MainWindow::loadData, filenames));
}

void MainWindow::onCloseFilesActionTriggered()
{
    runAsync(QtConcurrent::run(this, &MainWindow::unloadData));
}



void MainWindow::onAsyncTaskStarted()
{
    // disables menu bar and filter controls when an async task has finished running
    ui->menuBar->setEnabled(false);
    ui->frame_controls->setEnabled(false);
}

void MainWindow::onAsyncTaskFinished()
{
    // restores menu bar and filter controls when an async task has finished running
    ui->menuBar->setEnabled(m_shouldEnableMenuBar);
    ui->frame_controls->setEnabled(m_shouldEnableControlsFrame);
}



void MainWindow::onDataLoaded(const QVector<Trip>& trips, const QVector<Station>& stations)
{
    // we know we have data to display and manipulate so we can enable controls
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
    // data aren't available anymore so controls are disabled
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
    runAsync(QtConcurrent::run(this, &MainWindow::filterTrips, params));
}

void MainWindow::onStationsSorterParamChanged(const bss::SortOrder& param)
{
    runAsync(QtConcurrent::run(this, &MainWindow::sortStations, param));
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
    m_filterParams.shouldShowArrivals = (arg1 == Qt::Checked);
    onTripsFilterParamsChanged(m_filterParams);
}

void MainWindow::on_checkBox_departures_stateChanged(int arg1)
{
    m_filterParams.shouldShowDepartures = (arg1 == Qt::Checked);
    onTripsFilterParamsChanged(m_filterParams);
}

void MainWindow::on_checkBox_cycles_stateChanged(int arg1)
{
    m_filterParams.shouldShowCycles = (arg1 == Qt::Checked);
    onTripsFilterParamsChanged(m_filterParams);
}

void MainWindow::on_checkBox_duration_stateChanged(int arg1)
{
    // TODO : on_checkBox_duration_stateChanged
}

void MainWindow::on_checkBox_distance_stateChanged(int arg1)
{
    m_filterParams.shouldShowDistance = (arg1 == Qt::Checked);
    onTripsFilterParamsChanged(m_filterParams);
}

void MainWindow::on_comboBox_order_currentIndexChanged(int index)
{
    // hard-coded array, order depends on UI's combobox
    const QVector<bss::SortOrder> sortParams = QVector<bss::SortOrder>(
    { bss::DISTANCE, bss::DURATION, bss::ARRIVALS, bss::DEPARTURES, bss::CYCLES });

    onStationsSorterParamChanged(sortParams.at(index));
}

void MainWindow::on_rangeSlider_distance_firstValueChanged(qreal v)
{
    ui->lineEdit_minDistance->setText(QString::number(v));
}

void MainWindow::on_rangeSlider_distance_secondValueChanged(qreal v)
{
    ui->lineEdit_maxDistance->setText(QString::number(v));
}

void MainWindow::on_rangeSlider_duration_firstValueChanged(qreal v)
{
    ui->lineEdit_minDuration->setText(QString::number(v));
}

void MainWindow::on_rangeSlider_duration_secondValueChanged(qreal v)
{
    ui->lineEdit_maxDuration->setText(QString::number(v));
}

void MainWindow::on_rangeSlider_direction_firstValueChanged(qreal v)
{
    ui->lineEdit_minDirection->setText(QString::number(v));
}

void MainWindow::on_rangeSlider_direction_secondValueChanged(qreal v)
{
    ui->lineEdit_maxDirection->setText(QString::number(v));
}

void MainWindow::on_rangeSlider_odFlow_firstValueChanged(qreal v)
{
    ui->lineEdit_minOdFlow->setText(QString::number(v));
}

void MainWindow::on_rangeSlider_odFlow_secondValueChanged(qreal v)
{
    ui->lineEdit_maxOdFlow->setText(QString::number(v));
}

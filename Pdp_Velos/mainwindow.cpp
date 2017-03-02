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
}

MainWindow::MainWindow(QWidget* parent, Model* m):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_model(m)
{
    showMaximized();
    ui->setupUi(this);

    connect(m_model, &Model::dataLoaded,
            ui->mapwidget, &MapGLWidget::onDataLoaded);

    connect(m_model, &Model::dataLoaded,
            ui->timelinematrixwidget, &MatrixGLWidget::onDataLoaded);

    if (m_model)
    {
        const QString filename = "../2013-07 - Citi Bike trip data.csv";
        QElapsedTimer timer;
        timer.start();
        const int result = m_model->loadData(filename);
        if (result >= 0)
            qDebug() << "Time to load" << result << "trips from the file in" << timer.elapsed() << "milliseconds";
        else
            qDebug() << "ERROR WHILE LOADING FILE" << filename;
    }
}

MainWindow::~MainWindow()
{
    disconnect(m_model, &Model::dataLoaded,
            ui->mapwidget, &MapGLWidget::onDataLoaded);

    disconnect(m_model, &Model::dataLoaded,
            ui->timelinematrixwidget, &MatrixGLWidget::onDataLoaded);

    delete ui;
}

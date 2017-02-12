#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "model.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    MainWindow(QWidget* parent, Model* m);
    ~MainWindow();

signals:
    void dataLoaded(const QList<Station>& stations, const QList<Trip>& trips);

private:
    Ui::MainWindow* ui;
    Model* m_model;
};

#endif // MAINWINDOW_H

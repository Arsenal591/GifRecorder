#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qdebug.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent, Qt::WindowStaysOnTopHint)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent* ev) {
    auto mainRegion = QRegion(this->rect());
    auto cameraSize = ui->camera->size();
    auto cameraPos = ui->camera->mapToParent(ui->camera->pos());
    auto cameraRegion = QRegion(QRect(cameraPos, cameraSize));
    this->setMask(mainRegion.subtracted(cameraRegion));
}


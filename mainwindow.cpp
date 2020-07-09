#include "qdatetime.h"
#include "qdebug.h"
#include "qtimer.h"
#include "qwindow.h"
#include "qscreen.h"

#include "gif.h"

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


void MainWindow::on_recordButton_clicked()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (const QWindow *window = windowHandle())
        screen = this->windowHandle()->screen();
    if (!screen)
    {
        return;
    }
    auto cameraPos = this->mapToGlobal(ui->camera->pos());
    auto cameraSize = ui->camera->size();

    auto pixmap = screen->grabWindow(0, cameraPos.x(), cameraPos.y(), cameraSize.width(), cameraSize.height());
    pixmap.save("screenshot.png");
}

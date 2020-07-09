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
    , ui(new Ui::MainWindow), timer(nullptr)
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

void MainWindow::on_timer_fired() {
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
    buf.push_back(pixmap);
}


void MainWindow::on_recordButton_clicked()
{
    if(timer == nullptr) {
        timer = new QTimer(this);
        timer->setTimerType(Qt::PreciseTimer);
        connect(timer, &QTimer::timeout, this, &MainWindow::on_timer_fired);
        timer->start(33);
    } else {
        delete timer;
        if(buf.size()) {
            GifWriter g;
            int width = buf[0].width(), height = buf[0].height();
            GifBegin(&g, "out.gif", width, height, 0);
            for(auto& img: buf) {
                GifWriteFrame(&g, img.toImage().convertToFormat(QImage::Format_RGBA8888).bits(), img.width(), img.height(), 3);
            }
            GifEnd(&g);
        }
    }
}

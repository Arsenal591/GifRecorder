#include "qdatetime.h"
#include "qdebug.h"
#include "qdir.h"
#include "qfiledialog.h"
#include "qimagewriter.h"
#include "qtimer.h"
#include "qwindow.h"
#include "qscreen.h"
#include "qstandardpaths.h"

#include "gif.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

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

    }
}

void MainWindow::on_saveButton_clicked()
{
    QString initialPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if(initialPath.isEmpty()) {
        initialPath = QDir::currentPath();
    }
    initialPath += tr("/untitled") + ".gif";
    QFileDialog fileDialog(this, tr("Save As"), initialPath);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList mimeTypes;
    mimeTypes.append("image/gif");

    fileDialog.setMimeTypeFilters(mimeTypes);
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    std::string dest = fileDialog.selectedFiles().first().toStdString();
    if(buf.size()) {
        GifWriter g;
        int width = buf[0].width(), height = buf[0].height();
        GifBegin(&g, dest.data(), width, height, 1);
        for(auto& img: buf) {
            GifWriteFrame(&g, img.toImage().convertToFormat(QImage::Format_RGBA8888).bits(), img.width(), img.height(), 3);
        }
        GifEnd(&g);
    }
}

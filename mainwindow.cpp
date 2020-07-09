#include "qdatetime.h"
#include "qdebug.h"
#include "qdir.h"
#include "qevent.h"
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
    , ui(new Ui::MainWindow), timer(nullptr), resizeType(ResizeNone)
{
    ui->setupUi(this);
    setMouseTracking(true);
    ui->centralwidget->setMouseTracking(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent* ev) {
    auto mainRegion = QRegion(this->rect());
    auto cameraSize = ui->camera->size();
    auto cameraPos = ui->camera->mapToParent(ui->camera->pos());
    auto cameraRegion = QRegion(QRect(cameraPos, cameraSize));
    this->setMask(mainRegion.subtracted(cameraRegion));
}

void MainWindow::mouseMoveEvent(QMouseEvent *ev) {
    QPoint pos = ev->pos();
    ResizeType type = calculateCursorPosition(pos);
    if(type == ResizeLeft || type == ResizeRight) {
        setCursor(Qt::SizeHorCursor);
    } else if(type == ResizeTop || type == ResizeBottom) {
        setCursor(Qt::SizeVerCursor);
    } else if(type == ResizeTopLeft || type == ResizeBottomRight){
        setCursor(Qt::SizeFDiagCursor);
    } else if(type == ResizeTopRight || type == ResizeBottomLeft) {
        setCursor(Qt::SizeBDiagCursor);
    } else{
        setCursor(Qt::ArrowCursor);
    }

    if(ev->buttons() & Qt::LeftButton) {
        auto coord = ev->globalPos();
        auto geo = geometry(), oriGeo = geometry();
        if(resizeType == ResizeRight || resizeType == ResizeTopRight || resizeType == ResizeBottomRight) {
            geo.setRight(coord.x());
        }
        if(resizeType == ResizeLeft || resizeType == ResizeTopLeft || resizeType == ResizeBottomLeft) {
            geo.setLeft(coord.x());
        }
        if(resizeType == ResizeTop || resizeType == ResizeTopLeft || resizeType == ResizeTopRight) {
            geo.setTop(coord.y());
        }
        if(resizeType == ResizeBottom || resizeType == ResizeBottomLeft || resizeType == ResizeBottomRight) {
            geo.setBottom(coord.y());
        }
        this->setGeometry(geo);
    }

}

void MainWindow::mousePressEvent(QMouseEvent *ev) {
    if(ev->button() == Qt::LeftButton) {
        resizeType = calculateCursorPosition(ev->pos());
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *ev) {
    resizeType = ResizeNone;
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

ResizeType MainWindow::calculateCursorPosition(const QPoint& pos) {
    const int r = 10;
    bool onLeft = pos.x() <= r;
    bool onRight = pos.x() >= this->width() - r;
    bool onTop = pos.y() <= r;
    bool onBottom = pos.y() >= this->height() - r;
    if(onLeft && onTop) {
        return ResizeTopLeft;
    } if(onLeft && onBottom) {
        return ResizeBottomLeft;
    } if(onRight && onTop) {
        return ResizeTopRight;
    } if(onRight && onBottom){
        return ResizeBottomRight;
    } if(onLeft) {
        return ResizeLeft;
    } if(onRight) {
        return ResizeRight;
    } if(onTop) {
        return ResizeTop;
    } if(onBottom) {
        return ResizeBottom;
    }
    return ResizeDrag;
}

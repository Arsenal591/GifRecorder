#include "qaction.h"
#include "qcombobox.h"
#include "qdatetime.h"
#include "qdebug.h"
#include "qdir.h"
#include "qevent.h"
#include "qfiledialog.h"
#include "qimagewriter.h"
#include "qmenu.h"
#include "qprogressdialog.h"
#include "qscreen.h"
#include "qstandardpaths.h"
#include "qtimer.h"
#include "qwindow.h"

#include "gif.h"
#include "Magick++.h"

#include "settingsdialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent, Qt::WindowStaysOnTopHint)
    , ui(new Ui::MainWindow), timer(nullptr), resizeType(ResizeNone), settings(Settings())
{
    ui->setupUi(this);
    for(auto& child : this->findChildren<QWidget*>()) {
        child->installEventFilter(this);
    }
    
    setMouseTracking(true);
    ui->centralwidget->setMouseTracking(true);

    auto newAction = new QAction("New file", this);
    connect(newAction, &QAction::triggered, this, &MainWindow::clearBuffer);
    auto* menu = new QMenu(this);
    menu->addAction(newAction);
    ui->menuButton->setMenu(menu);

    auto icon = QApplication::style()->standardIcon(QStyle::SP_TitleBarCloseButton);
    ui->closeButton->setIcon(icon);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::paintEvent(QPaintEvent* ev) {
    auto mainRegion = QRegion(this->rect());
    auto cameraRegion = QRegion(ui->camera->geometry());
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
        if(resizeType == ResizeDrag) {
            auto newPos = ev->globalPos();
            geo.translate(newPos.x() - previousPos.x(), newPos.y() - previousPos.y());
            previousPos = newPos;
        }
        this->setGeometry(geo);
    }

}

void MainWindow::mousePressEvent(QMouseEvent *ev) {
    if(ev->button() == Qt::LeftButton) {
        resizeType = calculateCursorPosition(ev->pos());
        previousPos = ev->globalPos();
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
    ui->progessText->setText(QString::asprintf("Frame %d", buf.size()));
}


void MainWindow::on_recordButton_clicked()
{
    if(timer == nullptr) {
        ui->editButton->setEnabled(false);
        ui->saveButton->setEnabled(false);
        ui->menuButton->setEnabled(false);
        timer = new QTimer(this);
        timer->setTimerType(Qt::PreciseTimer);
        connect(timer, &QTimer::timeout, this, &MainWindow::on_timer_fired);
        timer->start(settings.fps);
    } else {
        ui->editButton->setEnabled(true);
        ui->saveButton->setEnabled(true);
        ui->menuButton->setEnabled(true);
        delete timer;
        timer = nullptr;
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
        QProgressDialog dia("Saving...", "", 0, buf.size() + 1, this);
        dia.show();

        std::vector<Magick::Image> frames;
        int idx = 0;
        for(auto& pixmap: buf) {
            idx += 1;
            auto img = pixmap.toImage();
            if(settings.color == "GrayScale") {
                img = img.convertToFormat(QImage::Format_Grayscale8);
            }
            img = img.convertToFormat(QImage::Format_RGB888);

            int width = img.width(), height = img.height();
            Magick::Image frame(Magick::Geometry(width, height), Magick::ColorRGB(0, 0, 0));
            auto pixels = frame.setPixels(0, 0, width, height);
            frame.modifyImage();
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    QColor pix = img.pixel(x, y);
                    auto offset = frame.channels() * (frame.columns() * y + x);
                    pixels[offset] = pix.redF() * 65536 ;
                    pixels[offset + 1] = pix.greenF() * 65536;
                    pixels[offset + 2] = pix.blueF() * 65536;
                }
            }
            frame.syncPixels();
            frame.animationDelay(settings.fps);
            frames.push_back(frame);

            dia.setValue(idx);
            QCoreApplication::processEvents();
        }
        Magick::optimizeTransparency(frames.begin(), frames.end());
        Magick::writeImages(frames.begin(), frames.end(), dest);
        dia.setValue(buf.size() + 1);
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
        return ResizeDrag;
    } if(onRight) {
        return ResizeRight;
    } if(onTop) {
        return ResizeDrag;
    } if(onBottom) {
        return ResizeBottom;
    }
    return ResizeDrag;
}

void MainWindow::on_settingsButton_clicked()
{
    SettingsDialog dialog;
    dialog.exec();
    settings.fps = dialog.findChild<QComboBox*>("fpsCombo")->currentText().toInt();
    settings.color = dialog.findChild<QComboBox*>("colorCombo")->currentText().toStdString();
}

void MainWindow::on_menuButton_clicked()
{
    ui->menuButton->showMenu();
}

void MainWindow::clearBuffer() {
    this->buf.clear();
    this->ui->progessText->setText("");
}

void MainWindow::on_closeButton_clicked()
{
    QCoreApplication::quit();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *ev) {
    if(obj != ui->centralwidget) {
        setCursor(Qt::ArrowCursor);
    }
    return false;
}

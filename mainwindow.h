#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum ResizeType {
    ResizeLeft,
    ResizeRight,
    ResizeTop,
    ResizeBottom,
    ResizeTopLeft,
    ResizeTopRight,
    ResizeBottomLeft,
    ResizeBottomRight,
    ResizeDrag,
    ResizeNone,
};

struct Settings {
    int fps;
    std::string color;
    Settings(): fps(33), color("RGB") {}
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    bool eventFilter(QObject *obj, QEvent *ev) override;

private slots:
    void on_recordButton_clicked();
    void on_timer_fired();
    void on_saveButton_clicked();
    void on_settingsButton_clicked();
    void on_menuButton_clicked();
    void on_closeButton_clicked();
    void on_editButton_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QVector<QPixmap> buf;
    ResizeType resizeType;
    QPoint previousPos;
    Settings settings;

    ResizeType calculateCursorPosition(const QPoint& pos);
    void clearBuffer();
};
#endif // MAINWINDOW_H

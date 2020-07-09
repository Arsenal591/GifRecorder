#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void showEvent(QShowEvent*);

private slots:
    void on_recordButton_clicked();
    void on_timer_fired();

    void on_saveButton_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QVector<QPixmap> buf;
};
#endif // MAINWINDOW_H

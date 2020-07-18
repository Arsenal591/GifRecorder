#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>

namespace Ui {
class EditDialog;
}

class EditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditDialog(QWidget *parent, QVector<QPixmap>& images);
    ~EditDialog();

private slots:
    void on_goPreviousButton_clicked();

    void on_goNextButton_clicked();

    void on_displayedImage_customContextMenuRequested(const QPoint &pos);

private:
    Ui::EditDialog *ui;
    QVector<QPixmap>& images;
    int idx;
    void setIndex(int);
    void removeCurrent();
};

#endif // EDITDIALOG_H

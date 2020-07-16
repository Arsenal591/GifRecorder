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

private:
    Ui::EditDialog *ui;
    QVector<QPixmap>& images;
    int idx;
    void setIndex(int);
};

#endif // EDITDIALOG_H

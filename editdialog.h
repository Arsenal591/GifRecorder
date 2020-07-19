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
    QVector<QPixmap> getImages();

private slots:
    void on_goPreviousButton_clicked();

    void on_goNextButton_clicked();

    void on_displayedImage_customContextMenuRequested(const QPoint &pos);

    void on_heightEdit_textChanged(const QString &arg1);

    void on_widthEdit_textEdited(const QString &arg1);

private:
    Ui::EditDialog *ui;
    QVector<QPixmap>& images;
    int idx;
    QSize size;
    void setIndex(int);
    void removeCurrent();
};

#endif // EDITDIALOG_H

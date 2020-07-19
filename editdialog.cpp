#include "qaction.h"
#include "qmenu.h"
#include "qdebug.h"

#include "editdialog.h"
#include "ui_editdialog.h"

EditDialog::EditDialog(QWidget *parent, QVector<QPixmap>& images) :
    QDialog(parent),
    ui(new Ui::EditDialog),
    images(images)
{
    ui->setupUi(this);
    if(!images.empty()) {
        this->size = images[0].size();
        ui->widthEdit->setText(QString::number(this->size.width()));
        ui->heightEdit->setText(QString::number(this->size.height()));
        ui->keepRatioCheck->setChecked(true);
    } else {
        ui->widthEdit->setEnabled(false);
        ui->heightEdit->setEnabled(false);
        ui->keepRatioCheck->setEnabled(false);
    }
}

EditDialog::~EditDialog()
{
    delete ui;
}

QVector<QPixmap> EditDialog::getImages() {
    return QVector<QPixmap>(this->images);
}

void EditDialog::setIndex(int idx) {

    if(idx < 0) idx = 0;
    if(!this->images.empty() && idx >= this->images.size()) idx = this->images.size() - 1;
    this->idx = idx;

    if(idx < images.size()) {
        auto pixmap = this->images[idx].scaled(this->size);
        pixmap = pixmap.scaled(ui->displayedImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->displayedImage->setPixmap(pixmap);
    } else {
        ui->displayedImage->clear();
    }
    ui->goPreviousButton->setEnabled(idx > 0);
    ui->goNextButton->setEnabled(idx < this->images.size() - 1);
    this->setWindowTitle(QString::asprintf("Edit frame %d/%d", idx + 1, this->images.size()));
}

void EditDialog::removeCurrent() {
    this->images.remove(this->idx);
    this->setIndex(this->idx);
}

void EditDialog::on_goPreviousButton_clicked()
{
    this->setIndex(this->idx - 1);
}

void EditDialog::on_goNextButton_clicked()
{
    this->setIndex(this->idx + 1);
}

void EditDialog::on_displayedImage_customContextMenuRequested(const QPoint &pos)
{
    if(this->images.empty()) return;
    QMenu contextMenu(this);

    QAction action1("Delete frame", this);
    connect(&action1, &QAction::triggered, this, &EditDialog::removeCurrent);
    contextMenu.addAction(&action1);

    contextMenu.exec(mapToGlobal(pos));
}

void EditDialog::on_heightEdit_textChanged(const QString &arg1)
{
    if(ui->keepRatioCheck->isChecked()) {
        this->size *= (1.0 * arg1.toInt() / this->size.height());
    } else {
        this->size.setHeight(arg1.toInt());
    }
    ui->widthEdit->setText(QString::number(this->size.width()));
    ui->heightEdit->setText(QString::number(this->size.height()));
    this->setIndex(this->idx);
}

void EditDialog::on_widthEdit_textEdited(const QString &arg1)
{
    if(ui->keepRatioCheck->isChecked()) {
        this->size *= (1.0 * arg1.toInt() / this->size.width());
    } else {
        this->size.setWidth(arg1.toInt());
    }
    ui->widthEdit->setText(QString::number(this->size.width()));
    ui->heightEdit->setText(QString::number(this->size.height()));
    this->setIndex(this->idx);
}

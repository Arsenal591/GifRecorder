#include "qaction.h"
#include "qmenu.h"

#include "editdialog.h"
#include "ui_editdialog.h"

EditDialog::EditDialog(QWidget *parent, QVector<QPixmap>& images) :
    QDialog(parent),
    ui(new Ui::EditDialog),
    images(images)
{
    ui->setupUi(this);
    this->setIndex(0);
}

EditDialog::~EditDialog()
{
    delete ui;
}

void EditDialog::setIndex(int idx) {
    if(idx < 0) idx = 0;
    if(!this->images.empty() && idx >= this->images.size()) idx = this->images.size() - 1;
    this->idx = idx;

    if(idx < images.size()) {
        ui->displayedImage->setPixmap(this->images[idx]);
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

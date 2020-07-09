#include "qdebug.h"
#include "qevent.h"

#include "silentwidget.h"

SilentWidget::SilentWidget(QWidget* parent): QWidget(parent) {
    setMouseTracking(true);
}

SilentWidget::~SilentWidget() {

}

void SilentWidget::mouseMoveEvent(QMouseEvent *event) {
    event->ignore();
}

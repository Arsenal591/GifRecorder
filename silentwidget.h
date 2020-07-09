#ifndef BUTTONCONTAINER_H
#define BUTTONCONTAINER_H

#endif // BUTTONCONTAINER_H

#include "qwidget.h"

class SilentWidget: public QWidget
{
    Q_OBJECT
public:
    SilentWidget(QWidget*);
    ~SilentWidget();

protected:
    void mouseMoveEvent(QMouseEvent *event);
};

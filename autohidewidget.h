#ifndef AUTOHIDEWIDGET_H
#define AUTOHIDEWIDGET_H

#include <QWidget>

class AutoHideWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AutoHideWidget(QWidget *parent = 0);
    
protected:
    void leaveEvent(QEvent *event);
};

#endif // AUTOHIDEWIDGET_H

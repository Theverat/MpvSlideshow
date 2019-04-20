#include "autohidewidget.h"

AutoHideWidget::AutoHideWidget(QWidget *parent) : QWidget(parent)
{}

void AutoHideWidget::leaveEvent(QEvent *event) {
    hide();
}

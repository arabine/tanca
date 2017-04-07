#ifndef EVENTWINDOW_H
#define EVENTWINDOW_H

#include <QDialog>
#include "DbManager.h"
#include "ui_EventWindow.h"

class EventWindow : public QDialog
{
    Q_OBJECT

public:
    EventWindow(QWidget *parent = 0);
    void GetEvent(Event &event);
    void SetEvent(const Event &event);

private:
    Ui::EventWindow ui;
};

#endif // EVENTWINDOW_H

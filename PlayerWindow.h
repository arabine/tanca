#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QDialog>
#include "ui_PlayerWindow.h"
#include "DbManager.h"

class PlayerWindow : public QDialog
{
    Q_OBJECT
public:
    explicit PlayerWindow(QWidget *parent = 0);

    Person GetPerson();

signals:

public slots:

private:
    Ui::PlayerWindow ui;
    Person mPerson;
};

#endif // PLAYERWINDOW_H

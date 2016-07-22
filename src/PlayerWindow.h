#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QDialog>
#include "ui_PlayerWindow.h"
#include "DatePickerWindow.h"
#include "DbManager.h"

class PlayerWindow : public QDialog
{
    Q_OBJECT
public:
    explicit PlayerWindow(QWidget *parent = 0);

    void GetPlayer(Player &player);
    void SetPlayer(const Player &player);

private slots:
    void slotAddLicence();
private:
    DatePickerWindow *datePickerWindow;

    Ui::PlayerWindow ui;
};

#endif // PLAYERWINDOW_H

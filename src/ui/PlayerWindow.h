#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QDialog>
#include "ui_PlayerWindow.h"
#include "DatePickerWindow.h"
#include "DbManager.h"
#include <QTableWidget>

class PlayerWindow : public QDialog
{
    Q_OBJECT
public:
    explicit PlayerWindow(QWidget *parent = 0);

    bool AddPlayer(DbManager &db);
    bool EditPlayer(DbManager &db, QTableWidget *widget);
    bool DeletePlayer(DbManager &db, QTableWidget *widget);
    bool ImportPlayerFile(DbManager &db);

private slots:
    void slotAddLicence();
    void slotAccept();
private:
    void GetPlayer(Player &player);
    void SetPlayer(const Player &player);

    DatePickerWindow *datePickerWindow;

    Ui::PlayerWindow ui;
};

#endif // PLAYERWINDOW_H

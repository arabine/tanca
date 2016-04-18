#ifndef TEAMWINDOW_H
#define TEAMWINDOW_H

#include <QDialog>
#include "ui_TeamWindow.h"
#include "DbManager.h"

class TeamWindow : public QDialog
{
    Q_OBJECT

public:
    TeamWindow(QWidget *parent);

    Team GetTeam() { return mTeam; }

private slots:
    void slotAccept();

private:
    Ui::TeamWindow ui;
    Team mTeam;
};

#endif // TEAMWINDOW_H

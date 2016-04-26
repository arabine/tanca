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

    void SetPlayers(const QList<Player> &players);

private slots:
    void slotAccept();

    void slotClicked();
    void slotMemberItemActivated(QListWidgetItem *item);
    void slotTeamItemActivated(QListWidgetItem *item);
private:
    Ui::TeamWindow ui;
    Team mTeam;

    QList<Player> mList;
    QList<Player> mSelection;
    void Update();
};

#endif // TEAMWINDOW_H

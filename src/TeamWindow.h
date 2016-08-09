#ifndef TEAM_WINDOW_H
#define TEAM_WINDOW_H

#include "SelectionWindow.h"
#include "DbManager.h"

class TeamWindow : public SelectionWindow
{
    Q_OBJECT

public:
    TeamWindow(QWidget *parent);

    void SetTeam(const Player &p1, const Player &p2);
    void GetTeam(Team &team);

    void Initialize(const QList<Player> &players, const QList<int> &inTeams);

    void ClickedRight(int index);
    void ClickedLeft(int id);

private:
    QList<Player> mList;
    QList<Player> mSelection;
    void Update();    
};

#endif // TEAM_WINDOW_H

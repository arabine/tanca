#ifndef TEAM_WINDOW_H
#define TEAM_WINDOW_H

#include "SelectionWindow.h"
#include "DbManager.h"
#include "UniqueId.h"

class TeamWindow : public SelectionWindow
{
    Q_OBJECT

public:
    TeamWindow(QWidget *parent);

    void SetTeam(const Player &p1, const Player &p2, const Team &team);
    void GetTeam(Team &team);

    void Initialize(const QList<Player> &players, const QList<int> &inTeams, bool isEdit);

    void ClickedRight(int index);
    void ClickedLeft(int id);

    void ClearIds() { mTeamsId.Clear(); }
    void AddId(std::uint32_t id) { mTeamsId.AddId(id); }
     void ListIds();

private slots:
    void slotAccept();

private:
    QList<Player> mList;
    QList<Player> mSelection;
    UniqueId mTeamsId;
    bool mIsEdit;

    void Update();
};

#endif // TEAM_WINDOW_H

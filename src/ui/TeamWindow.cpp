#include "TeamWindow.h"
#include "TableHelper.h"
#include "Log.h"



TeamWindow::TeamWindow(QWidget *parent)
    : SelectionWindow(parent, tr("Créer/modifier une équipe"), 2, 2)
{
    QStringList header;
    header << tr("Id") << tr("Prénom") << tr("Nom") << tr("Pseudo");
    SetHeader(header);

    SetLabelNumber(tr("Équipe N°:"));

    connect(ui.buttonOk, SIGNAL(clicked(bool)), this, SLOT(slotAccept()));
}


void TeamWindow::slotAccept()
{
    if ((ui.selectionList->count() >= mMinSize) && (ui.selectionList->count() <= mMaxSize))
    {
        accept();
    }
    else if ((ui.selectionList->count() == 0) && GetName().size() > 0)
    {
        accept();
    }
}


void TeamWindow::SetTeam(const Player &p1, const Player &p2)
{
    mSelection.clear();
    mSelection.append(p1);
    mSelection.append(p2);
    Update();
}

void TeamWindow::GetTeam(Team &team)
{
    if (mSelection.size() == 3)
    {
        team.player1Id = mSelection.at(0).id;
        team.player2Id = mSelection.at(1).id;
        team.player3Id = mSelection.at(2).id;
    }
    else if (mSelection.size() == 2)
    {
        team.player1Id = mSelection.at(0).id;
        team.player2Id = mSelection.at(1).id;
    }
    else if (mSelection.size() == 1)
    {
        team.player1Id = mSelection.at(0).id;
        team.player2Id = Player::cDummyPlayer;
    }
    else
    {
        team.player1Id = Player::cDummyPlayer;
        team.player2Id = Player::cDummyPlayer;
    }

    QString name = GetName();

    if (name.size() == 0)
    {
        // No any team name specified, create one
        DbManager::CreateName(team, mSelection.at(0), mSelection.at(1));
    }
    else
    {
        team.teamName = name;
    }
}

void TeamWindow::Initialize(const QList<Player> &players, const QList<int> &inTeams)
{
    // Create a list of players that are still alone
    mList.clear();
    foreach (Player p, players)
    {
        // This player has no team, add it to the list of available players
        if (!inTeams.contains(p.id))
        {
            mList.append(p);
        }
    }

    mSelection.clear();
    Update();
}

void TeamWindow::ClickedRight(int index)
{
    const Player &p = mSelection.at(index);
    // transfer to the left
    mList.append(p);
    mSelection.removeAt(index);

    Update();
}

void TeamWindow::ClickedLeft(int id)
{
    Player p;
    if (Player::Find(mList, id, p) && (mSelection.size() < GetMaxSize()))
    {
        // transfer to the right and remove the player from the list
        mSelection.append(p);
        int index;
        if (Player::Index(mList, id, index))
        {
            mList.removeAt(index);
        }
    }

    Update();
}

void TeamWindow::Update()
{
    StartUpdate(mList.size());

    foreach (Player p, mList)
    {
        QList<QVariant> rowData;
        rowData << p.id << p.name << p.lastName << p.nickName;
        AddLeftEntry(rowData);
    }

    foreach (Player p, mSelection)
    {
        AddRightEntry(p.name + " " + p.lastName);
    }

    FinishUpdate();
}

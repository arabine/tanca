#include "TeamWindow.h"
#include "TableHelper.h"
#include "Log.h"

#include <QMessageBox>


TeamWindow::TeamWindow(QWidget *parent)
    : SelectionWindow(parent, tr("Créer/modifier une équipe"), 2, 2)
    , mTeamsId(0U, 2000000U)
    , mIsEdit(false)
{
    QStringList header;
    header << tr("Id") << tr("Prénom") << tr("Nom") << tr("Pseudo");
    SetHeader(header);

    SetLabelNumber(tr("Équipe N°:"));

    connect(ui.buttonOk, SIGNAL(clicked(bool)), this, SLOT(slotAccept()));
}


void TeamWindow::slotAccept()
{
    bool ok = false;

    if ((ui.selectionList->count() >= mMinSize) && (ui.selectionList->count() <= mMaxSize))
    {
        ok = true;
    }
    else if ((ui.selectionList->count() == 0) && GetName().size() > 0)
    {
        ok = true;
    }
    else
    {
        (void) QMessageBox::warning(this, tr("Tanca"),
                                    tr("Vérifiez les champs."),
                                    QMessageBox::Ok);
    }

    if (!mIsEdit)
    {
        std::uint32_t newId = GetNumber();
        // Accept only if id is valid
        if (!mTeamsId.IsTaken(newId))
        {
            if (ok)
            {
                // if id was changed, release it
                mTeamsId.AddId(newId);
                ok = true;
            }
        }
        else
        {
            (void) QMessageBox::warning(this, tr("Tanca"),
                                        tr("Numéro d'équipe déjà pris."),
                                        QMessageBox::Ok);
        }
    }

    if (ok)
    {
        accept();
    }
}


void TeamWindow::SetTeam(const Player &p1, const Player &p2, const Team &team)
{
    mSelection.clear();
    mSelection.append(p1);
    mSelection.append(p2);

    SetName(team.teamName);
    SetNumber(team.number);
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

void TeamWindow::Initialize(const QList<Player> &players, const QList<int> &inTeams, bool isEdit)
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

    mIsEdit = isEdit;
    if (!isEdit)
    {
        SetNumber(mTeamsId.FindId());
        ui.spinSelectionNumber->setEnabled(true);
    }
    else
    {
        ui.spinSelectionNumber->setEnabled(false);
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

void TeamWindow::ListIds()
{
    mTeamsId.Dump();
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

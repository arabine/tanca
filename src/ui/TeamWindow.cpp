/*=============================================================================
 * Tanca - TeamWindow.cpp
 *=============================================================================
 * Team window management (add/edit)
 *=============================================================================
 * Tanca ( https://github.com/belegar/tanca ) - This file is part of Tanca
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony.rabine@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */


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

    if ((ui.selectionList->count() >= (int)mMinSize) && (ui.selectionList->count() <= (int)mMaxSize))
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
    mSelection.push_back(p1);
    mSelection.push_back(p2);

    SetName(team.teamName.c_str());
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
        team.teamName = name.toStdString();
    }
}

void TeamWindow::Initialize(const std::deque<Player> &players, const std::deque<int> &inTeams, bool isEdit)
{
    // Create a list of players that are still alone
    mList.clear();
    for (auto const & p : players)
    {
        // This player has no team, add it to the list of available players
        //if (!inTeams.contains(p.id))
        if (std::find(inTeams.begin(), inTeams.end(), p.id) == inTeams.end())
        {
            mList.push_back(p);
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
    mList.push_back(p);
    mSelection.erase(mSelection.begin() + index);

    Update();
}

void TeamWindow::ClickedLeft(int id)
{
    Player p;
    if (Player::Find(mList, id, p) && (mSelection.size() < GetMaxSize()))
    {
        // transfer to the right and remove the player from the list
        mSelection.push_back(p);
        int index;
        if (Player::Index(mList, id, index))
        {
            mList.erase(mList.begin() + index);
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
        std::list<Value> rowData = {p.id, p.name, p.lastName, p.nickName};
        AddLeftEntry(rowData);
    }

    foreach (Player p, mSelection)
    {
        AddRightEntry(p.FullName().c_str());
    }

    if (mSelection.size())
    {
         // Reset name when changes are performed
        SetName("");
    }
    else
    {
        SetName(tr("EXEMPTE"));
    }

    FinishUpdate();
}

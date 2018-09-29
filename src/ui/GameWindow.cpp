/*=============================================================================
 * Tanca - GameWindow.cpp
 *=============================================================================
 * Dialog used to add a new game (match between two teams)
 *=============================================================================
 * Tanca ( https://github.com/belegar/tanca ) - This file is part of Tanca
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony.rabine@tarotclub.fr
 *
 * Tanca is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tanca is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tanca.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#include "GameWindow.h"

GameWindow::GameWindow(QWidget *parent)
    : SelectionWindow(parent, tr("Créer un jeu"), 1, 2)
{
    QStringList header;
    header << tr("Id") << tr("N°") << tr("Équipe");
    SetHeader(header);

    SetLabelNumber(tr("Tour :"));

    connect(ui.buttonOk, SIGNAL(clicked(bool)), this, SLOT(slotAccept()));
}

void GameWindow::GetGame(Game &game)
{
    if (mSelection.size() >= 1)
    {
        game.team1Id = mSelection.at(0).id;
        if (mSelection.size() >= 2)
        {
            game.team2Id = mSelection.at(1).id;
        }
        else
        {
            game.team2Id = -1;
        }
    }
}

void GameWindow::Initialize(const std::deque<Team> &teams)
{
    mList = teams;
    mSelection.clear();
    Update();
}


void GameWindow::ClickedRight(int index)
{
    const Team &t = mSelection.at(index);
    // transfer to the left
    mList.push_back(t);
    mSelection.erase(mSelection.begin() + index);

    Update();
}

void GameWindow::ClickedLeft(int id)
{
    Team t;
    if (Team::Find(mList, id, t) && (mSelection.size() < GetMaxSize()))
    {
        // transfer to the right and remove the player from the list
        mSelection.push_back(t);
        int index;
        if (Team::Index(mList, id, index))
        {
            mList.erase(mList.begin() + index);
        }
    }

    Update();
}

void GameWindow::Update()
{
    StartUpdate(mList.size());

    foreach (Team t, mList)
    {
        std::list<Value> rowData = {t.id, t.number, t.teamName};
        AddLeftEntry(rowData);
    }

    foreach (Team t, mSelection)
    {
        AddRightEntry(t.teamName.c_str());
    }

    FinishUpdate();
}

void GameWindow::slotAccept()
{
    if ((ui.selectionList->count() >= (int)mMinSize) && (ui.selectionList->count() <= (int)mMaxSize))
    {
        accept();
    }
}



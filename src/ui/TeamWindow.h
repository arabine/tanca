/*=============================================================================
 * Tanca - TeamWindow.h
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

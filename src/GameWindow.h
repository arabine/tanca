/*=============================================================================
 * Tanca - GameWindow.h
 *=============================================================================
 * Dialog used to add a new game (match between two teams)
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

#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <QDialog>
#include "SelectionWindow.h"
#include "DbManager.h"

class GameWindow : public SelectionWindow
{
    Q_OBJECT

public:
    GameWindow(QWidget *parent);

    void GetGame(Game &game);

    void Initialize(const QList<Team> &teams);

    void ClickedRight(int index);
    void ClickedLeft(int id);

private slots:
    void slotAccept();

private:
    QList<Team> mList;
    QList<Team> mSelection;

    void Update();
};


#endif // GAME_WINDOW_H

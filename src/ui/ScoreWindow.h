/*=============================================================================
 * Tanca - ScoreWindow.h
 *=============================================================================
 * Enter match results between two teams
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


#ifndef SCOREWINDOW_H
#define SCOREWINDOW_H

#include <QDialog>
#include "DbManager.h"
#include "ui_ScoreWindow.h"

class ScoreWindow : public QDialog
{
    Q_OBJECT

public:
    ScoreWindow(QWidget *parent = 0);
    void GetGame(Game &game);
    void SetGame(const Game &game, const Team& team1, const Team& team2);

private:
    Ui::ScoreWindow ui;
};


#endif // SCOREWINDOW_H

//=============================================================================
// End of file ScoreWindow.h
//=============================================================================

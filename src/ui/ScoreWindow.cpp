/*=============================================================================
 * Tanca - ScoreWindow.cpp
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

#include "ScoreWindow.h"

ScoreWindow::ScoreWindow(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    connect(ui.buttonOk, &QPushButton::clicked, this, &ScoreWindow::accept);
    connect(ui.buttonCancel, &QPushButton::clicked, this, &ScoreWindow::reject);
}


void ScoreWindow::SetGame(const Game &game, const Team &team1, const Team &team2)
{
    ui.roundSpinBox->setValue(game.turn + 1);
    ui.team1GroupBox->setTitle(tr("Équipe n° %1").arg(team1.number));
    ui.team2GroupBox->setTitle(tr("Équipe n° %1").arg(team2.number));
    ui.labelTeam1Name->setText(team1.teamName);
    ui.labelTeam2Name->setText(team2.teamName);
    int value = (game.team1Score == -1) ? 0 : game.team1Score;
    ui.spinScore1->setValue(value);
    value = (game.team2Score == -1) ? 0 : game.team2Score;
    ui.spinScore2->setValue(value);
}

void ScoreWindow::GetGame(Game &game)
{
    game.team1Score = ui.spinScore1->value();
    game.team2Score = ui.spinScore2->value();
    game.turn = ui.roundSpinBox->value() - 1;
}


//=============================================================================
// End of file ScoreWindow.h
//=============================================================================

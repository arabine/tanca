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
}


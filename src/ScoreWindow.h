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

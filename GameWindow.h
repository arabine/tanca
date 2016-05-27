#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QDialog>
#include "DbManager.h"
#include "ui_GameWindow.h"

class GameWindow : public QDialog
{
    Q_OBJECT

public:
    GameWindow(QWidget *parent = 0);
    void GetGame(Game &game);
    void SetGame(const Game &game, const Team& team1, const Team& team2);

private:
    Ui::GameWindow ui;
};


#endif // GAMEWINDOW_H

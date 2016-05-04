#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DbManager.h"
#include "PlayerWindow.h"
#include "DatePickerWindow.h"
#include "BracketWindow.h"
#include "TeamWindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void slotAddPlayer();
     void slotShowRounds();
     void slotAddTeam();
     void slotAddMatch();
     void slotSeasonChanged(int index);
     void slotMatchItemActivated();
     void slotEditPlayer();
private:
    void InitializePlayers();

    // Windows
    Ui::MainWindow *ui;
    PlayerWindow *playerWindow;
    BracketWindow *bracketWindow;
    DatePickerWindow *datePickerWindow;
    TeamWindow *teamWindow;

    // other stuff
    DbManager mDatabase;
    QList<int>  mPlayersInTeams; // Players already in teams
    QList<Match> mMatches;
    QList<Team> mTeams;

    void UpdateTeamList(int matchId);
    bool FindPlayer(int id, Player &player);
};

#endif // MAINWINDOW_H

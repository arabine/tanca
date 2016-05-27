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
     void slotShowGames();
     void slotAddTeam();
     void slotAddEvent();
     void slotSeasonChanged(int index);
     void slotMatchItemActivated();
     void slotEditPlayer();
     void slotStartRounds();
     void slotEditGame();
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
    QList<Event> mMatches;
    QList<Team> mTeams;
    Event mCurrentEvent;

    void UpdateTeamList(int eventId);
    bool FindPlayer(int id, Player &player);
    void UpdateGameList();
    bool FindTeam(const int id, Team &team);
};

#endif // MAINWINDOW_H

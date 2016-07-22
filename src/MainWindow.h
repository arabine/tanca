#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DbManager.h"
#include "PlayerWindow.h"
#include "DatePickerWindow.h"
#include "BracketWindow.h"
#include "TeamWindow.h"
#include "GameWindow.h"
#include "EventWindow.h"

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
     void slotEventItemActivated();
     void slotEditPlayer();
     void slotStartRounds();
     void slotEditGame();
     void slotEditEvent();
     void slotRankingSeasonChanged(int index);
     void slotImportFile();
     void slotExport();
     void slotTabChanged(int index);

     void slotDeleteTeam();
     void slotEditTeam();
     void slotDeleteEvent();
private:
    void UpdatePlayersTable();

    // Windows
    Ui::MainWindow *ui;
    PlayerWindow *playerWindow;
    BracketWindow *bracketWindow;
    DatePickerWindow *datePickerWindow;
    TeamWindow *teamWindow;
    GameWindow *gameWindow;
    EventWindow *eventWindow;

    // other stuff
    DbManager mDatabase;
    QList<int>  mPlayersInTeams; // Players already in teams
    QList<Event> mEvents;
    QList<Team> mTeams;
    QList<Game> mGames;
    Event mCurrentEvent;

    void UpdateTeamList(int eventId);
    bool FindPlayer(int id, Player &player);
    void UpdateGameList();

    bool FindGame(const int id, Game &game);
    void UpdateSeasons();
    void UpdateEventsTable();

};

#endif // MAINWINDOW_H

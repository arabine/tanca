#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DbManager.h"
#include "PlayerWindow.h"
#include "DatePickerWindow.h"
#include "BracketWindow.h"
#include "TeamWindow.h"
#include "GameWindow.h"
#include "ScoreWindow.h"
#include "EventWindow.h"
#include "UniqueId.h"

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
     void slotEditPlayer();
     void slotDeletePlayer();

     void slotAddGame();
     void slotShowGames();
     void slotEditGame();
     void slotDeleteGame();

     void slotAddTeam();
     void slotEditTeam();
     void slotDeleteTeam();

     void slotAddEvent();
     void slotDeleteEvent();
     void slotEditEvent();

     void slotSeasonChanged(int index);
     void slotEventItemActivated();

     void slotRandomizeGames();


     void slotImportFile();
     void slotExportRanking();
     void slotTabChanged(int index);
     void slotExportPlayers();
private:
    void UpdatePlayersTable();

    // Windows
    Ui::MainWindow *ui;
    PlayerWindow *playerWindow;
    BracketWindow *bracketWindow;
    DatePickerWindow *datePickerWindow;
    TeamWindow *teamWindow;
    GameWindow *gameWindow;
    ScoreWindow *scoreWindow;
    EventWindow *eventWindow;

    // other stuff
    DbManager mDatabase;
    QList<int>  mPlayersInTeams; // Players already in teams
    QList<Event> mEvents;
    QList<Team> mTeams;
    QList<Game> mGames;
    Event mCurrentEvent;
    UniqueId mTeamsId;

    void UpdateTeamList(int eventId);
    bool FindPlayer(int id, Player &player);
    void UpdateGameList();
    void UpdateRanking(int index);
    bool FindGame(const int id, Game &game);
    void UpdateSeasons();
    void UpdateEventsTable();
    void ExportTable(QTableWidget *table, const QString &title);
};

#endif // MAINWINDOW_H

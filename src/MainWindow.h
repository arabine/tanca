/*=============================================================================
 * Tanca - MainWindow.h
 *=============================================================================
 * Main window widget
 *=============================================================================
 * Tanca ( https://github.com/belegar/tanca ) - This file is part of Tanca
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony.rabine@tarotclub.fr
 *
 * Tanca is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tanca is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tanca.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DbManager.h"
#include "PlayerWindow.h"
#include "DatePickerWindow.h"
#include "TeamWindow.h"
#include "GameWindow.h"
#include "ScoreWindow.h"
#include "EventWindow.h"
#include "ui_AboutWindow.h"
#include "Tournament.h"
#include "Server.h"

namespace Ui {
class MainWindow;
}

std::string StateToString(const Event &event);

std::string TypeToString(const Event &event);


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void Initialize();

private slots:
    void slotAddPlayer();
    void slotEditPlayer();
    void slotDeletePlayer();

    void slotAddGame();
    void slotEditGame();
    void slotDeleteGame();

    void slotAddTeam();
    void slotEditTeam();
    void slotDeleteTeam();

    void slotAddReward();
    void slotDeleteReward();

    void slotAddEvent();
    void slotDeleteEvent();
    void slotEditEvent();

    void slotSeasonChanged(int index);
    void slotEventItemActivated();

    void slotGenerateGames();
    void slotAboutBox();

    void slotImportPlayerFile();
    void slotExportRanking();
    void slotExportGames();
    void slotTabChanged(int index);
    void slotExportPlayers();
    void slotExportTeams();
    void slotRankingOptionChanged(bool checked);
    void slotRankingLeft();
    void slotRankingRight();
    void slotDeleteAllGames();
    void slotTeamItemActivated();
    void slotFilterPlayer();
private:
    void UpdatePlayersTable();

    // Windows
    Ui::MainWindow *ui;
    Ui::AboutWindow uiAboutBox;
    PlayerWindow *playerWindow;
    DatePickerWindow *datePickerWindow;
    TeamWindow *teamWindow;
    GameWindow *gameWindow;
    ScoreWindow *scoreWindow;
    EventWindow *eventWindow;

    // other stuff
    DbManager mDatabase;
    std::deque<int>  mPlayersInTeams; // Players already in teams
    std::deque<Event> mEvents;
    std::deque<Team> mTeams;
    std::deque<Game> mGames;
    Event mCurrentEvent;
    Tournament mTournament;
    int mCurrentRankingRound;
    int mSelectedTeam;
    Server mServer;

    void UpdateTeamList();
    bool FindPlayer(int id, Player &player);
    void UpdateGameList();
    void UpdateRanking();
    bool FindGame(const int id, Game &game);
    void UpdateSeasons();
    void UpdateEventsTable();
    void ExportTable(QTableWidget *table, const QString &title);
    void UpdateBrackets();
    void UpdateRewards();
};

#endif // MAINWINDOW_H

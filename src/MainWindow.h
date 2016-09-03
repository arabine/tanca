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
#include "BracketWindow.h"
#include "TeamWindow.h"
#include "GameWindow.h"
#include "ScoreWindow.h"
#include "EventWindow.h"
#include "UniqueId.h"
#include "ui_AboutWindow.h"

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
    void slotAboutBox();

    void slotImportFile();
    void slotExportRanking();
    void slotExportGames();
    void slotTabChanged(int index);
    void slotExportPlayers();
    void slotExportTeams();
    void slotRankingOptionChanged(bool checked);
    void slotDeleteAllGames();
private:
    void UpdatePlayersTable();

    // Windows
    Ui::MainWindow *ui;
    Ui::AboutWindow uiAboutBox;
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

    void UpdateTeamList();
    bool FindPlayer(int id, Player &player);
    void UpdateGameList();
    void UpdateRanking();
    bool FindGame(const int id, Game &game);
    void UpdateSeasons();
    void UpdateEventsTable();
    void ExportTable(QTableWidget *table, const QString &title);
};

#endif // MAINWINDOW_H

/*=============================================================================
 * Tanca - MainWindow.cpp
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

#include <QStandardPaths>
#include <iostream>
#include <QMessageBox>
#include <QFileDialog>
#include <QtQuick>

#include "Log.h"
#include "MainWindow.h"
#include "TableHelper.h"
#include "ui_MainWindow.h"
#include "Brackets.h"

static const QString gVersion = "1.7";

// Table headers
QStringList gGamesTableHeader;
QStringList gEventsTableHeader;
QStringList gPlayersTableHeader;
QStringList gTeamsTableHeader;

#ifdef USE_WINDOWS_OS
QString gAppDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/tanca";
#else
QString gAppDataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.tanca";
#endif
QString gDbFullPath = gAppDataPath + "/tanca.db";

class Outputter : public Observer<std::string>
{
public:
    virtual void Update(const std::string &info)
    {
        std::cout << info << std::endl;
    }
};

Outputter consoleOutput;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mDatabase(gDbFullPath)
    , mTeamsId(0U, 2000000U)
{
    Log::SetLogPath(gAppDataPath.toStdString());
    Log::RegisterListener(consoleOutput);

    std::cout << "Application path set to: " << gAppDataPath.toStdString() << std::endl;

    // Setup UI
    ui->setupUi(this);

    playerWindow = new PlayerWindow(this);
    playerWindow->hide();

    datePickerWindow = new DatePickerWindow(this);
    datePickerWindow->hide();

    teamWindow = new TeamWindow(this);
    teamWindow->hide();

    gameWindow = new GameWindow(this);
    gameWindow->hide();

    scoreWindow = new ScoreWindow(this);
    scoreWindow->hide();

    eventWindow = new EventWindow(this);
    eventWindow->hide();

    setWindowTitle("Tanca " + gVersion + " - Logiciel de gestion de club et de concours de pétanque.");

    // Setup signals for the menu
    connect(ui->actionImporter, &QAction::triggered, this, &MainWindow::slotImportFile);
    connect(ui->actionQuitter, &QAction::triggered, this, &QCoreApplication::quit);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::slotAboutBox);

    // Setup signals for TAB 1: players management
    connect(ui->buttonAddPlayer, &QPushButton::clicked, this, &MainWindow::slotAddPlayer);
    connect(ui->buttonEditPlayer, &QPushButton::clicked, this, &MainWindow::slotEditPlayer);
    connect(ui->buttonDeletePlayer, &QPushButton::clicked, this, &MainWindow::slotDeletePlayer);
    connect(ui->buttonExportPlayers, &QPushButton::clicked, this, &MainWindow::slotExportPlayers);

    // Setup signals for TAB 3: club championship management
    connect(ui->buttonAddEvent, &QPushButton::clicked, this, &MainWindow::slotAddEvent);
    connect(ui->buttonEditEvent, &QPushButton::clicked, this, &MainWindow::slotEditEvent);
    connect(ui->buttonDeleteEvent, &QPushButton::clicked, this, &MainWindow::slotDeleteEvent);

    connect(ui->buttonAddTeam, &QPushButton::clicked, this, &MainWindow::slotAddTeam);
    connect(ui->buttonEditTeam, &QPushButton::clicked, this, &MainWindow::slotEditTeam);
    connect(ui->buttonDeleteTeam, &QPushButton::clicked, this, &MainWindow::slotDeleteTeam);
    connect(ui->buttonExportTeams, &QPushButton::clicked, this, &MainWindow::slotExportTeams);

    connect(ui->eventTable, SIGNAL(itemSelectionChanged()), this, SLOT(slotEventItemActivated()));
    connect(ui->comboSeasons, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSeasonChanged(int)));
    connect(ui->buttonCreateGames, &QPushButton::clicked, this, &MainWindow::slotGenerateGames);

    connect(ui->buttonAddGame, &QPushButton::clicked, this, &MainWindow::slotAddGame);
    connect(ui->buttonEditGame, &QPushButton::clicked, this, &MainWindow::slotEditGame);
    connect(ui->buttonDeleteGame, &QPushButton::clicked, this, &MainWindow::slotDeleteGame);
    connect(ui->buttonDeleteAllGames, &QPushButton::clicked, this, &MainWindow::slotDeleteAllGames);
    connect(ui->buttonExportGames, &QPushButton::clicked, this, &MainWindow::slotExportGames);

    connect(ui->tabWidget_2, SIGNAL(currentChanged(int)), this, SLOT(slotTabChanged(int)));
    connect(ui->radioEvent, &QRadioButton::toggled, this, &MainWindow::slotRankingOptionChanged);
    connect(ui->radioSeason, &QRadioButton::toggled, this, &MainWindow::slotRankingOptionChanged);
    connect(ui->buttonExportRanking, &QPushButton::clicked, this, &MainWindow::slotExportRanking);

    // Setup other stuff
    mDatabase.Initialize();
    gGamesTableHeader << tr("Id") << tr("Partie") << tr("Équipe 1") << tr("Équipe 2") << tr("Score 1") << tr("Score 2");
    gEventsTableHeader << tr("Id") << tr("Date") << tr("Type") << tr("Titre") << tr("État");
    gPlayersTableHeader << tr("Id") << tr("UUID") << tr("Prénom") << tr("Nom") << tr("Pseudonyme") << tr("E-mail") << tr("Téléphone (mobile)") << tr("Téléphone (maison)") << tr("Date de naissance") << tr("Rue") << tr("Code postal") << tr("Ville") << tr("Licences") << tr("Commentaires") << tr("Statut") << tr("Divers");
    gTeamsTableHeader << tr("Id") << tr("Numéro") << tr("Joueur 1") << tr("Joueur 2") << ("Nom de l'équipe");


}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::Initialize()
{
    // Initialize views
    UpdatePlayersTable();
    UpdateSeasons();
}

void MainWindow::UpdatePlayersTable()
{
    TableHelper helper(ui->playersWidget);
    QList<Player> &list = mDatabase.GetPlayerList();
    helper.Initialize(gPlayersTableHeader, list.size());

    foreach (Player p, list)
    {
        QList<QVariant> rowData;

        rowData << p.id << p.uuid << p.name << p.lastName << p.nickName << p.email
                << p.mobilePhone << p.homePhone << p.birthDate.toString(Qt::TextDate) << p.road << p.postCode
                << p.city << p.membership << p.comments << p.state << p.document;

        helper.AppendLine(rowData, false);
    }

    ui->playersWidget->hideColumn(1); // don't show the UUID
    ui->playersWidget->hideColumn(14); // don't show the State
    ui->playersWidget->hideColumn(15); // don't show the Document

    helper.Finish();
}

void MainWindow::slotImportFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Ouvrir un fichier CSV"), QStandardPaths::displayName(QStandardPaths::DocumentsLocation), tr("Fichier CSV (*.csv)"));

    QFile file(fileName);

    if (file.exists())
    {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            TLogError("Cannot open CSV file: " + fileName.toStdString());
        }
        else
        {
            while (!file.atEnd())
            {
                QString line = file.readLine();
                QStringList exploded = line.split(";");

                // We need at least 5 information for a player
                if (exploded.size() >= 5)
                {
                    // Add this new player, if not already exist in the database
                    Player p;

                    p.lastName = exploded.at(0);
                    p.name = exploded.at(1);
                    p.road = exploded.at(2);
                    p.email = exploded.at(3);
                    p.mobilePhone = exploded.at(4);

                    QString fullname = p.name + " " + p.lastName;

                    if (!mDatabase.PlayerExists(p) && mDatabase.IsValid(p))
                    {
                        if (mDatabase.AddPlayer(p))
                        {
                            std::cout << "Imported player: " << fullname.toStdString() << std::endl;
                        }
                        else
                        {
                            TLogError("Import failed for player: " + fullname.toStdString());
                        }
                    }
                    else
                    {
                        TLogError("Player " + fullname.toStdString() + " is invalid or already exists in the database, cannot import it");
                    }
                }
                else
                {
                    TLogError("Bad CSV file format");
                }
            }
        }
    }
    else
    {
        TLogError("Cannot find CSV file: " + fileName.toStdString());
    }
}


void MainWindow::slotAddPlayer()
{
    Player newPlayer;
    playerWindow->SetPlayer(newPlayer);
    if (playerWindow->exec() == QDialog::Accepted)
    {
        playerWindow->GetPlayer(newPlayer);
        if (mDatabase.AddPlayer(newPlayer))
        {
            UpdatePlayersTable();
        }
        else
        {
            TLogError("Cannot add player!");
        }
    }
}

void MainWindow::slotEditPlayer()
{
    TableHelper helper(ui->playersWidget);

    int id;
    if (helper.GetFirstColumnValue(id))
    {
        Player p;
        if (mDatabase.FindPlayer(id, p))
        {
            playerWindow->SetPlayer(p);
            if (playerWindow->exec() == QDialog::Accepted)
            {
                playerWindow->GetPlayer(p);
                if (mDatabase.EditPlayer(p))
                {
                    UpdatePlayersTable();
                }
                else
                {
                    TLogError("Cannot edit player!");
                }
            }
        }
    }
}

void MainWindow::slotDeletePlayer()
{

    TableHelper helper(ui->playersWidget);

    int id;
    if (helper.GetFirstColumnValue(id))
    {
        Player p;
        if (mDatabase.FindPlayer(id, p))
        {
            bool canDelete = true;
            // We allow deleting a player if there is no any game finished for him
            QList<Team> teams = mDatabase.GetTeamsByPlayerId(id);

            if (teams.size() > 0)
            {
                // Check if the player has played some games
                foreach (Team team, teams)
                {
                    Event event = mDatabase.GetEvent(team.eventId);

                    if (event.IsValid())
                    {
                        canDelete = false;
                    }
                }
            }

            if (canDelete)
            {
                // Granted to delete, actually do it!
                if (mDatabase.DeletePlayer(id))
                {
                    UpdatePlayersTable();
                    (void)QMessageBox::information(this, tr("Suppression d'un joueur"),
                                        tr("Suppression du joueur réussie."),
                                        QMessageBox::Ok);
                }
                else
                {
                    (void)QMessageBox::critical(this, tr("Suppression d'un joueur"),
                                        tr("La tentative de suppression a échoué."),
                                        QMessageBox::Ok);
                }
            }
            else
            {
                (void)QMessageBox::critical(this, tr("Suppression d'un joueur"),
                                        tr("Impossible de supprimer un joueur ayant participé à un événement.\n"
                                           "Supprimez l'événement d'abord."),
                                        QMessageBox::Ok);
            }
        }
    }
}


void MainWindow::ExportTable(QTableWidget *table, const QString &title)
{
    QString fileName = QFileDialog::getSaveFileName(this, title,
                                 QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                 tr("Excel CSV (*.csv)"));
    if (!fileName.isEmpty())
    {
        TableHelper helper(table);
        helper.Export(fileName);
    }
}

void MainWindow::slotExportPlayers()
{
    ExportTable(ui->playersWidget, tr("Exporter la base de joueurs au format Excel (CSV)"));
}

void MainWindow::slotExportTeams()
{
     ExportTable(ui->teamTable, tr("Exporter la liste des équipes au format Excel (CSV)"));
}

void MainWindow::slotRankingOptionChanged(bool checked)
{
    Q_UNUSED(checked);
    UpdateRanking();
}

void MainWindow::UpdateTeamList()
{
    mTeams = mDatabase.GetTeams(mCurrentEvent.id);
    mPlayersInTeams.clear();

    TableHelper helper(ui->teamTable);
    helper.Initialize(gTeamsTableHeader, mTeams.size());
    mTeamsId.Clear();

    foreach (Team team, mTeams)
    {
        Player p1, p2, p3;
        bool valid = mDatabase.FindPlayer(team.player1Id, p1);
        valid = valid && mDatabase.FindPlayer(team.player2Id, p2);
        (void) mDatabase.FindPlayer(team.player3Id, p3); // Player 3 is optional

        if (valid)
        {
            mPlayersInTeams.append(team.player1Id);
            mPlayersInTeams.append(team.player2Id);

            mTeamsId.AddId(team.number);

            QList<QVariant> rowData;
            rowData << team.id << team.number << p1.FullName() << p2.FullName() << team.teamName;
            helper.AppendLine(rowData, false);
        }
    }

    helper.Finish();

    // Cannot edit teams if some games exist
    if (mGames.size() == 0)
    {
        ui->buttonEditTeam->setEnabled(true);
        ui->buttonDeleteTeam->setEnabled(true);
    }
    else
    {
        ui->buttonEditTeam->setEnabled(false);
        ui->buttonDeleteTeam->setEnabled(false);
    }
}

void MainWindow::slotEventItemActivated()
{
    int row = ui->eventTable->currentRow();
    if (row > -1)
    {
        int id;
        TableHelper helper(ui->eventTable);

        if (helper.GetFirstColumnValue(id))
        {
            mCurrentEvent = mDatabase.GetEvent(id);

            if (mCurrentEvent.IsValid())
            {
                std::cout << "Current event id: " << mCurrentEvent.id << std::endl;
                UpdateTeamList();
                UpdateGameList();
                UpdateRanking();
            }
            else
            {
                TLogError("Invalid event!");
            }
        }
    }
}

void MainWindow::UpdateSeasons()
{
    QStringList seasons = mDatabase.GetSeasons();

    ui->comboSeasons->clear();
    ui->comboSeasons->addItems(seasons);

    UpdateEventsTable();
}

void MainWindow::slotAddEvent()
{
    Event event;
    eventWindow->SetEvent(event);
    if (eventWindow->exec() == QDialog::Accepted)
    {
        eventWindow->GetEvent(event);
        event.year = event.date.date().year();
        if (mDatabase.AddEvent(event))
        {
            UpdateSeasons();
        }
    }
}

void MainWindow::slotEditEvent()
{
    TableHelper helper(ui->eventTable);

    int id;
    if (helper.GetFirstColumnValue(id))
    {
        eventWindow->SetEvent(mCurrentEvent);
        if (eventWindow->exec() == QDialog::Accepted)
        {
            eventWindow->GetEvent(mCurrentEvent);
            if (!mDatabase.EditEvent(mCurrentEvent))
            {
                TLogError("Cannot edit event!");
            }
            else
            {
                slotSeasonChanged(ui->comboSeasons->currentIndex());
            }
        }
    }
}

void MainWindow::slotDeleteEvent()
{
    TableHelper helper(ui->eventTable);

    int id;
    if (helper.GetFirstColumnValue(id))
    {
        if (QMessageBox::warning(this, tr("Suppression d'un événement"),
                                    tr("Attention ! Toutes les parties associées seront perdues. Continuer ?"),
                                    QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
        {
            bool success = mDatabase.DeleteGameByEventId(id);
            success = success && mDatabase.DeleteTeamByEventId(id);
            success = success && mDatabase.DeleteEvent(id);

            if (!success)
            {
                TLogError("Delete event failure");
            }
            UpdateSeasons();
        }
    }
}


void MainWindow::slotAddTeam()
{
    int selection = ui->eventTable->currentRow();
    if (selection > -1)
    {
        // Prepare widget contents
        std::uint32_t id = mTeamsId.TakeId();
        teamWindow->Initialize(mDatabase.GetPlayerList(), mPlayersInTeams);
        teamWindow->SetNumber(id);

        if (teamWindow->exec() == QDialog::Accepted)
        {
            Team team;
            teamWindow->GetTeam(team);
            team.eventId = mCurrentEvent.id;
            team.number = teamWindow->GetNumber();
            if (mDatabase.AddTeam(team))
            {
                UpdateTeamList();
            }
        }
        else
        {
            // release the unused id
            mTeamsId.ReleaseId(id);
        }
    }
}

void MainWindow::slotEditTeam()
{
    if (mGames.size() == 0)
    {
        TableHelper helper(ui->teamTable);

        int id;
        if (helper.GetFirstColumnValue(id))
        {
            Team team;
            if (Team::Find(mTeams, id, team))
            {
                // Prepare widget contents
                teamWindow->Initialize(mDatabase.GetPlayerList(), mPlayersInTeams);

                Player p1, p2;
                bool found = mDatabase.FindPlayer(team.player1Id, p1);
                found = found && mDatabase.FindPlayer(team.player2Id, p2);

                if (found)
                {
                    teamWindow->SetTeam(p1, p2);
                    teamWindow->SetNumber(team.number);

                    if (teamWindow->exec() == QDialog::Accepted)
                    {
                        teamWindow->GetTeam(team);
                        team.number = teamWindow->GetNumber();
                        if (mDatabase.EditTeam(team))
                        {
                            UpdateTeamList();
                        }
                    }
                }
                else
                {
                    TLogError("Cannot find players");
                }
            }
        }
    }
    else
    {
        (void) QMessageBox::warning(this, tr("Tanca"),
                                    tr("Impossible d'éditer une équipe s'il existe des rencontres."),
                                    QMessageBox::Ok);
    }
}

void MainWindow::slotDeleteTeam()
{
    if (mGames.size() == 0)
    {
        TableHelper helper(ui->teamTable);

        int id;
        if (helper.GetFirstColumnValue(id))
        {
            if (mDatabase.DeleteTeam(id))
            {
                UpdateTeamList();
            }
        }
    }
    else
    {
        (void) QMessageBox::warning(this, tr("Tanca"),
                                    tr("Impossible de supprimer une équipe, des rencontres existent."),
                                    QMessageBox::Ok);
    }
}

void MainWindow::UpdateEventsTable()
{
    TableHelper helper(ui->eventTable);
    helper.Initialize(gEventsTableHeader, mEvents.size());

    foreach (Event event, mEvents)
    {
        QList<QVariant> rowData;
        rowData << event.id << event.date.toString("d MMMM yyyy") << event.TypeToString() << event.title << event.StateToString();
        helper.AppendLine(rowData, false);
    }

    helper.Finish();

    if (mEvents.size() > 0)
    {
        // Will refresh all the UI elements for that event
        ui->eventTable->selectRow(mEvents.size() - 1);
    }
    else
    {
        // Empty teams and games
        ui->teamTable->clear();
        ui->gameTable->clear();
    }
}


void MainWindow::slotSeasonChanged(int index)
{
    mEvents = mDatabase.GetEvents(ui->comboSeasons->itemText(index).toInt());
    UpdateEventsTable();
}

void MainWindow::slotGenerateGames()
{
    QList<Game> games;
    QString error;

    if (mGames.size()%2)
    {
        (void) QMessageBox::warning(this, tr("Tanca"),
                                    tr("Nombre impair d'équipes, ajoutez une équipe fictive"),
                                    QMessageBox::Ok);
    }
    else
    {

        if (mCurrentEvent.type == Event::cRoundRobin)
        {
            int rounds = ui->spinNbRounds->value();
            error = mTournament.BuildRoundRobinRounds(mTeams, rounds, games);
        }
        else
        {
            // Swiss algorithm
            error = mTournament.BuildSwissRounds(mGames, mTeams, games);
        }

        if (games.size() > 0)
        {
            mCurrentEvent.state = Event::cStarted;
            mDatabase.UpdateEventState(mCurrentEvent);

            if (!mDatabase.AddGames(games))
            {
                TLogError("Cannot store rounds!");
            }

            UpdateGameList();
        }
        else
        {
            TLogError("Cannot build rounds!");
            (void) QMessageBox::warning(this, tr("Tanca"),
                                        tr("Impossible de générer les parties : ") + error,
                                        QMessageBox::Ok);
        }
    }
}

void MainWindow::slotAboutBox()
{
    QDialog about;
    uiAboutBox.setupUi(&about);
    about.exec();
}

bool MainWindow::FindGame(const int id, Game &game)
{
    bool found = false;
    for (int i = 0; i < mGames.size(); i++)
    {
        if (mGames[i].id == id)
        {
            found = true;
            game = mGames[i];
            break;
        }
    }

    return found;
}


void MainWindow::UpdateGameList()
{
    mGames = mDatabase.GetGamesByEventId(mCurrentEvent.id);

    QString json = mTournament.ToJsonString(mGames, mTeams);

    Brackets *item = ui->quickWidget->rootObject()->findChild<Brackets*>("brackets");
    if (item)
    {
        item->setProperty("rounds", json);
    }
/*
    QVariant returnedValue;
    QVariant t1, t2;
    QMetaObject::invokeMethod(ui->quickWidget->rootObject(), "drawGame",
            Q_RETURN_ARG(QVariant, returnedValue),
            Q_ARG(QVariant, 0),
            Q_ARG(QVariant, 0),
            Q_ARG(QVariant, t1),
            Q_ARG(QVariant, t2));
*/
    TableHelper helper(ui->gameTable);
    helper.Initialize(gGamesTableHeader, mGames.size());

    foreach (Game game, mGames)
    {
        Team t1, t2;
        QList<QVariant> gameData;

        // Be tolerant: only print found teams (forget return code)
        (void) Team::Find(mTeams, game.team1Id, t1);
        (void) Team::Find(mTeams, game.team2Id, t2);

        gameData << game.id << (int)(game.turn + 1) << t1.teamName << t2.teamName << game.team1Score << game.team2Score;
        helper.AppendLine(gameData, game.IsPlayed());

    }

    helper.Finish();
    ui->gameTable->sortByColumn(1, Qt::AscendingOrder);
}

void MainWindow::slotAddGame()
{
    // Prepare widget contents
    gameWindow->Initialize(mTeams);
    gameWindow->AllowZeroNumber(false);

    if (gameWindow->exec() == QDialog::Accepted)
    {
        Game game;
        gameWindow->GetGame(game);
        game.eventId = mCurrentEvent.id;
        game.turn = gameWindow->GetNumber() - 1; // turns begin internally @ zero

        QList<Game> list;
        list.append(game);
        if (mDatabase.AddGames(list))
        {
            if (mCurrentEvent.state != Event::cStarted)
            {
                mCurrentEvent.state = Event::cStarted;
                mDatabase.UpdateEventState(mCurrentEvent);
            }
            UpdateGameList();
        }
    }
}

void MainWindow::slotEditGame()
{
    QModelIndexList indexes = ui->gameTable->selectionModel()->selection().indexes();

    if (indexes.size() > 1)
    {
        QModelIndex index = indexes.at(0);
        QMap<int, QVariant> data = ui->gameTable->model()->itemData(index);

        if (data.contains(0))
        {
            int id = data[0].toInt();
            std::cout << "Found team id: " << id << std::endl;
            Game game;

            if (FindGame(id, game))
            {
                Team team1;
                Team team2;

                // Be tolerant if teams are not found
                (void) Team::Find(mTeams, game.team1Id, team1);
                (void) Team::Find(mTeams, game.team2Id, team2);

                scoreWindow->SetGame(game, team1, team2);
                if (scoreWindow->exec() == QDialog::Accepted)
                {
                    scoreWindow->GetGame(game);
                    if (!mDatabase.EditGame(game))
                    {
                        TLogError("Cannot edit game!");
                    }
                    else
                    {
                        UpdateGameList();
                    }
                }
            }
        }
        else
        {
            TLogError("Cannot find any valid game in the list");
        }
    }
}


void MainWindow::slotDeleteGame()
{
    TableHelper helper(ui->gameTable);

    int id;
    if (helper.GetFirstColumnValue(id))
    {
        if (QMessageBox::warning(this, tr("Suppression d'une rencontre"),
                                    tr("Attention ! Tous les points associées seront perdus. Continuer ?"),
                                    QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
        {
            if (!mDatabase.DeleteGame(id))
            {
                TLogError("Delete game failure");
            }
            UpdateGameList();
        }
    }
}

void MainWindow::slotDeleteAllGames()
{
    if (QMessageBox::warning(this, tr("Suppression de toutes les rencontres"),
                                tr("Attention ! Tous les points associées seront perdus. Continuer ?"),
                                QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
    {


        for( int r = 0; r < ui->gameTable->rowCount(); ++r )
        {
            int id = ui->gameTable->item(r, 0)->text().toInt();

            if (!mDatabase.DeleteGame(id))
            {
                TLogError("Delete game failure");
            }
        }

        UpdateGameList();
    }
}


void MainWindow::slotTabChanged(int index)
{
    Q_UNUSED(index);
    // Refresh ranking
    UpdateRanking();
    // Refresh team buttons

    // Cannot edit teams if some games exist
    if (mGames.size() == 0)
    {
        ui->buttonEditTeam->setEnabled(true);
        ui->buttonDeleteTeam->setEnabled(true);
    }
    else
    {
        ui->buttonEditTeam->setEnabled(false);
        ui->buttonDeleteTeam->setEnabled(false);
    }

    ui->quickWidget->update();
}

void MainWindow::slotExportRanking()
{
    ExportTable(ui->tableContest, tr("Exporter le classement au format Excel (CSV)"));
}

void MainWindow::slotExportGames()
{
    ExportTable(ui->gameTable, tr("Exporter la liste des parties au format Excel (CSV)"));
}

void MainWindow::UpdateRanking()
{
    /*
    bool isSeason = ui->radioSeason->isChecked(); // Display option


    ranking.Show(ui->tableContest, mDatabase.GetPlayerList(), mTeams, isSeason);
    ui->tableContest->sortByColumn(4, Qt::DescendingOrder);
    */
}


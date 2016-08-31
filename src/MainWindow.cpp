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
#include "Log.h"
#include "MainWindow.h"
#include "TableHelper.h"
#include "ui_MainWindow.h"

static const QString gVersion = "1.5";

// Table headers
QStringList gGamesTableHeader;
QStringList gEventsTableHeader;
QStringList gPlayersTableHeader;
QStringList gSeasonRankingTableHeader;
QStringList gTeamsTableHeader;
QStringList gEventRankingTableHeader;

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

    bracketWindow = new BracketWindow(this);
    bracketWindow->hide();

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

    connect(ui->eventTable, SIGNAL(itemSelectionChanged()), this, SLOT(slotEventItemActivated()));
    connect(ui->buttonShowRounds, &QPushButton::clicked, this, &MainWindow::slotShowGames);
    connect(ui->comboSeasons, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSeasonChanged(int)));
    connect(ui->buttonRandomizeGames, &QPushButton::clicked, this, &MainWindow::slotRandomizeGames);

    connect(ui->buttonAddGame, &QPushButton::clicked, this, &MainWindow::slotAddGame);
    connect(ui->buttonEditGame, &QPushButton::clicked, this, &MainWindow::slotEditGame);
    connect(ui->buttonDeleteGame, &QPushButton::clicked, this, &MainWindow::slotDeleteGame);
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
    gSeasonRankingTableHeader << tr("Id") << tr("Joueur") << tr("Points") << tr("Parties jouées");
    gEventRankingTableHeader << tr("Id") << tr("Équipe") << tr("Points") << tr("Parties jouées");
    gTeamsTableHeader << tr("Id") << tr("Joueur 1") << tr("Joueur 2") << ("Nom de l'équipe");

    // Initialize views
    UpdatePlayersTable();
    UpdateSeasons();
}

MainWindow::~MainWindow()
{
    delete ui;
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
        QFile f( fileName );
        if (f.open(QFile::WriteOnly))
        {
            QTextStream data( &f );
            QStringList strList;

            // Export header title
            for( int c = 0; c < table->columnCount(); ++c )
            {
                strList << table->horizontalHeaderItem(c)->data(Qt::DisplayRole).toString();
            }

            data << strList.join(";") << "\n";

            // Export table contents
            for( int r = 0; r < table->rowCount(); ++r )
            {
                strList.clear();
                for( int c = 0; c < table->columnCount(); ++c )
                {
                    strList << table->item( r, c )->text();
                }
                data << strList.join( ";" ) + "\n";
            }
            f.close();
        }
    }
}

void MainWindow::slotExportPlayers()
{
    ExportTable(ui->playersWidget, tr("Exporter la base de joueurs au format Excel (CSV)"));
}

void MainWindow::slotRankingOptionChanged(bool checked)
{
    Q_UNUSED(checked);
    UpdateRanking();
}

void MainWindow::UpdateTeamList(int eventId)
{
    mTeams = mDatabase.GetTeams(eventId);
    mPlayersInTeams.clear();

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
            rowData << team.id << p1.FullName() << p2.FullName() << team.teamName;
            helper.AppendLine(rowData, false);
        }
    }

    helper.Finish();
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
                UpdateTeamList(mCurrentEvent.id);
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
                UpdateTeamList(mCurrentEvent.id);
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
                            UpdateTeamList(mCurrentEvent.id);
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
                UpdateTeamList(mCurrentEvent.id);
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

void MainWindow::slotRandomizeGames()
{
    if (mGames.size() == 0)
    {
        QList<Game> games = bracketWindow->BuildRounds(mTeams, ui->spinNbRounds->value());

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
        }
    }
    else
    {
        (void) QMessageBox::warning(this, tr("Tanca"),
                                    tr("Des rencontres existent déjà.\n"
                                       "Videz la liste en supprimant les rencontres."),
                                    QMessageBox::Ok);
    }
}

void MainWindow::slotAboutBox()
{
    QDialog about;
    uiAboutBox.setupUi(&about);
    about.exec();
}

void MainWindow::slotShowGames()
{
    UpdateGameList();
    bracketWindow->setWindowModality(Qt::NonModal);
    bracketWindow->show();

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
    mGames = mDatabase.GetGames(mCurrentEvent.id);
    bracketWindow->SetGames(mGames, mTeams);

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


void MainWindow::slotTabChanged(int index)
{
    Q_UNUSED(index);
    // Refresh ranking
    UpdateRanking();
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
    struct Rank
    {
        int points;
        int playedGames;
        Rank()
            : points(0)
            , playedGames(0)
        {

        }
    };

    class Ranking
    {
    public:

        void Add(int playerId, int score)
        {
            if (mList.contains(playerId))
            {
                score += mList[playerId].points;
            }
            mList[playerId].points = score;
            mList[playerId].playedGames++;
        }

        void Show(QTableWidget *table, const QList<Player> &players, const QList<Team> &teams, bool isSeason)
        {
            TableHelper helper(table);

            if (isSeason)
            {
                helper.Initialize(gSeasonRankingTableHeader, mList.size());
            }
            else
            {
                helper.Initialize(gEventRankingTableHeader, mList.size());
            }

            QMapIterator<int, Rank> i(mList);
            while (i.hasNext())
            {
                i.next();

                // Fill the line
                Rank rank = i.value();
                int id = i.key();

                if (isSeason)
                {
                    // Show the whole season ranking
                    Player player;
                    if (Player::Find(players, id, player))
                    {
                        QList<QVariant> rowData;
                        rowData << player.id << player.FullName() << rank.points << rank.playedGames;
                        helper.AppendLine(rowData, false);
                    }
                    else
                    {
                        TLogError("Cannot find player to create ranking!");
                    }
                }
                else
                {
                    // Show the event result
                    Team team;
                    if (Team::Find(teams, id, team))
                    {
                        QList<QVariant> rowData;
                        rowData << team.id << team.teamName << rank.points << rank.playedGames;
                        helper.AppendLine(rowData, false);
                    }
                    else
                    {
                        TLogError("Cannot find team to create ranking!");
                    }
                }
            }

            helper.Finish();
        }

    private:
        QMap<int, Rank> mList;
    };

    Ranking ranking; // player id, Rank
    bool isSeason = ui->radioSeason->isChecked(); // Display option

    foreach (Event event, mEvents)
    {
        if ((event.state == Event::cStarted) && (event.type == Event::cClubContest))
        {
            bool ok = true;
            if (!isSeason)
            {
                // We want to show only the current event ranking
                if (event.id != mCurrentEvent.id)
                {
                    ok = false;
                }
            }

            if (ok)
            {
                // Search for every game played for this event
                QList<Game> games = mDatabase.GetGames(event.id);
                QList<Team> teams = mDatabase.GetTeams(event.id);

                foreach (Game game, games)
                {
                    if (game.IsPlayed())
                    {
                        Team team;

                        if (Team::Find(teams, game.team1Id, team))
                        {
                            if (isSeason)
                            {
                                ranking.Add(team.player1Id, game.team1Score);
                                ranking.Add(team.player2Id, game.team1Score);
                            }
                            else
                            {
                                ranking.Add(team.id, game.team1Score);
                            }
                        }

                        if (Team::Find(teams, game.team2Id, team))
                        {
                            if (isSeason)
                            {
                                ranking.Add(team.player1Id, game.team2Score);
                                ranking.Add(team.player2Id, game.team2Score);
                            }
                            else
                            {
                                ranking.Add(team.id, game.team2Score);
                            }
                        }
                    }
                }
            }
        }
    }

    ranking.Show(ui->tableContest, mDatabase.GetPlayerList(), mTeams, isSeason);
    ui->tableContest->sortByColumn(2, Qt::DescendingOrder);
}


#include <QStandardPaths>
#include <iostream>
#include <QMessageBox>
#include <QFileDialog>
#include "Log.h"
#include "MainWindow.h"
#include "TableHelper.h"
#include "ui_MainWindow.h"

static const QString gVersion = "1.3";


// Table headers
QStringList mGamesTableHeader;
QStringList mEventsTableHeader;
QStringList mPlayersTableHeader;
QStringList mRankingTableHeader;
QStringList mTeamsTableHeader;

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

    // Setup signals for TAB 1: players management
    connect(ui->buttonAddPlayer, &QPushButton::clicked, this, &MainWindow::slotAddPlayer);
    connect(ui->buttonEditPlayer, &QPushButton::clicked, this, &MainWindow::slotEditPlayer);

    // Setup signals for TAB 2: club championship ranking
    connect(ui->comboRankingSeasons, SIGNAL(currentIndexChanged(int)), this, SLOT(slotRankingSeasonChanged(int)));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotTabChanged(int)));
    connect(ui->buttonExport, &QPushButton::clicked, this, &MainWindow::slotExport);

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
    connect(ui->buttonStart, &QPushButton::clicked, this, &MainWindow::slotStartRounds);

    connect(ui->buttonAddGame, &QPushButton::clicked, this, &MainWindow::slotAddGame);
    connect(ui->buttonEditGame, &QPushButton::clicked, this, &MainWindow::slotEditGame);
    connect(ui->buttonDeleteGame, &QPushButton::clicked, this, &MainWindow::slotDeleteGame);

    // Setup other stuff
    mDatabase.Initialize();
    mGamesTableHeader << tr("Id") << tr("Partie") << tr("Rencontre");
    mEventsTableHeader << tr("Id") << tr("Date") << tr("Type") << tr("Titre") << tr("État");
    mPlayersTableHeader << tr("Id") << tr("UUID") << tr("Prénom") << tr("Nom") << tr("Pseudonyme") << tr("E-mail") << tr("Téléphone (mobile)") << tr("Téléphone (maison)") << tr("Date de naissance") << tr("Rue") << tr("Code postal") << tr("Ville") << tr("Licences") << tr("Commentaires") << tr("Statut") << tr("Divers");
    mRankingTableHeader << tr("Id") << tr("Joueur") << tr("Points") << tr("Parties jouées");
    mTeamsTableHeader << tr("Id") << ("Nom de l'équipe");

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
    helper.Initialize(mPlayersTableHeader, list.size());

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

void MainWindow::UpdateTeamList(int eventId)
{
    mTeams = mDatabase.GetTeams(eventId);
    mPlayersInTeams.clear();

    if (mCurrentEvent.state == Event::cNotStarted)
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
    helper.Initialize(mTeamsTableHeader, mTeams.size());

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

            QList<QVariant> rowData;
            rowData << team.id << team.teamName;
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

            if (mCurrentEvent.id != -1)
            {
                std::cout << "Current event id: " << mCurrentEvent.id << std::endl;
                UpdateTeamList(mCurrentEvent.id);
                UpdateGameList();
            }
        }
    }
}

void MainWindow::UpdateSeasons()
{
    QStringList seasons = mDatabase.GetSeasons();

    ui->comboSeasons->clear();
    ui->comboSeasons->addItems(seasons);

    ui->comboRankingSeasons->clear();
    ui->comboRankingSeasons->addItems(seasons);

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
        teamWindow->Initialize(mDatabase.GetPlayerList(), mPlayersInTeams);

        if (teamWindow->exec() == QDialog::Accepted)
        {
            Team team;
            teamWindow->GetTeam(team);
            team.eventId = mCurrentEvent.id;
            if (mDatabase.AddTeam(team))
            {
                UpdateTeamList(mCurrentEvent.id);
            }
        }
    }
}

void MainWindow::slotEditTeam()
{
    if (mCurrentEvent.state == Event::cNotStarted)
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

                    if (teamWindow->exec() == QDialog::Accepted)
                    {
                        teamWindow->GetTeam(team);
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
                                    tr("Les parties ont déjà commencé."),
                                    QMessageBox::Ok);
    }
}

void MainWindow::slotDeleteTeam()
{
    if (mCurrentEvent.state == Event::cNotStarted)
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
                                    tr("Les parties ont déjà commencé."),
                                    QMessageBox::Ok);
    }
}

void MainWindow::UpdateEventsTable()
{
    TableHelper helper(ui->eventTable);
    helper.Initialize(mEventsTableHeader, mEvents.size());

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

void MainWindow::slotStartRounds()
{
    if (mCurrentEvent.state == Event::cNotStarted)
    {
        QList<Game> games = bracketWindow->BuildRounds(mTeams);

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
                                    tr("Les parties ont déjà commencé."),
                                    QMessageBox::Ok);
    }
}

void MainWindow::slotShowGames()
{
    if (mCurrentEvent.state != Event::cNotStarted)
    {
        UpdateGameList();
        bracketWindow->setWindowModality(Qt::NonModal);
        bracketWindow->show();
    }
    else
    {
        (void) QMessageBox::warning(this, tr("Tanca"),
                                    tr("Il faut lancer la partie avant d'afficher les tours."),
                                    QMessageBox::Ok);
    }
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

    if (mCurrentEvent.state == Event::cNotStarted)
    {
        ui->buttonStart->setEnabled(true);
    }
    else
    {
        ui->buttonStart->setEnabled(false);
    }

    TableHelper helper(ui->gameTable);
    helper.Initialize(mGamesTableHeader, mGames.size());

    foreach (Game game, mGames)
    {
        Team t1, t2;
        bool valid = Team::Find(mTeams, game.team1Id, t1);
        valid = valid && Team::Find(mTeams, game.team2Id, t2);

        if (valid)
        {
            QList<QVariant> gameData;
            gameData << game.id << (int)(game.turn + 1) << (t1.teamName + " <--> " + t2.teamName);
            helper.AppendLine(gameData, game.IsPlayed());
        }
        else
        {
            TLogError("Cannot update game list!");
        }
    }

    helper.Finish();
    ui->gameTable->sortByColumn(1, Qt::AscendingOrder);
}

void MainWindow::slotAddGame()
{

}

void MainWindow::slotEditGame()
{
    if (mCurrentEvent.state == Event::cStarted)
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

                    bool valid = Team::Find(mTeams, game.team1Id, team1);
                    valid = valid && Team::Find(mTeams, game.team2Id, team2);

                    if (valid)
                    {
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
            }
            else
            {
                TLogError("Cannot find any valid game in the list");
            }
        }
    }
    else
    {
        (void) QMessageBox::warning(this, tr("Tanca"),
                                    tr("Il faut démarrer la compétition d'abord."),
                                    QMessageBox::Ok);
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
    slotRankingSeasonChanged(ui->comboRankingSeasons->currentIndex());
}

void MainWindow::slotExport()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Exporter le classement au format Excel (CSV)"),
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
            for( int c = 0; c < ui->tableContest->columnCount(); ++c )
            {
                strList << ui->tableContest->horizontalHeaderItem(c)->data(Qt::DisplayRole).toString();
            }

            data << strList.join(";") << "\n";

            // Export table contents
            for( int r = 0; r < ui->tableContest->rowCount(); ++r )
            {
                strList.clear();
                for( int c = 0; c < ui->tableContest->columnCount(); ++c )
                {
                    strList << ui->tableContest->item( r, c )->text();
                }
                data << strList.join( ";" ) + "\n";
            }
            f.close();
        }
    }
}

void MainWindow::slotRankingSeasonChanged(int index)
{
    QList<Event> events = mDatabase.GetEvents(ui->comboRankingSeasons->itemText(index).toInt());

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

        void Show(QTableWidget *table, const QList<Player> &players)
        {
            TableHelper helper(table);
            helper.Initialize(mRankingTableHeader, mList.size());

            QMapIterator<int, Rank> i(mList);
            while (i.hasNext())
            {
                i.next();

                // Fill the line
                Rank rank = i.value();
                int id = i.key();
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
        }

    private:
        QMap<int, Rank> mList;
    };

    Ranking ranking; // player id, Rank

    foreach (Event event, events)
    {
        if (event.state == Event::cStarted)
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
                        ranking.Add(team.player1Id, game.team1Score);
                        ranking.Add(team.player2Id, game.team1Score);
                    }
                    else
                    {
                        TLogError("[RANKING] Algorithm problem, cannot find team!");
                    }

                    if (Team::Find(teams, game.team2Id, team))
                    {
                        ranking.Add(team.player1Id, game.team2Score);
                        ranking.Add(team.player2Id, game.team2Score);
                    }
                    else
                    {
                        TLogError("[RANKING] Algorithm problem, cannot find team!");
                    }
                }
            }
        }
    }

    ranking.Show(ui->tableContest, mDatabase.GetPlayerList());
    ui->tableContest->setSortingEnabled(true);
    ui->tableContest->sortByColumn(2, Qt::DescendingOrder);
}


#include <QStandardPaths>
#include <iostream>
#include <QMessageBox>
#include <QFileDialog>
#include "Log.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

static const QString gVersion = "1.2";

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

    eventWindow = new EventWindow(this);
    eventWindow->hide();

    setWindowTitle("Tanca " + gVersion + " - Logiciel de gestion de club et de concours de pétanque.");

    // Setup signals for the menu
    connect(ui->actionImporter, &QAction::triggered, this, &MainWindow::slotImportFile);

    // Setup signals for TAB 1: players management
    connect(ui->buttonAddPlayer, &QPushButton::clicked, this, &MainWindow::slotAddPlayer);
    connect(ui->buttonEditPlayer, &QPushButton::clicked, this, &MainWindow::slotEditPlayer);

    // Setup signals for TAB 2: club championship ranking
    connect(ui->comboRankingSeasons, SIGNAL(currentIndexChanged(int)), this, SLOT(slotRankingSeasonChanged(int)));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotTabChanged(int)));
    connect(ui->buttonExport, &QPushButton::clicked, this, &MainWindow::slotExport);

    // Setup signals for TAB 3: club championship management
    connect(ui->buttonAddMatch, &QPushButton::clicked, this, &MainWindow::slotAddEvent);
    connect(ui->buttonAddTeam, &QPushButton::clicked, this, &MainWindow::slotAddTeam);
    connect(ui->eventList, SIGNAL(itemSelectionChanged()), this, SLOT(slotEventItemActivated()));
    connect(ui->buttonShowRounds, &QPushButton::clicked, this, &MainWindow::slotShowGames);
    connect(ui->comboSeasons, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSeasonChanged(int)));
    connect(ui->buttonStart, &QPushButton::clicked, this, &MainWindow::slotStartRounds);
    connect(ui->buttonEditGame, &QPushButton::clicked, this, &MainWindow::slotEditGame);

    // Setup other stuff
    mDatabase.Initialize();

    // Initialize views
    InitializePlayers();
    UpdateSeasons();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitializePlayers()
{
    ui->playersWidget->setModel(mDatabase.GetPlayersModel());
    ui->playersWidget->hideColumn(1); // don't show the UUID
    ui->playersWidget->hideColumn(14); // don't show the State
    ui->playersWidget->hideColumn(15); // don't show the Document
    ui->playersWidget->show();

    int count = ui->eventList->count();
    if (count > 0)
    {
        ui->eventList->setCurrentRow(count - 1);
    }
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
        if (!mDatabase.AddPlayer(newPlayer))
        {
            TLogError("Cannot add player!");
        }
    }
}

void MainWindow::slotEditPlayer()
{
    QModelIndexList indexes = ui->playersWidget->selectionModel()->selection().indexes();

    if (indexes.size() > 1)
    {
        QModelIndex index = indexes.at(0);
        QMap<int, QVariant> data = ui->playersWidget->model()->itemData(index);

        if (data.contains(0))
        {
            int id = data[0].toInt();
            std::cout << "Player ID: " << id << std::endl;
            Player p;
            if (mDatabase.FindPlayer(id, p))
            {
                playerWindow->SetPlayer(p);
                if (playerWindow->exec() == QDialog::Accepted)
                {
                    playerWindow->GetPlayer(p);
                    if (!mDatabase.EditPlayer(p))
                    {
                        TLogError("Cannot edit player!");
                    }
                }
            }
        }
    }
}

void MainWindow::UpdateTeamList(int eventId)
{
    ui->teamList->clear();
    mTeams = mDatabase.GetTeams(eventId);
    mPlayersInTeams.clear();

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

            ui->teamList->addItem(team.teamName);
        }
    }
}

void MainWindow::slotEventItemActivated()
{
    int match = ui->eventList->currentRow();
    if (match > -1)
    {
        mCurrentEvent = mDatabase.GetEvent(ui->eventList->currentItem()->text());
        std::cout << "Current match id: " << mCurrentEvent.id << std::endl;
        UpdateTeamList(mCurrentEvent.id);
        UpdateGameList();
    }
}

void MainWindow::UpdateSeasons()
{
    QStringList seasons = mDatabase.GetSeasons();

    ui->comboSeasons->clear();
    ui->comboSeasons->addItems(seasons);

    ui->comboRankingSeasons->clear();
    ui->comboRankingSeasons->addItems(seasons);
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
    int index = ui->eventList->currentRow();
    if (index >= 0)
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

void MainWindow::slotAddTeam()
{
    int match = ui->eventList->currentRow();
    if (match > -1)
    {
        // Prepare widget contents
        teamWindow->Initialize(mDatabase.GetPlayerList(), mPlayersInTeams);

        if (teamWindow->exec() == QDialog::Accepted)
        {
            Team team = teamWindow->GetTeam();
            team.eventId = mCurrentEvent.id;
            if (mDatabase.AddTeam(team))
            {
                UpdateTeamList(mCurrentEvent.id);
            }
        }
    }
}


void MainWindow::slotSeasonChanged(int index)
{
    ui->eventList->clear();
    mEvents = mDatabase.GetEvents(ui->comboSeasons->itemText(index).toInt());

    QStringList dates;
    foreach (Event match, mEvents)
    {
        dates.append(match.date.toString(Qt::ISODate));
    }

    ui->eventList->addItems(dates);

    int count = ui->eventList->count();
    if (count > 0)
    {
        ui->eventList->setCurrentRow(count - 1);
    }
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
    ui->gameList->clear();
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

    foreach (Game game, mGames)
    {
        Team t1, t2;
        bool valid = Team::Find(mTeams, game.team1Id, t1);
        valid = valid && Team::Find(mTeams, game.team2Id, t2);

        if (valid)
        {
            ui->gameList->addItem(t1.teamName + " <--> " + t2.teamName + "(" + QString("%1").arg(game.id) + ")");
        }
        else
        {
            TLogError("Cannot update game list!");
        }
    }
}

void MainWindow::slotEditGame()
{
    int index = ui->gameList->currentRow();
    if (index >= 0)
    {
        QRegularExpression re("\\.*(\\d+)");
        QRegularExpressionMatch match = re.match(ui->gameList->item(index)->text());

        if (match.hasMatch())
        {
            int id = match.captured(1).toInt();
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
                    gameWindow->SetGame(game, team1, team2);
                    if (gameWindow->exec() == QDialog::Accepted)
                    {
                        gameWindow->GetGame(game);
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

    class MyTableWidgetItem : public QTableWidgetItem {
        public:
            MyTableWidgetItem(const QString &text)
              : QTableWidgetItem(text)
            {

            }

            bool operator <(const QTableWidgetItem &other) const
            {
                return text().toInt() < other.text().toInt();
            }
    };

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
            QStringList header;
            header << tr("Joueur") << tr("Points") << tr("Parties jouées");
            table->clear();
            table->setHorizontalHeaderLabels(header);
            table->setRowCount(mList.size());

            std::cout << "Filling contest table: " << mList.size() << " elements." << std::endl;

            QMapIterator<int, Rank> i(mList);
            int row = 0;
            while (i.hasNext())
            {
                i.next();

                // Fill the line
                Rank rank = i.value();
                int id = i.key();
                Player player;
                if (Player::Find(players, id, player))
                {
                    MyTableWidgetItem *col1 = new MyTableWidgetItem(player.FullName());
                    MyTableWidgetItem *col2 = new MyTableWidgetItem(tr("%1").arg(rank.points));
                    MyTableWidgetItem *col3 = new MyTableWidgetItem(tr("%1").arg(rank.playedGames));

                    table->setItem(row, 0, col1);
                    table->setItem(row, 1, col2);
                    table->setItem(row, 2, col3);
                    row++;
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
    ui->tableContest->sortByColumn(1, Qt::DescendingOrder);
}


#include <QStandardPaths>
#include <iostream>
#include <QMessageBox>
#include "Log.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

static const QString gVersion = "1.0";

QString gAppDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/tanca";
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

    setWindowTitle("Tanca " + gVersion + " - Logiciel de gestion de club et de concours de pétanque.");

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

    // Setup signals for TAB 1: players management
    connect(ui->buttonAddPlayer, &QPushButton::clicked, this, &MainWindow::slotAddPlayer);
    connect(ui->buttonEditPlayer, &QPushButton::clicked, this, &MainWindow::slotEditPlayer);

    // Setup signals for TAB 3: club championship management
    connect(ui->buttonAddMatch, &QPushButton::clicked, this, &MainWindow::slotAddEvent);
    connect(ui->buttonAddTeam, &QPushButton::clicked, this, &MainWindow::slotAddTeam);
    connect(ui->matchList, SIGNAL(itemSelectionChanged()), this, SLOT(slotMatchItemActivated()));
    connect(ui->buttonShowRounds, &QPushButton::clicked, this, &MainWindow::slotShowGames);
    connect(ui->comboSeasons, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSeasonChanged(int)));
    connect(ui->buttonStart, &QPushButton::clicked, this, &MainWindow::slotStartRounds);
    connect(ui->buttonEditGame, &QPushButton::clicked, this, &MainWindow::slotEditGame);

    // Setup other stuff
    mDatabase.Initialize();

    // Initialize views
    InitializePlayers();
    ui->comboSeasons->addItems(mDatabase.GetSeasons());
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

    int count = ui->matchList->count();
    if (count > 0)
    {
        ui->matchList->setCurrentRow(count - 1);
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

void MainWindow::slotMatchItemActivated()
{
    int match = ui->matchList->currentRow();
    if (match > -1)
    {
        mCurrentEvent = mDatabase.GetEvent(ui->matchList->currentItem()->text());
        std::cout << "Current match id: " << mCurrentEvent.id << std::endl;
        UpdateTeamList(mCurrentEvent.id);
    }
}

void MainWindow::slotAddTeam()
{
    int match = ui->matchList->currentRow();
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

void MainWindow::slotAddEvent()
{
    if (datePickerWindow->exec() == QDialog::Accepted)
    {
        Event match;
        QDate date = datePickerWindow->GetDate();
        match.date = date;
        match.year = date.year();
        if (mDatabase.AddEvent(match))
        {
            ui->comboSeasons->clear();
            ui->comboSeasons->addItems(mDatabase.GetSeasons());
        }
    }
}

void MainWindow::slotSeasonChanged(int index)
{
    ui->matchList->clear();
    mMatches = mDatabase.GetEvents(ui->comboSeasons->itemText(index).toInt());

    QStringList dates;
    foreach (Event match, mMatches)
    {
        dates.append(match.date.toString(Qt::ISODate));
    }

    ui->matchList->addItems(dates);

    int count = ui->matchList->count();
    if (count > 0)
    {
        ui->matchList->setCurrentRow(count - 1);
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

bool MainWindow::FindTeam(const int id, Team &team)
{
    bool found = false;
    for (int i = 0; i < mTeams.size(); i++)
    {
        if (mTeams[i].id == id)
        {
            found = true;
            team = mTeams[i];
            break;
        }
    }

    return found;
}

void MainWindow::UpdateGameList()
{
    ui->gameList->clear();
    QList<Game> games = mDatabase.GetGames(mCurrentEvent.id);
    bracketWindow->SetGames(games);

    foreach (Game game, games)
    {
        Team t1, t2;
        bool valid = FindTeam(game.team1Id, t1);
        valid = valid && FindTeam(game.team2Id, t2);

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
    /*
    QModelIndexList indexes = ui->gameList->selectionModel()->selection().indexes();

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
    */
}

#include <QStandardPaths>
#include <iostream>
#include "MainWindow.h"
#include "ui_MainWindow.h"

QString gDbFullPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/tanca/tanca.db";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mDatabase(gDbFullPath)
{
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
    connect(ui->buttonAddMatch, &QPushButton::clicked, this, &MainWindow::slotAddMatch);
    connect(ui->buttonAddTeam, &QPushButton::clicked, this, &MainWindow::slotAddTeam);
    connect(ui->matchList, SIGNAL(itemSelectionChanged()), this, SLOT(slotMatchItemActivated()));
    connect(ui->buttonShowRounds, &QPushButton::clicked, this, &MainWindow::slotShowRounds);
    connect(ui->comboSeasons, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSeasonChanged(int)));

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
    ui->playersWidget->show();

    int count = ui->matchList->count();
    if (count > 0)
    {
        ui->matchList->setCurrentRow(count - 1);
    }
}


void MainWindow::UpdateTeamList(int matchId)
{
    ui->teamList->clear();
    mTeams = mDatabase.GetTeams(matchId);
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
        int matchId = mDatabase.GetMatch(ui->matchList->currentItem()->text());
        UpdateTeamList(matchId);
    }
}

void MainWindow::slotAddTeam()
{
    int match = ui->matchList->currentRow();
    if (match > -1)
    {
        int matchId = mDatabase.GetMatch(ui->matchList->currentItem()->text());

        // Prepare widget contents
        teamWindow->Initialize(mDatabase.GetPlayerList(), mPlayersInTeams);

        if (teamWindow->exec() == QDialog::Accepted)
        {
            Team team = teamWindow->GetTeam();
            team.matchId = matchId;
            if (mDatabase.AddTeam(team))
            {
                UpdateTeamList(matchId);
            }
        }
    }
}

void MainWindow::slotAddMatch()
{
    if (datePickerWindow->exec() == QDialog::Accepted)
    {
        Match match;
        QDate date = datePickerWindow->GetDate();
        match.date = date;
        match.year = date.year();
        if (mDatabase.AddMatch(match))
        {
            ui->comboSeasons->clear();
            ui->comboSeasons->addItems(mDatabase.GetSeasons());
        }
    }
}

void MainWindow::slotSeasonChanged(int index)
{
    ui->matchList->clear();
    mMatches = mDatabase.GetMatches(ui->comboSeasons->itemText(index).toInt());

    QStringList dates;
    foreach (Match match, mMatches)
    {
        dates.append(match.date.toString());
    }

    ui->matchList->addItems(dates);

    int count = ui->matchList->count();
    if (count > 0)
    {
        ui->matchList->setCurrentRow(count - 1);
    }
}


void MainWindow::slotShowRounds()
{
    bracketWindow->SetTeams(mTeams);
    bracketWindow->setWindowModality(Qt::NonModal);
    bracketWindow->show();
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
            std::cout << "Cannot add player!" << std::endl;
        }
    }
}

void MainWindow::slotEditPlayer()
{
   // ->currentIndex().row()
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
                        std::cout << "Cannot edit player!" << std::endl;
                    }
                }
            }
        }
    }
}

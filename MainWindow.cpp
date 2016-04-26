#include <QStandardPaths>

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

    matchWindow = new MatchWindow(this);
    matchWindow->hide();

    bracketWindow = new BracketWindow(this);
    bracketWindow->hide();

    teamWindow = new TeamWindow(this);
    teamWindow->hide();

    // Setup signals
    connect(ui->buttonAddPlayer, &QPushButton::clicked, this, &MainWindow::slotAddPlayer);
    connect(ui->buttonAddMatch, &QPushButton::clicked, this, &MainWindow::slotAddMatch);
    connect(ui->buttonAddTeam, &QPushButton::clicked, this, &MainWindow::slotAddTeam);

    connect(ui->buttonShowRounds, &QPushButton::clicked, this, &MainWindow::slotShowRounds);
    connect(ui->comboSeasons, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSeasonChanged(int)));

    // Setup other stuff
    mDatabase.Initialize();

    // Initialize views
    InitializePlayers();
    ui->comboSeasons->addItems(mDatabase.GetSeasons());
}

void MainWindow::InitializePlayers()
{
    ui->playersWidget->setModel(mDatabase.GetPlayersModel());
    ui->playersWidget->hideColumn(0); // don't show the ID
    ui->playersWidget->show();
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotAddTeam()
{
    int match = ui->matchList->currentRow();
    if (match > -1)
    {
        // Prepare widget contents
        teamWindow->SetPlayers(mDatabase.GetPlayerList());

        if (teamWindow->exec() == QDialog::Accepted)
        {
            if (mDatabase.AddTeam(teamWindow->GetTeam()))
            {
                ui->teamList->clear();
                ui->teamList->addItems(mDatabase.GetTeams());
            }
        }
    }
}

void MainWindow::slotAddMatch()
{
    if (matchWindow->exec() == QDialog::Accepted)
    {
        if (mDatabase.AddMatch(matchWindow->GetMatch()))
        {
            ui->comboSeasons->clear();
            ui->comboSeasons->addItems(mDatabase.GetSeasons());
        }
    }
}

void MainWindow::slotSeasonChanged(int index)
{
    ui->matchList->clear();
    ui->matchList->addItems(mDatabase.GetMatches(ui->comboSeasons->itemText(index).toInt()));
}


void MainWindow::slotShowRounds()
{
    bracketWindow->setWindowModality(Qt::NonModal);
    bracketWindow->show();
}

void MainWindow::slotAddPlayer()
{
    if (playerWindow->exec() == QDialog::Accepted)
    {
        mDatabase.AddPlayer(playerWindow->GetPlayer());
    }
}


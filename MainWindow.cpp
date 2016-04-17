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

    // Setup signals
    connect(ui->buttonAddPlayer, &QPushButton::clicked, this, &MainWindow::slotAddPlayer);
    connect(ui->buttonAddMatch, &QPushButton::clicked, this, &MainWindow::slotAddMatch);
    connect(ui->buttonShowRounds, &QPushButton::clicked, this, &MainWindow::slotShowRounds);

    // Setup other stuff
    mDatabase.Initialize();

    // Initialize views
    InitializePlayers();
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

void MainWindow::slotAddMatch()
{
    if (matchWindow->exec() == QDialog::Accepted)
    {
        //mDatabase.AddMatch(playerWindow->GetPerson());
    }
}

void MainWindow::slotAddTeam()
{

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
        mDatabase.AddPerson(playerWindow->GetPerson());
    }
}


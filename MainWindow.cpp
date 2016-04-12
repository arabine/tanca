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

    roundWindow = new RoundWindow(this);
    roundWindow->hide();

    // Setup signals
    connect(ui->buttonAddPlayer, &QPushButton::clicked, this, &MainWindow::slotAddPlayer);
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

void MainWindow::slotShowRounds()
{
    roundWindow->setWindowModality(Qt::NonModal);
    roundWindow->show();
}

void MainWindow::slotAddPlayer()
{
    if (playerWindow->exec() == QDialog::Accepted)
    {
        mDatabase.AddPerson(playerWindow->GetPerson());
    }
}


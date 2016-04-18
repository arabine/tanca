#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DbManager.h"
#include "PlayerWindow.h"
#include "MatchWindow.h"
#include "BracketWindow.h"
#include "TeamWindow.h"

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
     void slotShowRounds();
     void slotAddTeam();
     void slotAddMatch();
     void slotSeasonChanged(int index);
private:
    void InitializePlayers();

    // Windows
    Ui::MainWindow *ui;
    PlayerWindow *playerWindow;
    BracketWindow *bracketWindow;
    MatchWindow *matchWindow;
    TeamWindow *teamWindow;

    // other stuff
    DbManager mDatabase;
};

#endif // MAINWINDOW_H

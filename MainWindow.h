#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DbManager.h"
#include "PlayerWindow.h"
#include "RoundWindow.h"

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

private:
    void InitializePlayers();

    // Windows
    Ui::MainWindow *ui;
    PlayerWindow *playerWindow;
    RoundWindow *roundWindow;

    // other stuff
    DbManager mDatabase;
};

#endif // MAINWINDOW_H

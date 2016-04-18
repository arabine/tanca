#include "PlayerWindow.h"


PlayerWindow::PlayerWindow(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    mPlayer.name = "zozo";
}

Player PlayerWindow::GetPlayer()
{
    return mPlayer;
}


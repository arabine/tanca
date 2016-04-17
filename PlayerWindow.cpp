#include "PlayerWindow.h"


PlayerWindow::PlayerWindow(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    mPerson.name = "zozo";
}

Player PlayerWindow::GetPerson()
{
    return mPerson;
}


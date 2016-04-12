#include "PlayerWindow.h"


PlayerWindow::PlayerWindow(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);

    mPerson.name = "zozo";
}

Person PlayerWindow::GetPerson()
{
    return mPerson;
}


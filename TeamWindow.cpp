#include "TeamWindow.h"

TeamWindow::TeamWindow(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(slotAccept()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void TeamWindow::slotAccept()
{
    mTeam
    accept();
}


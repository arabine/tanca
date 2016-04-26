#include "TeamWindow.h"

TeamWindow::TeamWindow(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(slotAccept()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui.buttonSwap, &QPushButton::clicked, this, &TeamWindow::slotClicked);

    connect(ui.memberList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(slotMemberItemActivated(QListWidgetItem*)));
    connect(ui.teamList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(slotTeamItemActivated(QListWidgetItem*)));
}

void TeamWindow::SetPlayers(const QList<Player> &players)
{
    mList = players;
    Update();
}

void TeamWindow::Update()
{
    ui.memberList->clear();
    ui.teamList->clear();

    foreach (Player p, mList)
    {
        ui.memberList->addItem(p.name + " " + p.lastName);
    }

    foreach (Player p, mSelection)
    {
        ui.teamList->addItem(p.name + " " + p.lastName);
    }
}

void TeamWindow::slotMemberItemActivated(QListWidgetItem *item)
{
    (void) item;
    ui.teamList->clearSelection();
    ui.teamList->clearFocus();
}

void TeamWindow::slotTeamItemActivated(QListWidgetItem *item)
{
    (void) item;
    ui.memberList->clearSelection();
    ui.memberList->clearFocus();
}

void TeamWindow::slotClicked()
{
    int selectionLeft = ui.memberList->currentRow();
    int selectionRight = ui.teamList->currentRow();

    if (selectionLeft > -1)
    {
        if (mSelection.size() < 2)
        {
            const Player &p = mList.at(selectionLeft);
            // transfer to the right
            mSelection.append(p);
            mList.removeAt(selectionLeft);
        }
    }
    else if (selectionRight > -1)
    {
        const Player &p = mSelection.at(selectionRight);
        // transfer to the right
        mList.append(p);
        mSelection.removeAt(selectionRight);
    }

    Update();
}

void TeamWindow::slotAccept()
{
    if (mSelection.size() == 2)
    {
        mTeam.player1Id = mSelection.at(0).id;
        mTeam.player2Id = mSelection.at(1).id;
        mTeam.player3Id = -1;
        accept();
    }
}


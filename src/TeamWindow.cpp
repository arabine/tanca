#include "TeamWindow.h"
#include "TableHelper.h"
#include "Log.h"

QStringList gPlayerListHeader;

TeamWindow::TeamWindow(QWidget *parent)
    : QDialog(parent)
    , mTeamSize(2)
{
    ui.setupUi(this);

    connect(ui.buttonOk, SIGNAL(clicked(bool)), this, SLOT(slotAccept()));
    connect(ui.buttonCancel, SIGNAL(clicked(bool)), this, SLOT(reject()));
    connect(ui.buttonSwap, &QPushButton::clicked, this, &TeamWindow::slotClicked);

    connect(ui.playersTable, SIGNAL(itemSelectionChanged()), this, SLOT(slotPlayerItemActivated()));
    connect(ui.teamList, &QListWidget::itemClicked, this, &TeamWindow::slotTeamItemActivated);

    gPlayerListHeader << tr("Id") << tr("Prénom") << tr("Nom") << tr("Pseudo");
}

void TeamWindow::SetTeam(const Player &p1, const Player &p2)
{
    mSelection.clear();
    mSelection.append(p1);
    mSelection.append(p2);
    Update();
}

void TeamWindow::GetTeam(Team &team)
{
    if (mSelection.size() >= 2)
    {
        team.player1Id = mSelection.at(0).id;
        team.player2Id = mSelection.at(1).id;
    }

    if (mSelection.size() >= 3)
    {
        team.player3Id = mSelection.at(2).id;
    }

    // Update team name
    team.CreateName(mSelection.at(0).name, mSelection.at(1).name);
}

void TeamWindow::Initialize(const QList<Player> &players, const QList<int> &inTeams)
{
    // Create a list of players that are still alone
    mList.clear();
    foreach (Player p, players)
    {
        // This player has no team, add it to the list of available players
        if (!inTeams.contains(p.id))
        {
            mList.append(p);
        }
    }

    mSelection.clear();
    Update();
}

void TeamWindow::Update()
{
    ui.playersTable->clear();
    ui.teamList->clear();

    TableHelper helper(ui.playersTable);
    helper.Initialize(gPlayerListHeader, mList.size());

    foreach (Player p, mList)
    {
        QList<QVariant> rowData;
        rowData << p.id << p.name << p.lastName << p.nickName;
        helper.AppendLine(rowData, false);
    }

    ui.playersTable->setSortingEnabled(true);

    foreach (Player p, mSelection)
    {
        ui.teamList->addItem(p.name + " " + p.lastName);
    }

    helper.Finish();
}

void TeamWindow::slotPlayerItemActivated()
{
    ui.teamList->clearSelection();
    ui.teamList->clearFocus();
}

void TeamWindow::slotTeamItemActivated(QListWidgetItem *item)
{
    (void) item;
    ui.playersTable->clearSelection();
    ui.playersTable->clearFocus();
}

void TeamWindow::slotClicked()
{
    int selectionRight = ui.teamList->currentRow();
    TableHelper helper(ui.playersTable);

    int id = -1;
    (void) helper.GetFirstColumnValue(id);

    if (id > -1)
    {
        Player p;
        if (Player::Find(mList, id, p) && (mSelection.size() < mTeamSize))
        {
            // transfer to the right and remove the player from the list
            mSelection.append(p);
            int index;
            if (Player::Index(mList, id, index))
            {
                mList.removeAt(index);
            }
        }
    }
    else if (selectionRight > -1)
    {
        const Player &p = mSelection.at(selectionRight);
        // transfer to the left
        mList.append(p);
        mSelection.removeAt(selectionRight);
    }

    Update();
}

void TeamWindow::slotAccept()
{
    if (mSelection.size() == mTeamSize)
    {
        accept();
    }
}


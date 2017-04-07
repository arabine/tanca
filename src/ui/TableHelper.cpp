
#include <iostream>
#include <QModelIndex>
#include <QTableWidgetItem>
#include <QHeaderView>

#include "TableHelper.h"
#include "Log.h"

QStringList gEventRankingTableHeader;
QStringList gSeasonRankingTableHeader;


TableHelper::TableHelper(QTableWidget *widget)
    : mWidget(widget)
    , mSelectedColor(255,218,185)
    , mRow(0)
{

    gEventRankingTableHeader << tr("Id") << tr("Numéro") << tr("Équipe") << tr("Parties gagnées") << tr("Parties perdues") << tr("Points marqués") << tr("Points concédés") << tr("Différence") << tr("Buchholz");
    gSeasonRankingTableHeader << tr("Id") << tr("Joueur") << tr("Parties gagnées") << tr("Parties perdues") << tr("Points marqués") << tr("Points concédés") << tr("Différence");
}

bool TableHelper::GetFirstColumnValue(int &value)
{
    bool ret = false;
    QModelIndexList indexes = mWidget->selectionModel()->selection().indexes();

    if (indexes.size() > 1)
    {
        QModelIndex index = indexes.at(0);
        QMap<int, QVariant> data = mWidget->model()->itemData(index);

        if (data.contains(0))
        {
            value = data[0].toInt();
            ret = true;
            std::cout << "Selected row value: " << value << std::endl;
        }
    }

    return ret;
}


void TableHelper::Initialize(const QStringList &header, int rows)
{
    mWidget->clear();
    mWidget->setRowCount(rows);
    mWidget->setColumnCount(header.size());
    mWidget->hideColumn(0);
    mWidget->setHorizontalHeaderLabels(header);
    mWidget->setSortingEnabled(false);

    mRow = 0;
}

void TableHelper::SetSelectedColor(const QColor &color)
{
    mSelectedColor = color;
    mWidget->setStyleSheet("alternate-background-color: " + color.name() + " ;background-color: white;");
}

void TableHelper::SetAlternateColors(bool enable)
{
    mWidget->setAlternatingRowColors(enable);
}

void TableHelper::Finish()
{
    mWidget->setSortingEnabled(true);
    mWidget->horizontalHeader()->setStretchLastSection(true);
    mWidget->resizeColumnsToContents();
}

void TableHelper::AppendLine(const QList<QVariant> &list, bool selected)
{
    int column = 0;

    foreach (QVariant data, list)
    {
        QTableWidgetItem *cell;
        if (data.type() == QVariant::Int)
        {
            cell = new IntegerTableItem(data.toInt());
        }
        else
        {
            cell = new QTableWidgetItem(data.toString());
        }

        if (selected)
        {
            cell->setBackgroundColor(mSelectedColor);
        }
        mWidget->setItem(mRow, column, cell);
        column++;
    }
    mRow++;
}

void TableHelper::Export(const QString &fileName)
{
    // FIXME: detect the output format thanks to the file extension

    QFile f( fileName );
    if (f.open(QFile::WriteOnly))
    {
        QTextStream data( &f );
        QStringList strList;

        // Export header title
        for( int c = 0; c < mWidget->columnCount(); ++c )
        {
            strList << mWidget->horizontalHeaderItem(c)->data(Qt::DisplayRole).toString();
        }

        data << strList.join(";") << "\n";

        // Export table contents
        for( int r = 0; r < mWidget->rowCount(); ++r )
        {
            strList.clear();
            for( int c = 0; c < mWidget->columnCount(); ++c )
            {
                strList << mWidget->item( r, c )->text();
            }
            data << strList.join( ";" ) + "\n";
        }
        f.close();
    }
}

void TableHelper::Show(QTableWidget *table, const QList<Player> &players, const QList<Team> &teams, bool isSeason, const QMap<int, Rank> &list)
{
    SetSelectedColor(QColor(245,245,220));
    SetAlternateColors(true);

    if (isSeason)
    {
        Initialize(gSeasonRankingTableHeader, list.size());
    }
    else
    {
        Initialize(gEventRankingTableHeader, list.size());
    }

    QMapIterator<int, Rank> i(list);
    while (i.hasNext())
    {
        i.next();

        // Fill the line
        Rank rank = i.value();
        int id = i.key();

        if (isSeason)
        {
            // Show the whole season ranking
            Player player;
            if (Player::Find(players, id, player))
            {
                QList<QVariant> rowData;
                rowData << player.id << player.FullName() << rank.gamesWon << rank.gamesLost << rank.pointsWon << rank.pointsLost << rank.Difference();
                AppendLine(rowData, false);
            }
            else
            {
                TLogError("Cannot find player to create ranking!");
            }
        }
        else
        {
            // Show the event result
            Team team;
            if (Team::Find(teams, id, team))
            {
                QList<QVariant> rowData;
                rowData << team.id << team.number << team.teamName << rank.gamesWon << rank.gamesLost << rank.pointsWon << rank.pointsLost << rank.Difference();
                AppendLine(rowData, false);
            }
            else
            {
                TLogError("Cannot find team to create ranking!");
            }
        }
    }

    Finish();
}


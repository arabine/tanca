/*=============================================================================
 * Tanca - TableHelper.cpp
 *=============================================================================
 * Utility class to add helpers for all tables
 *=============================================================================
 * Tanca ( https://github.com/belegar/tanca ) - This file is part of Tanca
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony.rabine@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#include <iostream>
#include <QModelIndex>
#include <QTableWidgetItem>
#include <QHeaderView>

#include "TableHelper.h"
#include "Log.h"


QStringList InitEventRanking()
{
    QStringList list;
    list << QObject::tr("Id") << QObject::tr("Rang") << QObject::tr("Numéro d'équipe") << QObject::tr("Équipe")
         << QObject::tr("Gagnés")<< QObject::tr("Nuls")  << QObject::tr("Perdus") << QObject::tr("Points marqués")
         << QObject::tr("Points concédés") << QObject::tr("Différence") << QObject::tr("Buchholz");
    return list;
}


QStringList InitSeasonRanking()
{
    QStringList list;
    list << QObject::tr("Id") << QObject::tr("Rang")  << QObject::tr("Joueur") << QObject::tr("Gagnés")<< QObject::tr("Nuls")  << QObject::tr("Perdus")
         << QObject::tr("Points marqués") << QObject::tr("Points concédés") << QObject::tr("Différence") << QObject::tr("Parties jouées") ;
    return list;
}

QStringList gEventRankingTableHeader = InitEventRanking();
QStringList gSeasonRankingTableHeader = InitSeasonRanking();


TableHelper::TableHelper(QTableWidget *widget)
    : mWidget(widget)
    , mSelectedColor(255,218,185)
    , mRow(0)
{

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

void TableHelper::AppendLine(const std::list<Value> &list, bool selected)
{
    int column = 0;

    for (auto const &data : list)
    {
        QTableWidgetItem *cell;
        if (data.GetType() == Value::INTEGER)
        {
            cell = new IntegerTableItem(data.GetInteger());
        }
        else
        {
            cell = new QTableWidgetItem(data.GetString().c_str());
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

#include <JsonWriter.h>

void TableHelper::Export(const QString &fileName)
{
    // FIXME: detect the output format thanks to the file extension
    QString ext = QFileInfo(fileName).completeSuffix().toLower();
    //bool saveInCSV = (ext == "csv");
    bool saveInJson = (ext == "json");

    QFile f( fileName );

    if (f.open(QFile::WriteOnly))
    {
        QTextStream data( &f );
        QStringList strList;
        QStringList titles;

        JsonArray players;

        // Export header title
        for( int c = 0; c < mWidget->columnCount(); ++c )
        {
            QString title = mWidget->horizontalHeaderItem(c)->data(Qt::DisplayRole).toString();
            title.replace(" ", "_");
            title.replace("(", "");
            title.replace(")", "");
            title.replace("é", "e");
            title = title.toLower();
            titles << title;
        }

        data << titles.join(";") << "\n";

        // Export table contents
        for( int r = 0; r < mWidget->rowCount(); ++r )
        {
            strList.clear();
            JsonObject player;
            for( int c = 0; c < mWidget->columnCount(); ++c )
            {
                QString element = mWidget->item( r, c )->text();
                player.AddValue( titles[c].toStdString(), element.toStdString());
                strList << element;
            }
            players.AddValue(player);
            data << strList.join( ";" ) + "\n";
        }

        JsonObject root;
        root.AddValue("players", players);

        f.close();

        if (saveInJson)
        {
            JsonWriter::SaveToFile(root, fileName.toStdString());
        }
    }
}

void TableHelper::Show(const std::deque<Player> &players, const std::deque<Team> &teams, bool isSeason, const std::deque<Rank> &list)
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

    int line = 1;
    for (auto &rank : list)
    {
        if (isSeason)
        {
            // Show the whole season ranking
            Player player;
            if (Player::Find(players, rank.id, player))
            {
                int nbGames = rank.gamesWon + rank.gamesLost + rank.gamesDraw;
                std::list<Value> rowData = {player.id, line, player.FullName(), rank.gamesWon, rank.gamesDraw, rank.gamesLost, rank.pointsWon, rank.pointsLost, rank.Difference(), nbGames};
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
            if (Team::Find(teams, rank.id, team))
            {
                std::list<Value> rowData = {team.id, line, team.number, team.teamName, rank.gamesWon, rank.gamesDraw, rank.gamesLost, rank.pointsWon, rank.pointsLost, rank.Difference(), rank.pointsOpponents};
                AppendLine(rowData, false);
            }
            else
            {
                TLogError("Cannot find team to create ranking!");
            }
        }
        line++;
    }

    Finish();
}


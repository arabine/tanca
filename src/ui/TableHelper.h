/*=============================================================================
 * Tanca - TableHelper.h
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

#ifndef TABLE_HELPER_H
#define TABLE_HELPER_H

#include <QtCore>
#include <QTableWidget>
#include "DbManager.h"
#include "Tournament.h"

/**
 * @brief Custom table cell to enable column sorting with integers
 */
class IntegerTableItem : public QTableWidgetItem {
    public:
        IntegerTableItem(int value)
          : QTableWidgetItem(QString("%1").arg(value))
        {

        }

        bool operator <(const QTableWidgetItem &other) const
        {
            return text().toInt() < other.text().toInt();
        }
};

/**
 * @brief Base class helper
 */
class TableHelper : public QObject
{
    Q_OBJECT

public:
    TableHelper(QTableWidget *widget);

    void SetTableWidget(QTableWidget *widget) { mWidget = widget; }

    bool GetFirstColumnValue(int &value);
    void Initialize(const QStringList &header, int rows);
    void AppendLine(const QList<QVariant> &list, bool selected);
    void Finish();
    void SetSelectedColor(const QColor &color);
    void SetAlternateColors(bool enable);
    void Export(const QString &fileName);
    void Show(const QList<Player> &players, const QList<Team> &teams, bool isSeason, const std::vector<Rank> &list);

private:
    QTableWidget *mWidget;
    QColor mSelectedColor;
    int mRow;
};

#endif // TABLE_HELPER_H

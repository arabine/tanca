/*=============================================================================
 * Tanca - Brackets.h
 *=============================================================================
 * QML item used to share bracket elements for Canvas
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

#ifndef BRACKETS_H
#define BRACKETS_H

#include <QObject>

class Brackets : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString rounds READ getRounds WRITE setRounds NOTIFY roundsChanged)

public:
    explicit Brackets(QObject *parent = 0);

    QString getRounds();
    void setRounds(const QString &rounds);

signals:
  void roundsChanged();

public slots:

private:
    QString mRounds;
};

#endif // BRACKETS_H

//=============================================================================
// End of file Brackets.h
//=============================================================================

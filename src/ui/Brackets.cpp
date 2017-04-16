/*=============================================================================
 * Tanca - Brackets.cpp
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

#include "Brackets.h"

Brackets::Brackets(QObject *parent)
    : QObject(parent)
    , mRounds("[]")
{

}

QString Brackets::getRounds()
{
    return mRounds;
}

void Brackets::setRounds(const QString &rounds)
{
    mRounds = rounds;
    emit roundsChanged();
}

//=============================================================================
// End of file Brackets.cpp
//=============================================================================

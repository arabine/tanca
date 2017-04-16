/*=============================================================================
 * Tanca - DatePickerWindow.cpp
 *=============================================================================
 * Base class to edit/add a date-time
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

#include "DatePickerWindow.h"
#include <QVBoxLayout>

DatePickerWindow::DatePickerWindow(QWidget *parent)
    : QDialog(parent)
{
    mDate = new QDateTimeEdit(this);
    mDate->setCalendarPopup(true);
    mDate->setDateTime(QDateTime::currentDateTime());

    mButtons = new QDialogButtonBox(QDialogButtonBox::Ok
                                         | QDialogButtonBox::Cancel);

    connect(mButtons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(mButtons, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(mDate);
    mainLayout->addWidget(mButtons);

    setLayout(mainLayout);
}

//=============================================================================
// End of file DatePickerWindow.h
//=============================================================================


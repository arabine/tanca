/*=============================================================================
 * Tanca - EventWindow.cpp
 *=============================================================================
 * Manage events (add/new)
 *=============================================================================
 * Tanca ( https://github.com/belegar/tanca ) - This file is part of Tanca
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony.rabine@tarotclub.fr
 *
 * Tanca is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tanca is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tanca.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#include "EventWindow.h"

EventWindow::EventWindow(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    connect(ui.buttonOk, &QPushButton::clicked, this, &EventWindow::accept);
    connect(ui.buttonCancel, &QPushButton::clicked, this, &EventWindow::reject);
}


void EventWindow::GetEvent(Event &event)
{
    event.date = Util::FromISODateTime(ui.dateTimeEdit->dateTime().toString(Qt::ISODate).toStdString());
    event.state = ui.comboState->currentIndex();
    event.title = ui.lineTitle->text().toStdString();
    event.type = ui.comboType->currentIndex();
    event.option = ui.checkBoxSeasonRanking->isChecked() ? Event::cOptionSeasonRanking : Event::cNoOption;
}

void EventWindow::SetEvent(const Event &event)
{
    ui.dateTimeEdit->setDateTime(QDateTime::fromString(Util::ToISODateTime(event.date).c_str(), Qt::ISODate));
    ui.comboState->setCurrentIndex(event.state);
    ui.lineTitle->setText(event.title.c_str());
    ui.comboType->setCurrentIndex(event.type);

    if (event.HasOption(Event::cOptionSeasonRanking))
    {
        ui.checkBoxSeasonRanking->setChecked(true);
    }
    else
    {
        ui.checkBoxSeasonRanking->setChecked(false);
    }
}

//=============================================================================
// End of file EventWindow.cpp
//=============================================================================

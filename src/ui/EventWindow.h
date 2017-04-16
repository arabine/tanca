/*=============================================================================
 * Tanca - EventWindow.h
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

#ifndef EVENTWINDOW_H
#define EVENTWINDOW_H

#include <QDialog>
#include "DbManager.h"
#include "ui_EventWindow.h"

class EventWindow : public QDialog
{
    Q_OBJECT

public:
    EventWindow(QWidget *parent = 0);
    void GetEvent(Event &event);
    void SetEvent(const Event &event);

private:
    Ui::EventWindow ui;
};

#endif // EVENTWINDOW_H

//=============================================================================
// End of file EventWindow.h
//=============================================================================

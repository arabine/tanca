/*=============================================================================
 * Tanca - SelectionWindow.h
 *=============================================================================
 * Base class widget that is used by several selection dialogs
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

#ifndef SELECTION_WINDOW_H
#define SELECTION_WINDOW_H

#include <QDialog>
#include "ui_SelectionWindow.h"
#include "TableHelper.h"

class SelectionWindow : public QDialog
{
    Q_OBJECT

public:
    SelectionWindow(QWidget *parent, const QString &title, int minSize, int maxSize);

    void SetHeader(const QStringList &tableHeader);
    void StartUpdate(int size);
    void FinishUpdate();
    void AddLeftEntry(const QList<QVariant> &rowData);
    void AddRightEntry(const QString &text);
    int GetMaxSize() { return mMaxSize; }
    void SetName(const QString &name);
    void SetNumber(uint32_t number);
    uint32_t GetNumber();
    QString GetName();
    void AllowZeroNumber(bool enable);

    // Virtual methods that must be implemented in the child classes
    virtual void ClickedRight(int index) = 0;
    virtual void ClickedLeft(int id) = 0;

    void SetLabelNumber(const QString &name);
private slots:
    void slotAccept();

    void slotClicked();
    void slotPlayerItemActivated();
    void slotSelectionItemActivated(QListWidgetItem *item);

    void slotFilter();
protected:
    Ui::SelectionWindow ui;

    TableHelper mHelper;
    int mMinSize;
    int mMaxSize;
    QStringList mTableHeader;
};

#endif // SELECTION_WINDOW_H

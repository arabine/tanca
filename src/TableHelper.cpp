#include "TableHelper.h"
#include <iostream>
#include <QModelIndex>
#include <QTableWidgetItem>

TableHelper::TableHelper(QTableWidget *widget)
    : mWidget(widget)
    , row(0)
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

    row = 0;
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

        cell->setBackgroundColor(selected ? Qt::lightGray : Qt::white);
        mWidget->setItem(row, column, cell);
        column++;
    }
    row++;
}


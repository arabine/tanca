#ifndef TABLE_HELPER_H
#define TABLE_HELPER_H

#include <QtCore>
#include <QTableWidget>

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

    bool GetFirstColumnValue(int &value);
    void Initialize(const QStringList &header, int rows);
    void AppendLine(const QList<QVariant> &list, bool selected);
    void Finish();
private:
    QTableWidget *mWidget;
    int row;

};

#endif // TABLE_HELPER_H

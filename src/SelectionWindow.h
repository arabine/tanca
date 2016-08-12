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
    void SetNumber(uint32_t number);
    uint32_t GetNumber();
    void AllowZeroNumber(bool enable);

    // Virtual methods that must be implemented in the child classes
    virtual void ClickedRight(int index) = 0;
    virtual void ClickedLeft(int id) = 0;

private slots:
    void slotAccept();

    void slotClicked();
    void slotPlayerItemActivated();
    void slotSelectionItemActivated(QListWidgetItem *item);
private:
    Ui::SelectionWindow ui;

    TableHelper mHelper;
    int mMinSize;
    int mMaxSize;
    QStringList mTableHeader;
};

#endif // SELECTION_WINDOW_H

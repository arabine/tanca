#ifndef MATCHWINDOW_H
#define MATCHWINDOW_H

#include <QDialog>
#include <QPushButton>
#include <QDateTimeEdit>
#include <QDialogButtonBox>

#include "DbManager.h"

class MatchWindow : public QDialog
{
    Q_OBJECT
public:
    MatchWindow(QWidget *parent = 0);

    Match GetMatch() { return mMatch; }

private:
    QDateEdit *mDate;
    QDialogButtonBox *mButtons;

    Match mMatch;

};

#endif // MATCHWINDOW_H

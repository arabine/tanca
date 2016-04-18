#include "MatchWindow.h"
#include <QVBoxLayout>

MatchWindow::MatchWindow(QWidget *parent)
    : QDialog(parent)
{
    mDate = new QDateEdit(this);
    mDate->setCalendarPopup(true);
    mDate->setDate(QDate::currentDate());

    mButtons = new QDialogButtonBox(QDialogButtonBox::Ok
                                         | QDialogButtonBox::Cancel);

    connect(mButtons, SIGNAL(accepted()), this, SLOT(slotAccept()));
    connect(mButtons, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(mDate);
    mainLayout->addWidget(mButtons);

    setLayout(mainLayout);
}

void MatchWindow::slotAccept()
{
    QDate date = mDate->date();
    mMatch.date = date.toString(Qt::ISODate);
    mMatch.year = date.year();
    accept();
}


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


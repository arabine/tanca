#ifndef DATEPICKERWINDOW_H
#define DATEPICKERWINDOW_H

#include <QDialog>
#include <QPushButton>
#include <QDateTimeEdit>
#include <QDialogButtonBox>


class DatePickerWindow : public QDialog
{
    Q_OBJECT
public:
    DatePickerWindow(QWidget *parent = 0);

    QDateTime GetDateTime() { return mDate->dateTime(); }

private:
    QDateTimeEdit *mDate;
    QDialogButtonBox *mButtons;

};

#endif // DATEPICKERWINDOW_H

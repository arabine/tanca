
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
    event.date = ui.dateTimeEdit->dateTime();
    event.state = ui.comboState->currentIndex();
    event.title = ui.lineTitle->text();
    event.type = ui.comboType->currentIndex();
}

void EventWindow::SetEvent(const Event &event)
{
    ui.dateTimeEdit->setDateTime(event.date);
    ui.comboState->setCurrentIndex(event.state);
    ui.lineTitle->setText(event.title);
    ui.comboType->setCurrentIndex(event.type);
}

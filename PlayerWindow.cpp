#include "PlayerWindow.h"


PlayerWindow::PlayerWindow(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    datePickerWindow = new DatePickerWindow(this);
    datePickerWindow->hide();

    connect(ui.buttonAddLicence, &QPushButton::clicked, this, &PlayerWindow::slotAddLicence);
}


void PlayerWindow::slotAddLicence()
{
    if (datePickerWindow->exec() == QDialog::Accepted)
    {
        QDate date = datePickerWindow->GetDate();
        ui.listLicences->addItem(date.toString());
    }
}

void PlayerWindow::SetPlayer(const Player &player)
{
    ui.dateBirth->setDate(player.birthDate);
    ui.lineCity->setText(player.city);
    ui.plainComments->setPlainText(player.comments);
    ui.lineLastName->setText(player.lastName);
    ui.listLicences->clear();
    ui.listLicences->addItems(player.membership.split(";"));
    ui.lineName->setText(player.name);
    ui.lineNickName->setText(player.nickName);
    ui.spinPostCode->setValue(player.postCode);
    ui.lineStreet->setText(player.road);
}

void PlayerWindow::GetPlayer(Player &player)
{
    player.birthDate = ui.dateBirth->date();
    player.city = ui.lineCity->text();
    player.comments = ui.plainComments->toPlainText();
    player.lastName = ui.lineLastName->text();

    // Separate each licence with a ';'
    QString licences;
    for (int i = 0; i < ui.listLicences->count(); i++)
    {
        if (i != 0)
        {
            licences += ";";
        }
        licences += ui.listLicences->item(i)->text();
    }

    player.membership = licences;
    player.name = ui.lineName->text();
    player.nickName = ui.lineNickName->text();
    player.postCode = ui.spinPostCode->value();
    player.road = ui.lineStreet->text();

}


#include "PlayerWindow.h"


PlayerWindow::PlayerWindow(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    datePickerWindow = new DatePickerWindow(this);
    datePickerWindow->hide();

    connect(ui.buttonAddLicence, &QPushButton::clicked, this, &PlayerWindow::slotAddLicence);
    connect(ui.buttonOk, &QPushButton::clicked, this, &PlayerWindow::accept);
    connect(ui.buttonCancel, &QPushButton::clicked, this, &PlayerWindow::reject);
}


void PlayerWindow::slotAddLicence()
{
    if (datePickerWindow->exec() == QDialog::Accepted)
    {
        QDateTime date = datePickerWindow->GetDateTime();
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
    QStringList items = player.membership.split(";");
    if (items.size() > 0)
    {
        // Manually add elements to be able to test each value
        foreach (QString item, items)
        {
            if (!item.isEmpty())
            {
                ui.listLicences->addItem(item);
            }
        }
    }

    ui.lineName->setText(player.name);
    ui.lineNickName->setText(player.nickName);
    ui.spinPostCode->setValue(player.postCode);
    ui.lineStreet->setText(player.road);
    ui.lineMobilePhone->setText(player.mobilePhone);
    ui.lineHomePhone->setText(player.homePhone);
    ui.lineMail->setText(player.email);
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
    player.mobilePhone = ui.lineMobilePhone->text();
    player.homePhone = ui.lineHomePhone->text();
    player.email = ui.lineMail->text();
}


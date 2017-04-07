#include "PlayerWindow.h"
#include <QMessageBox>

PlayerWindow::PlayerWindow(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    datePickerWindow = new DatePickerWindow(this);
    datePickerWindow->hide();

    connect(ui.buttonAddLicence, &QPushButton::clicked, this, &PlayerWindow::slotAddLicence);
    connect(ui.buttonOk, &QPushButton::clicked, this, &PlayerWindow::slotAccept);
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

void PlayerWindow::slotAccept()
{
    bool ok = true;

    if (ui.lineName->text().size() == 0)
    {
        (void) QMessageBox::warning(this, tr("Formulaire de joueur"),
                                            tr("Il faut inscrire un prénom"),
                                            QMessageBox::Ok);
        ok = false;
    }

    if (ui.lineLastName->text().size() == 0)
    {
        (void) QMessageBox::warning(this, tr("Formulaire de joueur"),
                                            tr("Il faut inscrire un nom"),
                                            QMessageBox::Ok);
        ok = false;
    }

    if (ok)
    {
        accept();
    }
}

void PlayerWindow::SetPlayer(const Player &player)
{
    QString name = player.name.toLower();
    if (name.size() > 0)
    {
        name.replace(0, 1, name[0].toUpper());
    }

    QString lastName = player.lastName.toLower();
    if (lastName.size() > 0)
    {
        lastName.replace(0, 1, lastName[0].toUpper());
    }

    QString nickName = player.nickName.toLower();
    if (nickName.size() > 0)
    {
        nickName.replace(0, 1, nickName[0].toUpper());
    }

    ui.dateBirth->setDate(player.birthDate);
    ui.lineCity->setText(player.city);
    ui.plainComments->setPlainText(player.comments);
    ui.lineName->setText(name);
    ui.lineNickName->setText(nickName);
    ui.lineLastName->setText(lastName);

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

    ui.spinPostCode->setValue(player.postCode);
    ui.lineStreet->setText(player.road);
    ui.lineMobilePhone->setText(player.mobilePhone);
    ui.lineHomePhone->setText(player.homePhone);
    ui.lineMail->setText(player.email);
}

void PlayerWindow::GetPlayer(Player &player)
{
    QString name = ui.lineName->text().toLower();
    if (name.size() > 0)
    {
        name.replace(0, 1, name[0].toUpper());
    }

    QString lastName = ui.lineLastName->text().toLower();
    if (lastName.size() > 0)
    {
        lastName.replace(0, 1, lastName[0].toUpper());
    }

    QString nickName = ui.lineNickName->text().toLower();
    if (nickName.size() > 0)
    {
        nickName.replace(0, 1, nickName[0].toUpper());
    }

    player.birthDate = ui.dateBirth->date();
    player.city = ui.lineCity->text();
    player.comments = ui.plainComments->toPlainText();
    player.lastName = lastName;
    player.name = name;
    player.nickName = nickName;

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
    player.postCode = ui.spinPostCode->value();
    player.road = ui.lineStreet->text();
    player.mobilePhone = ui.lineMobilePhone->text();
    player.homePhone = ui.lineHomePhone->text();
    player.email = ui.lineMail->text();
}


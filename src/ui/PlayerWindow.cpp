#include "PlayerWindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include "Log.h"
#include "TableHelper.h"

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

bool PlayerWindow::AddPlayer(DbManager &db)
{
    bool success = false;
    Player newPlayer;

    SetPlayer(newPlayer);
    if (exec() == QDialog::Accepted)
    {
        GetPlayer(newPlayer);
        if (db.AddPlayer(newPlayer))
        {
            success = true;
        }
        else
        {
            TLogError("Cannot add player!");
        }
    }
    return success;
}

bool PlayerWindow::EditPlayer(DbManager &db, QTableWidget *widget)
{
    bool success = false;
    TableHelper helper(widget);

    int id;
    if (helper.GetFirstColumnValue(id))
    {
        Player p;
        if (db.FindPlayer(id, p))
        {
            SetPlayer(p);
            if (exec() == QDialog::Accepted)
            {
                GetPlayer(p);
                if (db.EditPlayer(p))
                {
                    success = true;
                }
                else
                {
                    TLogError("Cannot edit player!");
                }
            }
        }
    }
    return success;
}

bool PlayerWindow::DeletePlayer(DbManager &db, QTableWidget *widget)
{
    bool success = false;

    TableHelper helper(widget);

    int id;
    if (helper.GetFirstColumnValue(id))
    {
        Player p;
        if (db.FindPlayer(id, p))
        {
            bool canDelete = true;
            // We allow deleting a player if there is no any game finished for him
            QList<Team> teams = db.GetTeamsByPlayerId(id);

            if (teams.size() > 0)
            {
                // Check if the player has played some games
                foreach (Team team, teams)
                {
                    Event event = db.GetEvent(team.eventId);

                    if (event.IsValid())
                    {
                        canDelete = false;
                    }
                }
            }

            if (canDelete)
            {
                // Granted to delete, actually do it!
                if (db.DeletePlayer(id))
                {
                    success = true;
                    (void)QMessageBox::information(this, tr("Suppression d'un joueur"),
                                        tr("Suppression du joueur réussie."),
                                        QMessageBox::Ok);
                }
                else
                {
                    (void)QMessageBox::critical(this, tr("Suppression d'un joueur"),
                                        tr("La tentative de suppression a échoué."),
                                        QMessageBox::Ok);
                }
            }
            else
            {
                (void)QMessageBox::critical(this, tr("Suppression d'un joueur"),
                                        tr("Impossible de supprimer un joueur ayant participé à un événement.\n"
                                           "Supprimez l'événement d'abord."),
                                        QMessageBox::Ok);
            }
        }
    }

    return success;
}

bool PlayerWindow::ImportPlayerFile(DbManager &db)
{
    bool importNoError = true;
    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Ouvrir un fichier CSV"), QStandardPaths::displayName(QStandardPaths::DocumentsLocation), tr("Fichier CSV (*.csv)"));

    QFile file(fileName);

    if (file.exists())
    {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            TLogError("Cannot open CSV file: " + fileName.toStdString());
            importNoError = false;
        }
        else
        {
            while (!file.atEnd())
            {
                QString line = file.readLine();
                QStringList exploded = line.split(";");

                // We need at least 5 information for a player
                if (exploded.size() >= 5)
                {
                    // Add this new player, if not already exist in the database
                    Player p;

                    p.lastName = exploded.at(0);
                    p.name = exploded.at(1);
                    p.road = exploded.at(2);
                    p.email = exploded.at(3);
                    p.mobilePhone = exploded.at(4);

                    QString fullname = p.name + " " + p.lastName;

                    if (!db.PlayerExists(p) && db.IsValid(p))
                    {
                        if (!db.AddPlayer(p))
                        {
                            TLogError("Import failed for player: " + fullname.toStdString());
                            importNoError = false;
                        }
                    }
                    else
                    {
                        TLogError("Player " + fullname.toStdString() + " is invalid or already exists in the database, cannot import it");
                        importNoError = false;
                    }
                }
                else
                {
                    TLogError("Bad CSV file format");
                    importNoError = false;
                }
            }
        }
    }
    else
    {
        TLogError("Cannot find CSV file: " + fileName.toStdString());
        importNoError = false;
    }

    return importNoError;
}



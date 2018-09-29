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
    QString name = Util::ToLower(player.name).c_str();
    if (name.size() > 0)
    {
        name.replace(0, 1, name[0].toUpper());
    }

    QString lastName = Util::ToLower(player.lastName).c_str();
    if (lastName.size() > 0)
    {
        lastName.replace(0, 1, lastName[0].toUpper());
    }

    QString nickName = Util::ToLower(player.nickName).c_str();
    if (nickName.size() > 0)
    {
        nickName.replace(0, 1, nickName[0].toUpper());
    }

    ui.dateBirth->setDate(QDate::fromString(Util::ToISODateTime(player.birthDate).c_str(), Qt::ISODate));
    ui.lineCity->setText(player.city.c_str());
    ui.plainComments->setPlainText(player.comments.c_str());
    ui.lineName->setText(name);
    ui.lineNickName->setText(nickName);
    ui.lineLastName->setText(lastName);

    ui.listLicences->clear();
    std::vector<std::string> items = Util::Split(player.membership, ";");
    if (items.size() > 0)
    {
        // Manually add elements to be able to test each value
        for (auto const &item : items)
        {
            if (item.size() > 0)
            {
                ui.listLicences->addItem(item.c_str());
            }
        }
    }

    ui.spinPostCode->setValue(player.postCode);
    ui.lineStreet->setText(player.road.c_str());
    ui.lineMobilePhone->setText(player.mobilePhone.c_str());
    ui.lineHomePhone->setText(player.homePhone.c_str());
    ui.lineMail->setText(player.email.c_str());
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

    player.birthDate = Util::FromISODateTime(ui.dateBirth->date().toString().toStdString());
    player.city = ui.lineCity->text().toStdString();
    player.comments = ui.plainComments->toPlainText().toStdString();
    player.lastName = lastName.toStdString();
    player.name = name.toStdString();
    player.nickName = nickName.toStdString();

    // Separate each licence with a ';'
    std::string licences;
    for (int i = 0; i < ui.listLicences->count(); i++)
    {
        if (i != 0)
        {
            licences += ";";
        }
        licences += ui.listLicences->item(i)->text().toStdString();
    }

    player.membership = licences;
    player.postCode = ui.spinPostCode->value();
    player.road = ui.lineStreet->text().toStdString();
    player.mobilePhone = ui.lineMobilePhone->text().toStdString();
    player.homePhone = ui.lineHomePhone->text().toStdString();
    player.email = ui.lineMail->text().toStdString();
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
            std::deque<Team> teams = db.GetTeamsByPlayerId(id);

            if (teams.size() > 0)
            {
                // Check if the player has played some games
                for (auto const &team : teams)
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

                    p.lastName = exploded.at(0).toStdString();
                    p.name = exploded.at(1).toStdString();
                    p.road = exploded.at(2).toStdString();
                    p.email = exploded.at(3).toStdString();
                    p.mobilePhone = exploded.at(4).toStdString();

                    std::string fullname = p.name + " " + p.lastName;

                    if (!db.PlayerExists(p) && db.IsValid(p))
                    {
                        if (!db.AddPlayer(p))
                        {
                            TLogError("Import failed for player: " + fullname);
                            importNoError = false;
                        }
                    }
                    else
                    {
                        TLogError("Player " + fullname + " is invalid or already exists in the database, cannot import it");
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



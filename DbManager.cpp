#include "DbManager.h"
#include "Log.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QUuid>
#include <iostream>

static QStringList MakeTables()
{
    QStringList tables;

    tables << Player::Table();
    tables << Event::Table();
    tables << Team::Table();
    tables << Game::Table();

    return tables;
}


DbManager::DbManager(const QString &path)
{
    // Create directory if not exists
    QDir().mkpath(QFileInfo(path).absolutePath());

    mDb = QSqlDatabase::addDatabase("QSQLITE", "tanca");
    mDb.setDatabaseName(path);
}

DbManager::~DbManager()
{
    if (mDb.isOpen())
    {
        mDb.close();
    }
}

void DbManager::Initialize()
{
    if (mDb.open())
    {
        qDebug() << "Database: connection ok";

        // Create tables if it is a new file
        QStringList gTables = MakeTables();
        for (int i = 0; i < gTables.size(); i++)
        {
            QSqlQuery query(gTables[i], mDb);
            if(!query.exec())
            {
                qDebug() << "Create table failed: " << query.lastError();
            }
            else
            {
                QRegularExpression re("EXISTS (\\w+) \\(");
                QRegularExpressionMatch match = re.match(gTables[i]);
                if (match.hasMatch())
                {
                    qDebug() << "Created table: " << match.captured(1);
                }
            }
        }

        // Init models
        mPlayersModel = new QSqlTableModel(this, mDb);
        mPlayersModel->setTable("players");
        mPlayersModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        mPlayersModel->select();
        mPlayersModel->setHeaderData(0, Qt::Horizontal, tr("Id"));
        mPlayersModel->setHeaderData(1, Qt::Horizontal, tr("UUID"));
        mPlayersModel->setHeaderData(2, Qt::Horizontal, tr("Prénom"));
        mPlayersModel->setHeaderData(3, Qt::Horizontal, tr("Nom"));
        mPlayersModel->setHeaderData(4, Qt::Horizontal, tr("Pseudonyme"));
        mPlayersModel->setHeaderData(5, Qt::Horizontal, tr("E-mail"));
        mPlayersModel->setHeaderData(6, Qt::Horizontal, tr("Téléphone (mobile)"));
        mPlayersModel->setHeaderData(7, Qt::Horizontal, tr("Téléphone (maison)"));
        mPlayersModel->setHeaderData(8, Qt::Horizontal, tr("Date de naissance"));
        mPlayersModel->setHeaderData(9, Qt::Horizontal, tr("Rue"));
        mPlayersModel->setHeaderData(10, Qt::Horizontal, tr("Code postal"));
        mPlayersModel->setHeaderData(11, Qt::Horizontal, tr("Ville"));
        mPlayersModel->setHeaderData(12, Qt::Horizontal, tr("Licences"));
        mPlayersModel->setHeaderData(13, Qt::Horizontal, tr("Commentaires"));
        mPlayersModel->setHeaderData(14, Qt::Horizontal, tr("Statut"));
        mPlayersModel->setHeaderData(15, Qt::Horizontal, tr("Divers"));

        mPlayers = UpdatePlayerList();
    }
    else
    {
       qDebug() << "Error: connection with database fail";
    }

    // Initialize the Cities DB
    mCities = QSqlDatabase::addDatabase("QSQLITE", "cities");
    mCities.setDatabaseName("~/Tanca/villes.db"); // FIXME: use the executable path
}

QList<Player> &DbManager::GetPlayerList()
{
    return mPlayers;
}

bool DbManager::PlayerExists(const Player &player) const
{
    bool found = false;

    foreach (Player p, mPlayers)
    {
        if ((p.lastName == player.lastName) &&
            (p.name == player.name))
        {
            found = true;
            break;
        }
    }
    return found;
}

QList<Player> DbManager::UpdatePlayerList()
{
    QSqlQuery query("SELECT * FROM players", mDb);
    QList<Player> result;

    while (query.next())
    {
        Player player;

        player.id = query.value("id").toInt();
        player.uuid = query.value("uuid").toString();
        player.name = query.value("name").toString();
        player.lastName = query.value("last_name").toString();
        player.nickName = query.value("nick_name").toString();
        player.email = query.value("email").toString();
        player.mobilePhone = query.value("mobile_phone").toString();
        player.homePhone = query.value("home_phone").toString();
        player.birthDate = query.value("birth_date").toDate();
        player.road = query.value("road").toString();
        player.postCode = query.value("post_code").toInt();
        player.city = query.value("city").toString();
        player.membership = query.value("membership").toString();
        player.comments = query.value("comments").toString();
        player.state = query.value("state").toInt();
        player.document = query.value("document").toString();

        result.append(player);
    }
    return result;
}

bool DbManager::FindPlayer(int id, Player &player)
{
    return Player::Find(mPlayers, id, player);
}

bool DbManager::AddPlayer(const Player& player)
{
    bool success = false;

    if (IsValid(player))
    {
        QSqlQuery queryAdd(mDb);
        queryAdd.prepare("INSERT INTO players (uuid, name, last_name, nick_name, email, mobile_phone, home_phone, birth_date, road, post_code, city, membership, comments, state, document) "
                         "VALUES (:uuid, :name, :last_name, :nick_name, :email, :mobile_phone, :home_phone, :birth_date, :road, :post_code, :city, :membership, :comments, :state, :document)");
        queryAdd.bindValue(":uuid", QUuid::createUuid().toString());
        queryAdd.bindValue(":name", player.name);
        queryAdd.bindValue(":last_name", player.lastName);
        queryAdd.bindValue(":nick_name", player.nickName);
        queryAdd.bindValue(":email", player.email);
        queryAdd.bindValue(":mobile_phone", player.mobilePhone);
        queryAdd.bindValue(":home_phone", player.homePhone);
        queryAdd.bindValue(":birth_date", player.birthDate);
        queryAdd.bindValue(":road", player.road);
        queryAdd.bindValue(":post_code", player.postCode);
        queryAdd.bindValue(":city", player.city);
        queryAdd.bindValue(":membership", player.membership);
        queryAdd.bindValue(":comments", player.comments);
        queryAdd.bindValue(":state", player.state);
        queryAdd.bindValue(":document", player.document);

        if(queryAdd.exec())
        {
            qDebug() << "Add player success";
            success = true;
            mPlayersModel->select();

            mPlayers = UpdatePlayerList();
        }
        else
        {
            TLogError("Add player failed: " + queryAdd.lastError().text().toStdString());
        }
    }
    else
    {
        TLogError("Add player failed: name cannot be empty");
    }

    return success;
}

bool DbManager::EditPlayer(const Player& player)
{
    bool success = false;

    if (IsValid(player))
    {
        QSqlQuery queryEdit(mDb);
        queryEdit.prepare("UPDATE players SET name = :name, last_name = :last_name, nick_name = :nick_name, email = :email, "
                          "mobile_phone = :mobile_phone, home_phone = :home_phone, birth_date = :birth_date, road = :road, post_code = :post_code, "
                         "city = :city, membership = :membership, comments = :comments, state = :state, document = :document "
                         "WHERE id = :id");

        queryEdit.bindValue(":id", player.id);
        queryEdit.bindValue(":name", player.name);
        queryEdit.bindValue(":last_name", player.lastName);
        queryEdit.bindValue(":nick_name", player.nickName);
        queryEdit.bindValue(":email", player.email);
        queryEdit.bindValue(":mobile_phone", player.mobilePhone);
        queryEdit.bindValue(":home_phone", player.homePhone);
        queryEdit.bindValue(":birth_date", player.birthDate.toString());
        queryEdit.bindValue(":road", player.road);
        queryEdit.bindValue(":post_code", player.postCode);
        queryEdit.bindValue(":city", player.city);
        queryEdit.bindValue(":membership", player.membership);
        queryEdit.bindValue(":comments", player.comments);
        queryEdit.bindValue(":state", player.state);
        queryEdit.bindValue(":document", player.document);

        if(queryEdit.exec())
        {
            qDebug() << "Edit player success";
            success = true;
            mPlayersModel->select();

            mPlayers = UpdatePlayerList();
        }
        else
        {
            TLogError("Edit player failed: " + queryEdit.lastError().text().toStdString());
        }
    }
    else
    {
        TLogError("Edit player failed: name cannot be empty");
    }

    return success;
}


QStringList DbManager::GetSeasons()
{
    QSqlQuery query(mDb);
    QStringList result;

    query.prepare("SELECT DISTINCT year FROM events");
    if(query.exec())
    {
        while (query.next())
        {
            result.append(query.value(0).toString());
        }
    }
    else
    {
        TLogError("Get seasons failed: " + query.lastError().text().toStdString());
    }

    return result;
}

bool DbManager::UpdateEventState(const Event &event)
{
    bool success = false;

    QSqlQuery queryEdit(mDb);
    queryEdit.prepare("UPDATE events SET state = :state WHERE id = :id");
    queryEdit.bindValue(":id", event.id);
    queryEdit.bindValue(":state", event.state);

    if(queryEdit.exec())
    {
        qDebug() << "Edit event state success";
        success = true;
    }
    else
    {
        TLogError("Edit event state failed: " + queryEdit.lastError().text().toStdString());
    }

    return success;
}

QStringList DbManager::GetCities(int postCode)
{
    (void) postCode; // FIXME
    return QStringList();
}

bool DbManager::IsValid(const Player& player)
{
    // FIXME: check the validity of the player parameters
    bool valid = false;

    if ((player.name != "") && (player.lastName != ""))
    {
        valid = true;
    }
    return valid;
}

bool DbManager::AddEvent(const Event& event)
{
    bool success = false;

    QSqlQuery queryAdd(mDb);
    queryAdd.prepare("INSERT INTO events (year, date, title, state, type, document) VALUES (:year, :date, :title, :state, :type, :document)");

    queryAdd.bindValue(":year", event.year);
    queryAdd.bindValue(":date", event.date.toString(Qt::ISODate));
    queryAdd.bindValue(":title", event.title);
    queryAdd.bindValue(":state", event.state);
    queryAdd.bindValue(":type", event.type);
    queryAdd.bindValue(":document", event.document);

    if(queryAdd.exec())
    {
        qDebug() << "Add event success";
        success = true;
    }
    else
    {
        TLogError("Add event failed: " + queryAdd.lastError().text().toStdString());
    }

    return success;
}

bool DbManager::EditEvent(const Event& event)
{
    bool success = false;

    QSqlQuery queryEdit(mDb);

    queryEdit.prepare("UPDATE events SET year = :year, date = :date, title = :title, state = :state, type = :type, document = :document WHERE id = :id");

    queryEdit.bindValue(":id", event.id);
    queryEdit.bindValue(":year", event.year);
    queryEdit.bindValue(":date", event.date.toString(Qt::ISODate));
    queryEdit.bindValue(":title", event.title);
    queryEdit.bindValue(":state", event.state);
    queryEdit.bindValue(":type", event.type);
    queryEdit.bindValue(":document", event.document);

    if(queryEdit.exec())
    {
        qDebug() << "Edit game success";
        success = true;
    }
    else
    {
        TLogError("Edit game failed: " + queryEdit.lastError().text().toStdString());
    }
    return success;
}

Event DbManager::GetEvent(const QString &date)
{
    Event event;

    QSqlQuery query(mDb);
    query.prepare("SELECT * FROM events WHERE date = :date");
    query.bindValue(":date", date);

    if(query.exec())
    {
        if (query.next())
        {
            event.id = query.value("id").toInt();
            event.year = query.value("year").toInt();
            event.date = query.value("date").toDateTime();
            event.title = query.value("title").toString();
            event.state = query.value("state").toInt();
            event.type = query.value("type").toInt();
            event.document = query.value("document").toString();
        }
        else
        {
            TLogError("Cannot find any event for that date");
        }
    }

    return event;
}

QList<Event> DbManager::GetEvents(int year)
{
    QSqlQuery query(mDb);
    query.prepare("SELECT * FROM events WHERE year = :year");
    query.bindValue(":year", year);

    QList<Event> result;

    if(query.exec())
    {
        while (query.next())
        {
            Event event;
            event.id = query.value("id").toInt();
            event.year = query.value("year").toInt();
            event.date = query.value("date").toDateTime();
            event.title = query.value("title").toString();
            event.state = query.value("state").toInt();
            event.type = query.value("type").toInt();
            event.document = query.value("document").toString();
            result.append(event);
        }
    }
    return result;
}


bool DbManager::AddGames(const QList<Game>& games)
{
    bool success = false;

    foreach (Game game, games)
    {
        QSqlQuery queryAdd(mDb);
        queryAdd.prepare("INSERT INTO games (event_id, turn, team1_id, team2_id, team1_score, team2_score, state, document) "
                         "VALUES (:event_id, :turn, :team1_id, :team2_id, :team1_score, :team2_score, :state, :document)");
        queryAdd.bindValue(":event_id", game.eventId);
        queryAdd.bindValue(":turn", game.turn);
        queryAdd.bindValue(":team1_id", game.team1Id);
        queryAdd.bindValue(":team2_id", game.team2Id);
        queryAdd.bindValue(":team1_score", game.team1Score);
        queryAdd.bindValue(":team2_score", game.team2Score);
        queryAdd.bindValue(":state", game.state);
        queryAdd.bindValue(":document", game.document);

        if(queryAdd.exec())
        {
            qDebug() << "Add games success";
            success = true;
        }
        else
        {
            TLogError("Add games failed: " + queryAdd.lastError().text().toStdString());
            success = false;
            break;
        }
    }

    return success;
}

QList<Game> DbManager::GetGames(int event_id)
{
    QSqlQuery query(mDb);
    query.prepare("SELECT * FROM games WHERE event_id = :event_id");
    query.bindValue(":event_id", event_id);

    QList<Game> result;

    if(query.exec())
    {
        while (query.next())
        {
            Game game;
            game.id = query.value("id").toInt();
            game.eventId = query.value("event_id").toInt();
            game.turn = query.value("turn").toInt();
            game.team1Id = query.value("team1_id").toInt();
            game.team2Id = query.value("team2_id").toInt();
            game.team1Score = query.value("team1_score").toInt();
            game.team2Score = query.value("team2_score").toInt();
            game.state = query.value("state").toInt();
            game.document = query.value("document").toString();
            result.append(game);
        }
    }
    return result;
}

bool DbManager::EditGame(const Game& game)
{
    bool success = false;

    QSqlQuery queryEdit(mDb);

    queryEdit.prepare("UPDATE games SET event_id = :event_id, "
                     "turn = :turn, team1_id = :team1_id, team2_id = :team2_id, "
                     "team1_score = :team1_score, team2_score = :team2_score, "
                     "state = :state, document = :document "
                     "WHERE id = :id");

    queryEdit.bindValue(":id", game.id);
    queryEdit.bindValue(":event_id", game.eventId);
    queryEdit.bindValue(":turn", game.turn);
    queryEdit.bindValue(":team1_id", game.team1Id);
    queryEdit.bindValue(":team2_id", game.team2Id);
    queryEdit.bindValue(":team1_score", game.team1Score);
    queryEdit.bindValue(":team2_score", game.team2Score);
    queryEdit.bindValue(":state", game.state);
    queryEdit.bindValue(":document", game.document);

    if(queryEdit.exec())
    {
        qDebug() << "Edit game success";
        success = true;
    }
    else
    {
        TLogError("Edit game failed: " + queryEdit.lastError().text().toStdString());
    }
    return success;
}

bool DbManager::AddTeam(const Team &team)
{
    bool success = false;

    QSqlQuery queryAdd(mDb);
    queryAdd.prepare("INSERT INTO teams (event_id, team_name, player1_id, player2_id, player3_id, state, document) "
                     "VALUES (:event_id, :team_name, :player1_id, :player2_id, :player3_id, :state, :document)");
    queryAdd.bindValue(":event_id", team.eventId);
    queryAdd.bindValue(":team_name", team.teamName);
    queryAdd.bindValue(":player1_id", team.player1Id);
    queryAdd.bindValue(":player2_id", team.player2Id);
    queryAdd.bindValue(":player3_id", team.player3Id);
    queryAdd.bindValue(":state", team.state);
    queryAdd.bindValue(":document", team.document);

    if(queryAdd.exec())
    {
        qDebug() << "Add team success";
        success = true;
    }
    else
    {
        TLogError("Add team failed: " + queryAdd.lastError().text().toStdString());
    }

    return success;
}

QList<Team> DbManager::GetTeams(int eventId)
{
    QSqlQuery query(mDb);
    query.prepare("SELECT * FROM teams WHERE event_id = :event_id");
    query.bindValue(":event_id", eventId);

    QList<Team> result;

    if(query.exec())
    {
        while (query.next())
        {
            Team team;
            team.id = query.value("id").toInt();
            team.eventId = query.value("event_id").toInt();
            team.teamName = query.value("team_name").toString();
            team.player1Id = query.value("player1_id").toInt();
            team.player2Id = query.value("player2_id").toInt();
            team.player3Id = query.value("player3_id").toInt();
            team.state = query.value("state").toInt();
            team.document = query.value("document").toString();

            if (team.teamName == "")
            {
                // Create a team name
                Player p1, p2;

                bool valid = FindPlayer(team.player1Id, p1);
                valid = valid && FindPlayer(team.player2Id, p2);

                if (valid)
                {
                    team.teamName = p1.name + " / " + p2.name;
                }
                else
                {
                    std::cout << "Cannot find players" << std::endl;
                }
            }

            result.append(team);
        }
    }
    return result;
}


#include "DbManager.h"
#include "Log.h"
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QUuid>
#include <iostream>


/**
 * History of changes
 *
 * 1.2
 *      - Converted type "Club championship" into Round Robin
 *      - Added "Season Ranking" option in Event
 *
 * 1.0
 *      - Added Infos table
 *      - Added the 'number' column into 'teams' table
 * 0.0
 *      Initial version
 */

static const QString gBaseVersion = "0.0";
static const QString gVersion1_0 = "1.0";
static const QString gVersion1_1 = "1.1";
static const QString gVersion1_2 = "1.2";

static QStringList MakeTables()
{
    QStringList tables;

    tables << Player::Table();
    tables << Event::Table();
    tables << Team::Table();
    tables << Game::Table();
    tables << Infos::Table();
    tables << Reward::Table();

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

bool DbManager::EditInfos()
{
    bool success = false;

    QSqlQuery query(mDb);
    query.prepare("UPDATE infos SET version = :version");
    query.bindValue(":version", mInfos.version);

    if(query.exec())
    {
        qDebug() << "Updgraded DB version to " << mInfos.version;
        success = true;
    }
    else
    {
        TLogError("Updgraded DB failed: " + query.lastError().text().toStdString());
    }

    return success;
}

void DbManager::Upgrade()
{
    QSqlQuery query(mDb);

    if (mDb.tables().contains("infos"))
    {
        query.prepare("SELECT * FROM infos");

        if(query.exec())
        {
            if (query.next())
            {
                mInfos.version = query.value("version").toString();
            }
            else
            {
                // No any table entries, add one
                query.prepare("INSERT INTO infos (version) VALUES (:version)");
                query.bindValue(":version", gBaseVersion);
                mInfos.version = gBaseVersion;

                if(query.exec())
                {
                    qDebug() << "Add some info field success";
                }
                else
                {
                    TLogError("Add some info field failed");
                }
            }
        }
    }

    // Ok, now we have all the necessary information, change the tables if needed
    if (mInfos.version == gBaseVersion)
    {
        // Upgrade to the 1.0
        query.prepare("ALTER TABLE teams ADD COLUMN number INTEGER DEFAULT 0");
        if(query.exec())
        {
            qDebug() << "Upgrade table 'teams' to 1.0 success";
        }
        mInfos.version = gVersion1_0;
        EditInfos();
    }

    if (mInfos.version == gVersion1_0)
    {
        // Upgrade to the 1.1
        Player p;
        p.name = "John";
        p.lastName = "Doe";
        p.comments = "Dummy player";

        if(AddPlayer(p, Player::cDummyPlayer))
        {
            qDebug() << "Upgrade table 'teams' to 1.1 success";
        }
        mInfos.version = gVersion1_1;
        EditInfos();
    }

    if (mInfos.version == gVersion1_1)
    {
        // Upgrade to the 1.2
        query.prepare("ALTER TABLE events ADD COLUMN option INTEGER DEFAULT 0");
        if(query.exec())
        {
            qDebug() << "Upgrade table 'events' to 1.2 success";
            // Then update the type (RoundRobin = 1 is now 0)
            query.prepare("UPDATE TABLE events SET type = 0 WHERE type = 1");
            if(query.exec())
            {
                qDebug() << "Upgrade table 'events' to 1.2 success";
            }
        }

        mInfos.version = gVersion1_2;
        EditInfos();
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
            // retrieve the table name
            QRegularExpression re("EXISTS (\\w+) \\(");
            QRegularExpressionMatch match = re.match(gTables[i]);
            if (match.hasMatch())
            {
                QString tableName = match.captured(1);
                // Test if table exists
                QString testTable = "SELECT name FROM sqlite_master WHERE type='table' AND name='" + tableName + "'";

                QSqlQuery testQuery(testTable, mDb);
                if (testQuery.exec())
                {
                    if (testQuery.next())
                    {
                        qDebug() << "Found table: " << tableName;
                    }
                    else
                    {
                        QSqlQuery query(gTables[i], mDb);
                        if(!query.exec())
                        {
                            qDebug() << "Create table failed: " << query.lastError();
                        }
                        else
                        {
                            qDebug() << "Created table: " << tableName;
                        }
                    }
                }
                else
                {
                    qDebug() << "Cannot search for table: " << tableName;
                }
            }
        }

        Upgrade();
        UpdatePlayerList();
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
        if ((p.lastName.toLower() == player.lastName.toLower()) &&
            (p.name.toLower() == player.name.toLower()))
        {
            found = true;
            break;
        }
    }
    return found;
}

bool DbManager::DeletePlayer(int id)
{
    bool success = false;

    QSqlQuery query(mDb);
    query.prepare("DELETE FROM players WHERE id= :id");
    query.bindValue(":id", id);

    if(query.exec())
    {
        qDebug() << "Delete player success";
        success = true;
        UpdatePlayerList();
    }
    else
    {
        TLogError("Delete player failed: " + query.lastError().text().toStdString());
    }

    return success;
}

void DbManager::UpdatePlayerList()
{
    QSqlQuery query("SELECT * FROM players", mDb);
    mPlayers.clear();

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

        if (player.id != Player::cDummyPlayer)
        {
            mPlayers.append(player);
        }
    }
}

bool DbManager::FindPlayer(int id, Player &player) const
{
    return Player::Find(mPlayers, id, player);
}

bool DbManager::AddPlayer(const Player& player, int id)
{
    bool success = false;

    if (IsValid(player))
    {
        QSqlQuery queryAdd(mDb);
        QString cmd = "INSERT INTO players (uuid, name, last_name, nick_name, email, mobile_phone, home_phone, birth_date, road, post_code, city, membership, comments, state";

        if (id >= 0)
        {
            cmd += ", id";
        }

        cmd += ", document) ";

        cmd += "VALUES (:uuid, :name, :last_name, :nick_name, :email, :mobile_phone, :home_phone, :birth_date, :road, :post_code, :city, :membership, :comments, :state";

        if (id >= 0)
        {
            cmd += ", :id";
        }
        cmd += ", :document)";

        queryAdd.prepare(cmd);

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

        if (id >= 0)
        {
            queryAdd.bindValue(":id", id);
        }

        queryAdd.bindValue(":document", player.document);

        if(queryAdd.exec())
        {
            qDebug() << "Add player success with id: " << id;
            success = true;
            UpdatePlayerList();
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
            UpdatePlayerList();
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
    queryAdd.prepare("INSERT INTO events (year, date, title, state, type, option, document) VALUES (:year, :date, :title, :state, :type, :option, :document)");

    queryAdd.bindValue(":year", event.year);
    queryAdd.bindValue(":date", event.date.toString(Qt::ISODate));
    queryAdd.bindValue(":title", event.title);
    queryAdd.bindValue(":state", event.state);
    queryAdd.bindValue(":type", event.type);
    queryAdd.bindValue(":option", event.option);
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

    queryEdit.prepare("UPDATE events SET year = :year, date = :date, title = :title, state = :state, type = :type, option = :option, document = :document WHERE id = :id");

    queryEdit.bindValue(":id", event.id);
    queryEdit.bindValue(":year", event.year);
    queryEdit.bindValue(":date", event.date.toString(Qt::ISODate));
    queryEdit.bindValue(":title", event.title);
    queryEdit.bindValue(":state", event.state);
    queryEdit.bindValue(":type", event.type);
    queryEdit.bindValue(":option", event.option);
    queryEdit.bindValue(":document", event.document);

    if(queryEdit.exec())
    {
        qDebug() << "Edit event success";
        success = true;
    }
    else
    {
        TLogError("Edit event failed: " + queryEdit.lastError().text().toStdString());
    }
    return success;
}

Event DbManager::GetEvent(int id)
{
    Event event;

    QSqlQuery query(mDb);
    query.prepare("SELECT * FROM events WHERE id = :id");
    query.bindValue(":id", id);

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
            event.option = query.value("option").toInt();
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
            event.option = query.value("option").toInt();
            event.document = query.value("document").toString();
            result.append(event);
        }
    }
    return result;
}

bool DbManager::DeleteEvent(int id)
{
    bool success = false;

    QSqlQuery query(mDb);
    query.prepare("DELETE FROM events WHERE id= :id");
    query.bindValue(":id", id);

    if(query.exec())
    {
        qDebug() << "Delete event success";
        success = true;
    }
    else
    {
        TLogError("Delete event failed: " + query.lastError().text().toStdString());
    }

    return success;
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

QList<Game> DbManager::GetGamesByEventId(int event_id) const
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
            game.FillFrom(query);
            result.append(game);
        }
    }
    return result;
}

Game DbManager::GetGameById(int game_id) const
{
    QSqlQuery query(mDb);
    query.prepare("SELECT * FROM games WHERE id = :game_id");
    query.bindValue(":game_id", game_id);

    Game result;

    if(query.exec())
    {
        if (query.next())
        {
            result.FillFrom(query);
        }
    }
    return result;
}

QList<Game> DbManager::GetGamesByTeamId(int teamId)
{
    QSqlQuery query(mDb);
    query.prepare("SELECT * FROM games WHERE team1_id = :teamId OR team2_id = :teamId");
    query.bindValue(":teamId", teamId);

    QList<Game> result;

    if(query.exec())
    {
        while (query.next())
        {
            Game game;
            game.FillFrom(query);
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

bool DbManager::DeleteGame(int id)
{
    bool success = false;

    QSqlQuery queryDel(mDb);
    queryDel.prepare("DELETE FROM games WHERE id= :id");
    queryDel.bindValue(":id", id);

    if(queryDel.exec())
    {
        qDebug() << "Delete game success";
        success = true;
    }
    else
    {
        TLogError("Delete game failed: " + queryDel.lastError().text().toStdString());
    }

    return success;
}

bool DbManager::DeleteGameByEventId(int eventId)
{
    bool success = false;

    QSqlQuery queryAdd(mDb);
    queryAdd.prepare("DELETE FROM games WHERE event_id= :event_id");
    queryAdd.bindValue(":event_id", eventId);

    if(queryAdd.exec())
    {
        qDebug() << "Delete game success";
        success = true;
    }
    else
    {
        TLogError("Delete game failed: " + queryAdd.lastError().text().toStdString());
    }

    return success;
}

QList<Reward> DbManager::GetRewardsForTeam(int team_id)
{
    QSqlQuery query(mDb);
    query.prepare("SELECT * FROM rewards WHERE team_id = :teamId");
    query.bindValue(":teamId", team_id);

    QList<Reward> result;

    if(query.exec())
    {
        while (query.next())
        {
            Reward reward;
            reward.FillFrom(query);
            result.append(reward);
        }
    }
    return result;
}

bool DbManager::AddReward(const Reward &reward)
{
    bool success = false;

    QSqlQuery queryAdd(mDb);
    queryAdd.prepare("INSERT INTO rewards (event_id, team_id, total, comment, state, document) "
                     "VALUES (:event_id, :team_id, :total, :comment, :state, :document)");
    queryAdd.bindValue(":event_id", reward.eventId);
    queryAdd.bindValue(":team_id", reward.teamId);
    queryAdd.bindValue(":total", reward.total);
    queryAdd.bindValue(":comment", reward.comment);
    queryAdd.bindValue(":state", reward.state);
    queryAdd.bindValue(":document", reward.document);

    if(queryAdd.exec())
    {
        qDebug() << "Add reward success";
        success = true;
    }
    else
    {
        TLogError("Add reward failed: " + queryAdd.lastError().text().toStdString());
        success = false;
    }

    return success;
}

bool DbManager::DeleteReward(int id)
{
    bool success = false;

    QSqlQuery queryDel(mDb);
    queryDel.prepare("DELETE FROM rewards WHERE id= :id");
    queryDel.bindValue(":id", id);

    if(queryDel.exec())
    {
        qDebug() << "Delete reward success";
        success = true;
    }
    else
    {
        TLogError("Delete reward failed: " + queryDel.lastError().text().toStdString());
    }

    return success;
}


bool DbManager::AddTeam(const Team &team)
{
    bool success = false;

    QSqlQuery queryAdd(mDb);
    queryAdd.prepare("INSERT INTO teams (event_id, team_name, player1_id, player2_id, player3_id, state, document, number) "
                     "VALUES (:event_id, :team_name, :player1_id, :player2_id, :player3_id, :state, :document, :number)");
    queryAdd.bindValue(":event_id", team.eventId);
    queryAdd.bindValue(":team_name", team.teamName);
    queryAdd.bindValue(":player1_id", team.player1Id);
    queryAdd.bindValue(":player2_id", team.player2Id);
    queryAdd.bindValue(":player3_id", team.player3Id);
    queryAdd.bindValue(":state", team.state);
    queryAdd.bindValue(":document", team.document);
    queryAdd.bindValue(":number", team.number);

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

QList<Team> DbManager::GetTeams(int eventId) const
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
            team.FillFrom(query);

            if (team.teamName == "")
            {
                // Create a team name
                Player p1, p2;

                bool valid = FindPlayer(team.player1Id, p1);
                valid = valid && FindPlayer(team.player2Id, p2);

                if (valid)
                {
                    CreateName(team, p1, p2);
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

QList<Team> DbManager::GetTeamsByPlayerId(int playerId)
{
    QSqlQuery query(mDb);
    query.prepare("SELECT * FROM teams WHERE player1_id = :playerId OR player2_id = :playerId");
    query.bindValue(":playerId", playerId);

    QList<Team> result;

    if(query.exec())
    {
        while (query.next())
        {
            Team team;
            team.FillFrom(query);
            result.append(team);
        }
    }
    return result;
}


void DbManager::CreateName(Team &team, const Player &p1, const Player &p2)
{
    team.teamName = p1.name + " " + p1.lastName.left(3) + QString(". / ") + p2.name + " " + p2.lastName.left(3) + QString(".");
}

bool DbManager::EditTeam(const Team &team)
{
    bool success = false;

    QSqlQuery queryEdit(mDb);

    queryEdit.prepare("UPDATE teams SET event_id = :event_id, team_name = :team_name, player1_id = :player1_id, "
                      "player2_id = :player2_id, player3_id = :player3_id, state = :state, document = :document, number = :number WHERE id = :id");

    queryEdit.bindValue(":id", team.id);
    queryEdit.bindValue(":event_id", team.eventId);
    queryEdit.bindValue(":team_name", team.teamName);
    queryEdit.bindValue(":player1_id", team.player1Id);
    queryEdit.bindValue(":player2_id", team.player2Id);
    queryEdit.bindValue(":player3_id", team.player3Id);
    queryEdit.bindValue(":state", team.state);
    queryEdit.bindValue(":document", team.document);
    queryEdit.bindValue(":number", team.number);

    if(queryEdit.exec())
    {
        qDebug() << "Edit team success";
        success = true;
    }
    else
    {
        TLogError("Edit team failed: " + queryEdit.lastError().text().toStdString());
    }
    return success;
}

bool DbManager::DeleteTeam(int id)
{
    bool success = false;

    QSqlQuery queryAdd(mDb);
    queryAdd.prepare("DELETE FROM teams WHERE id = :id");
    queryAdd.bindValue(":id", id);

    if(queryAdd.exec())
    {
        qDebug() << "Delete team success";
        success = true;
    }
    else
    {
        TLogError("Delete team failed: " + queryAdd.lastError().text().toStdString());
    }

    return success;
}

bool DbManager::DeleteTeamByEventId(int eventId)
{
    bool success = false;

    QSqlQuery queryAdd(mDb);
    queryAdd.prepare("DELETE FROM teams WHERE event_id= :event_id");
    queryAdd.bindValue(":event_id", eventId);

    if(queryAdd.exec())
    {
        qDebug() << "Delete team success";
        success = true;
    }
    else
    {
        TLogError("Delete team failed: " + queryAdd.lastError().text().toStdString());
    }

    return success;
}


#include "DbManager.h"
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
    tables << Match::Table();
    tables << Team::Table();
    tables << Round::Table();

    return tables;
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
        mPlayersModel->setHeaderData(5, Qt::Horizontal, tr("Date de naissance"));
        mPlayersModel->setHeaderData(6, Qt::Horizontal, tr("Rue"));
        mPlayersModel->setHeaderData(7, Qt::Horizontal, tr("Code postal"));
        mPlayersModel->setHeaderData(8, Qt::Horizontal, tr("Ville"));
        mPlayersModel->setHeaderData(9, Qt::Horizontal, tr("Licences"));
        mPlayersModel->setHeaderData(10, Qt::Horizontal, tr("Commentaires"));

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
        player.birthDate = query.value("birth_date").toDate();
        player.nickName = query.value("nick_name").toString();
        player.postCode = query.value("post_code").toInt();
        player.city = query.value("city").toString();
        player.membership = query.value("membership").toString();
        player.comments = query.value("comments").toString();

        result.append(player);
    }
    return result;
}

bool DbManager::FindPlayer(int id, Player &player)
{
    bool found = false;

    foreach (Player p, mPlayers)
    {
        if (p.id == id)
        {
            player = p;
            found = true;
            break;
        }
    }
    return found;
}

QStringList DbManager::GetSeasons()
{
    QSqlQuery query("SELECT DISTINCT year FROM matches", mDb);
    QStringList result;

    while (query.next())
    {
        result.append(query.value(0).toString());
    }
    return result;
}

QList<Team> DbManager::GetTeams(int matchId)
{
    QSqlQuery query(mDb);
    query.prepare("SELECT * FROM teams WHERE match_id = :match_id");
    query.bindValue(":match_id", matchId);

    QList<Team> result;

    if(query.exec())
    {
        while (query.next())
        {
            Team team;
            team.id = query.value("id").toInt();
            team.matchId = query.value("match_id").toInt();
            team.teamName = query.value("team_name").toString();
            team.player1Id = query.value("player1_id").toInt();
            team.player2Id = query.value("player2_id").toInt();
            team.player3Id = query.value("player3_id").toInt();

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


int DbManager::GetMatch(const QString &date)
{
    int match_id = -1;

    QSqlQuery query(mDb);
    query.prepare("SELECT id FROM matches WHERE date = :date");
    query.bindValue(":date", date);

    if(query.exec())
    {
        while (query.next())
        {
            match_id = query.value(0).toInt();
        }
    }

    return match_id;
}

QList<Match> DbManager::GetMatches(int year)
{
    QSqlQuery query(mDb);
    query.prepare("SELECT * FROM matches WHERE year = :year");
    query.bindValue(":year", year);

    QList<Match> result;

    if(query.exec())
    {
        while (query.next())
        {
            Match match;
            match.id = query.value("id").toInt();
            match.date = query.value("date").toDate();
            match.year = query.value("year").toInt();
            match.state = query.value("state").toInt();
            result.append(match);
        }
    }
    return result;
}

QStringList DbManager::GetCities(int postCode)
{
    (void) postCode; // FIXME
    return QStringList();
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

bool DbManager::IsValid(const Player& player)
{
    // FIXME: check the validity of the player parameters
    (void) player;
    return true;
}

bool DbManager::AddMatch(const Match& match)
{
    bool success = false;

    QSqlQuery queryAdd(mDb);
    queryAdd.prepare("INSERT INTO matches (date, year) VALUES (:date, :year)");
    queryAdd.bindValue(":date", match.date);
    queryAdd.bindValue(":year", match.year);

    if(queryAdd.exec())
    {
        qDebug() << "Add match success";
        success = true;
    }
    else
    {
        qDebug() << "Add match failed: " << queryAdd.lastError();
    }

    return success;
}

bool DbManager::AddTeam(const Team &team)
{
    bool success = false;

    QSqlQuery queryAdd(mDb);
    queryAdd.prepare("INSERT INTO teams (match_id, team_name, player1_id, player2_id, player3_id) VALUES (:match_id, :team_name, :player1_id, :player2_id, :player3_id)");
    queryAdd.bindValue(":match_id", team.matchId);
    queryAdd.bindValue(":team_name", team.teamName);
    queryAdd.bindValue(":player1_id", team.player1Id);
    queryAdd.bindValue(":player2_id", team.player2Id);
    queryAdd.bindValue(":player3_id", team.player3Id);

    if(queryAdd.exec())
    {
        qDebug() << "Add team success";
        success = true;
    }
    else
    {
        qDebug() << "Add team failed: " << queryAdd.lastError();
    }

    return success;
}

bool DbManager::EditPlayer(const Player& player)
{
    bool success = false;

    if (IsValid(player))
    {
        QSqlQuery queryEdit(mDb);
        queryEdit.prepare("UPDATE players SET name = :name, "
                         "SET last_name = :last_name, SET nick_name = :nick_name, "
                         "SET birth_date = :birth_date, SET road = :road, SET post_code = :post_code, "
                         "SET city = :city, SET membership = :membership, SET comments = :comments "
                         "WHERE id = :id");

        queryEdit.bindValue(":id", player.id);
        queryEdit.bindValue(":name", player.name);
        queryEdit.bindValue(":last_name", player.lastName);
        queryEdit.bindValue(":nick_name", player.nickName);
        queryEdit.bindValue(":birth_date", player.birthDate.toString());
        queryEdit.bindValue(":road", player.road);
        queryEdit.bindValue(":post_code", player.postCode);
        queryEdit.bindValue(":city", player.city);
        queryEdit.bindValue(":membership", player.membership);
        queryEdit.bindValue(":comments", player.comments);

        if(queryEdit.exec())
        {
            qDebug() << "Edit player success";
            success = true;
            mPlayersModel->select();

            mPlayers = UpdatePlayerList();
        }
        else
        {
            qDebug() << "Edit player failed: " << queryEdit.lastError();
        }
    }
    else
    {
        qDebug() << "Edit player failed: name cannot be empty";
    }

    return success;
}

bool DbManager::AddPlayer(const Player& player)
{
    bool success = false;

    if (IsValid(player))
    {
        QSqlQuery queryAdd(mDb);
        queryAdd.prepare("INSERT INTO players (uuid, name, last_name, nick_name, birth_date, road, post_code, city, membership, comments) "
                         "VALUES (:uuid, :name, :last_name, :nick_name, :birth_date, :road, :post_code, :city, :membership, :comments)");
        queryAdd.bindValue(":uuid", QUuid::createUuid().toString());
        queryAdd.bindValue(":name", player.name);
        queryAdd.bindValue(":last_name", player.lastName);
        queryAdd.bindValue(":nick_name", player.nickName);
        queryAdd.bindValue(":birth_date", player.birthDate);
        queryAdd.bindValue(":road", player.road);
        queryAdd.bindValue(":post_code", player.postCode);
        queryAdd.bindValue(":city", player.city);
        queryAdd.bindValue(":membership", "");
        queryAdd.bindValue(":comments", player.comments);

        if(queryAdd.exec())
        {
            qDebug() << "Add player success";
            success = true;
            mPlayersModel->select();

            mPlayers = UpdatePlayerList();
        }
        else
        {
            qDebug() << "add player failed: " << queryAdd.lastError();
        }
    }
    else
    {
        qDebug() << "add player failed: name cannot be empty";
    }

    return success;
}

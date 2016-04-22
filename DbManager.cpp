#include "DbManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QUuid>

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
        mPlayersModel->setEditStrategy(QSqlTableModel::OnRowChange);
        mPlayersModel->select();
        mPlayersModel->setHeaderData(0, Qt::Horizontal, tr("Id"));
        mPlayersModel->setHeaderData(1, Qt::Horizontal, tr("UUID"));
        mPlayersModel->setHeaderData(2, Qt::Horizontal, tr("Prénom"));
        mPlayersModel->setHeaderData(3, Qt::Horizontal, tr("Nom"));
        mPlayersModel->setHeaderData(4, Qt::Horizontal, tr("Date de naissance"));
        mPlayersModel->setHeaderData(5, Qt::Horizontal, tr("Rue"));
        mPlayersModel->setHeaderData(6, Qt::Horizontal, tr("Code postal"));
        mPlayersModel->setHeaderData(7, Qt::Horizontal, tr("Ville"));
        mPlayersModel->setHeaderData(8, Qt::Horizontal, tr("Licences"));
        mPlayersModel->setHeaderData(9, Qt::Horizontal, tr("Commentaires"));
    }
    else
    {
       qDebug() << "Error: connection with database fail";
    }

    // Initialize the Cities DB
    mCities = QSqlDatabase::addDatabase("QSQLITE", "cities");
    mCities.setDatabaseName("~/Tanca/villes.db"); // FIXME: use the executable path
}

QList<Player> DbManager::GetPlayerList()
{
    QSqlQuery query("SELECT * FROM players", mDb);
    QList<Player> result;

    while (query.next())
    {
        Player player;

        player.id = query.value("id").toInt();
        player.name = query.value("name").toString();
        player.lastName = query.value("last_name").toString();

        result.append(player);
    }
    return result;
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

QStringList DbManager::GetMatches(int year)
{
    QSqlQuery query(mDb);
    query.prepare("SELECT date FROM matches WHERE year = :year");
    query.bindValue(":year", year);

    QStringList result;

    if(query.exec())
    {
        while (query.next())
        {
            result.append(query.value(0).toString());
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

bool DbManager::IsValid(const Player& person)
{
    // FIXME: check the validity of the person parameters
    (void) person;
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

bool DbManager::AddPlayer(const Player& player)
{
    bool success = false;

    if (IsValid(player))
    {
        QSqlQuery queryAdd(mDb);
        queryAdd.prepare("INSERT INTO players (uuid, name, last_name, birth_date, road, post_code, city, membership, comments) "
                         "VALUES (:uuid, :name, :last_name, :birth_date, :road, :post_code, :city, :membership, :comments)");
        queryAdd.bindValue(":uuid", QUuid::createUuid().toString());
        queryAdd.bindValue(":name", player.name);
        queryAdd.bindValue(":last_name", player.lastName);
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
        }
        else
        {
            qDebug() << "add person failed: " << queryAdd.lastError();
        }
    }
    else
    {
        qDebug() << "add person failed: name cannot be empty";
    }

    return success;
}

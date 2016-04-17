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
            QSqlQuery query(gTables[i]);
            if(!query.exec())
            {
                qDebug() << "Create table failed: " << query.lastError();
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

bool DbManager::AddPerson(const Player& person)
{
    bool success = false;

    if (IsValid(person))
    {
        QSqlQuery queryAdd(mDb);
        queryAdd.prepare("INSERT INTO players (uuid, name, last_name, birth_date, road, post_code, city, membership, comments) "
                         "VALUES (:uuid, :name, :last_name, :birth_date, :road, :post_code, :city, :membership, :comments)");
        queryAdd.bindValue(":uuid", QUuid::createUuid().toString());
        queryAdd.bindValue(":name", person.name);
        queryAdd.bindValue(":last_name", person.lastName);
        queryAdd.bindValue(":birth_date", person.birthDate);
        queryAdd.bindValue(":road", person.road);
        queryAdd.bindValue(":post_code", person.postCode);
        queryAdd.bindValue(":city", person.city);
        queryAdd.bindValue(":membership", "");
        queryAdd.bindValue(":comments", person.comments);

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

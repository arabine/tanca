#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QtCore>
#include <QSqlTableModel>

struct Person
{
    int id;
    QString uuid;
    QString name;
    QString lastName;
    QDate birthDate;
    QString road;
    int postCode;
    QString city;
    QString membership;    ///< JSON format, season start on 01/09 of each year.
    QString comments;
};

struct Round
{
    int id;
    QString date;
};

struct Team
{
    int id;
    int roundId;
    QString player1Uuid;
    QString player2Uuid;
    QString player3Uuid;
};

struct Match
{
    int id;
    int team1Id;
    int team2Id;
    int scoreTeam1;
    int scoreTeam2;
};

class ICities
{
public:
    virtual QStringList GetCities(int postCode) = 0;
};


class DbManager : public QObject, public ICities
{

public:
    /**
     * @brief Constructor
     *
     * Constructor sets up connection with db and opens it
     * @param path - absolute path to db file
     */
    DbManager(const QString& path);
    ~DbManager();

    static bool IsValid(const Person &person);
    bool AddPerson(const Person &person);
    void Initialize();
    QSqlTableModel *GetPlayersModel() { return mPlayersModel; }

    // From ICities
    virtual QStringList GetCities(int postCode);

private:
    QSqlDatabase mDb;
    QSqlTableModel *mPlayersModel;

    QSqlDatabase mCities;
};

#endif // DBMANAGER_H

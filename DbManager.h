#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QtCore>
#include <QSqlTableModel>

struct Player
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

    static QString Table() {
        return "CREATE TABLE IF NOT EXISTS players (id INTEGER PRIMARY KEY, uuid TEXT, name TEXT, last_name TEXT, birth_date TEXT, road TEXT, post_code INTEGER, city TEXT, membership TEXT, comments TEXT);";
    }
};

struct Match
{
    int id;
    int year;
    QString date; // full date

    static QString Table() {
        return "CREATE TABLE IF NOT EXISTS matches (id INTEGER PRIMARY KEY, date TEXT, year INTEGER);";
    }
};

struct Team
{
    int id;
    int roundId;
    int player1Id;
    int player2Id;
    int player3Id;

    static QString Table() {
        return "CREATE TABLE IF NOT EXISTS teams (id INTEGER PRIMARY KEY, round_id INTEGER, player1_id INTEGER, player2_id INTEGER, player3_id INTEGER);";
    }
};

struct Round
{
    int id;
    int team1Id;
    int team2Id;
    int team1Score;
    int team2Score;

    static QString Table() {
        return "CREATE TABLE IF NOT EXISTS rounds (id INTEGER PRIMARY KEY, team1_id INTEGER, team2_id INTEGER, team1_score INTEGER, team2_score INTEGER);";
    }
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

    void Initialize();

    static bool IsValid(const Player &person);
    bool AddPlayer(const Player &player);
    bool AddMatch(const Match &match);
    bool AddTeam(const Team &team);

    QSqlTableModel *GetPlayersModel() { return mPlayersModel; }

    QStringList GetSeasons();
    QStringList GetTeams(QString match_date);

    // From ICities
    virtual QStringList GetCities(int postCode);

    QStringList GetMatches(int year);
    QList<Player> GetPlayerList();
private:
    QSqlDatabase mDb;
    QSqlDatabase mCities;

    QSqlTableModel *mPlayersModel;
};

#endif // DBMANAGER_H

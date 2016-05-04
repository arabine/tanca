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
    QString nickName;
    QDate birthDate;
    QString road;
    int postCode;
    QString city;
    QString membership;    ///< JSON format, season start on 01/09 of each year.
    QString comments;

    Player()
    {
        Reset();
    }

    void Reset()
    {
        id = -1;
        postCode = -1;
        uuid = "";
        name = "";
        lastName = "";
        nickName = "";
        birthDate = QDate::currentDate();
        road = "";
        city = "";
        membership = "";
        comments = "";
    }

    static QString Table() {
        return "CREATE TABLE IF NOT EXISTS players (id INTEGER PRIMARY KEY, uuid TEXT, name TEXT, last_name TEXT, nick_name TEXT, birth_date TEXT, road TEXT, post_code INTEGER, city TEXT, membership TEXT, comments TEXT);";
    }
};

struct Match
{
    int id;
    int year;
    QDate date; // full date
    int state;

    static const int cNotStarted = -1;

    Match()
        : id(-1)
        , year(-1)
        , state(cNotStarted)
    {
        date = QDate::currentDate();
    }

    static QString Table() {
        return "CREATE TABLE IF NOT EXISTS matches (id INTEGER PRIMARY KEY, date TEXT, year INTEGER, state INTEGER);";
    }
};

struct Team
{
    int id;
    int matchId;
    QString teamName;
    int player1Id;
    int player2Id;
    int player3Id;

    // Not part of the database
    QList<int> opponents; // opponent team id (index is the round number)

    Team()
        : id(-1)
        , matchId(-1)
        , player1Id(-1)
        , player2Id(-1)
        , player3Id(-1)
    {

    }

    static QString Table() {
        return "CREATE TABLE IF NOT EXISTS teams (id INTEGER PRIMARY KEY, match_id INTEGER, team_name TEXT, player1_id INTEGER, player2_id INTEGER, player3_id INTEGER);";
    }
};

struct Round
{
    int id;
    int matchId; // Also include te match id to avoid too complex DB queries

    int team1Id;
    int team2Id;
    int team1Score;
    int team2Score;

    Round()
        : id(-1)
        , matchId(-1)
        , team1Id(-1)
        , team2Id(-1)
        , team1Score(-1)
        , team2Score(-1)
    {

    }

    static QString Table() {
        return "CREATE TABLE IF NOT EXISTS rounds (id INTEGER PRIMARY KEY, match_id INTEGER, team1_id INTEGER, team2_id INTEGER, team1_score INTEGER, team2_score INTEGER);";
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

    static bool IsValid(const Player &player);
    bool AddPlayer(const Player &player);
    bool AddMatch(const Match &match);
    bool AddTeam(const Team &team);

    QSqlTableModel *GetPlayersModel() { return mPlayersModel; }

    int GetMatch(const QString &date);
    QStringList GetSeasons();
    QList<Team> GetTeams(int matchId);

    // From ICities
    virtual QStringList GetCities(int postCode);

    QList<Match> GetMatches(int year);

    bool EditPlayer(const Player &player);
    bool FindPlayer(int id, Player &player);
    QList<Player> &GetPlayerList();

private:
    QSqlDatabase mDb;
    QSqlDatabase mCities;

    QList<Player> mPlayers; // Cached player list
    QSqlTableModel *mPlayersModel;

    QList<Player> UpdatePlayerList();
};

#endif // DBMANAGER_H

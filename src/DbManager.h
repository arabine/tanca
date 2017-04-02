#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QtCore>
#include <QSqlTableModel>
#include <QSqlQuery>

struct Player
{
    static const int cDummyPlayer = 999999999;

    int id;
    QString uuid;
    QString name;
    QString lastName;
    QString nickName;
    QString email;
    QString mobilePhone;
    QString homePhone;
    QDate birthDate;
    QString road;
    int postCode;
    QString city;
    QString membership;    ///< semi_column separated list, season start on 01/09 of each year.
    QString comments;
    int state;
    QString document; // JSON document, reserved to store anything in the future

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
        email = "";
        mobilePhone = "";
        homePhone = "";
        birthDate = QDate::currentDate();
        road = "";
        city = "";
        membership = "";
        comments = "";
        state = -1;
        document = "";
    }

    QString FullName()
    {
        return name + " " + lastName;
    }

    static bool Find(const QList<Player> &players, const int id, Player &player)
    {
        bool found = false;

        foreach (Player p, players)
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

    static bool Index(const QList<Player> &players, const int id, int &index)
    {
        bool found = false;

        index = 0;
        foreach (Player p, players)
        {
            if (p.id == id)
            {
                found = true;
                break;
            }
            index++;
        }
        return found;
    }

    static QString Table() {
        return "CREATE TABLE IF NOT EXISTS players (id INTEGER PRIMARY KEY AUTOINCREMENT, uuid TEXT, name TEXT, last_name TEXT, nick_name TEXT, "
                "email TEXT, mobile_phone TEXT, home_phone TEXT, birth_date TEXT, road TEXT, post_code INTEGER, city TEXT, "
                "membership TEXT, comments TEXT, state INTEGER, document TEXT);";
    }
};

struct Event
{
    int id;
    int year;
    QDateTime date;
    QString title;
    int state;
    int type;
    QString document;  // JSON document, reserved to store anything in the future

    // State
    static const int cNotStarted = 0;
    static const int cStarted = 1;
    static const int cCanceled = 2;

    // Type
    static const int cClubContest = 0;
    static const int cSwissRounds = 1;

    Event()
        : id(-1)
        , year(-1)
        , state(cNotStarted)
        , type(cClubContest)
    {
        date = QDateTime::currentDateTime();
    }

    bool IsValid()
    {
        return (id != -1);
    }

    QString StateToString()
    {
        if (state == cNotStarted)
        {
            return QObject::tr("Non démarré");
        }
        else if (state == cStarted)
        {
            return QObject::tr("En cours");
        }
        else if (state == cCanceled)
        {
            return QObject::tr("Annulé");
        }
        else
        {
            return QObject::tr("");
        }
    }

    QString TypeToString()
    {
        if (type == cClubContest)
        {
            return QObject::tr("Championnat du club");
        }
        else if (type == cSwissRounds)
        {
            return QObject::tr("Tournoi type Suisse");
        }
        else
        {
            return QObject::tr("");
        }
    }

    static QString Table() {
        return "CREATE TABLE IF NOT EXISTS events (id INTEGER PRIMARY KEY AUTOINCREMENT, date TEXT, year INTEGER, title TEXT, state INTEGER, type INTEGER, document TEXT);";
    }
};

struct Team
{
    // Version 0.0
    int id;
    int eventId;
    QString teamName;
    int player1Id;
    int player2Id;
    int player3Id;
    int state;
    QString document;  // JSON document, reserved to store anything in the future

    // Version 1.0
    int number;

    Team()
        : id(-1)
        , eventId(-1)
        , player1Id(-1)
        , player2Id(-1)
        , player3Id(-1)
        , state(-1)
    {

    }

    void FillFrom(const QSqlQuery &query)
    {
        id = query.value("id").toInt();
        eventId = query.value("event_id").toInt();
        teamName = query.value("team_name").toString();
        player1Id = query.value("player1_id").toInt();
        player2Id = query.value("player2_id").toInt();
        player3Id = query.value("player3_id").toInt();
        state = query.value("state").toInt();
        document = query.value("document").toString();
        number = query.value("number").toInt();
    }

    /**
     * @brief Find a team id within a list
     * @param teams
     * @param id
     * @param team
     * @return
     */
    static bool Find(const QList<Team> &teams, const int id, Team &team)
    {
        bool found = false;
        for (int i = 0; i < teams.size(); i++)
        {
            if (teams[i].id == id)
            {
                found = true;
                team = teams[i];
                break;
            }
        }

        return found;
    }

    /**
     * @brief Find a team index in the list by its id
     * @param players
     * @param id
     * @param index
     * @return
     */
    static bool Index(const QList<Team> &teams, const int id, int &index)
    {
        bool found = false;

        index = 0;
        foreach (Team t, teams)
        {
            if (t.id == id)
            {
                found = true;
                break;
            }
            index++;
        }
        return found;
    }

    static QString Table() {
        return "CREATE TABLE IF NOT EXISTS teams (id INTEGER PRIMARY KEY AUTOINCREMENT, event_id INTEGER, team_name TEXT, player1_id INTEGER, "
                "player2_id INTEGER, player3_id INTEGER, state INTEGER, document TEXT);";
    }
};

struct Game
{
    int id;
    int eventId; // Also include te match id to avoid too complex DB queries
    int turn;       // Round turn (1 turn, 2nd turn ...)
    int team1Id;
    int team2Id;
    int team1Score;
    int team2Score;
    int state;
    QString document;  // JSON document, reserved to store anything in the future

    void FillFrom(const QSqlQuery &query)
    {
        id = query.value("id").toInt();
        eventId = query.value("event_id").toInt();
        turn = query.value("turn").toInt();
        team1Id = query.value("team1_id").toInt();
        team2Id = query.value("team2_id").toInt();
        team1Score = query.value("team1_score").toInt();
        team2Score = query.value("team2_score").toInt();
        state = query.value("state").toInt();
        document = query.value("document").toString();
    }


    bool IsPlayed()
    {
        bool active = false;
        if ((team1Score != -1) && (team2Score != -1))
        {
            if ((team1Score + team2Score) > 0)
            {
                active = true;
            }
        }
        return active;
    }

    void Cancel()
    {
        team1Score = -1;
        team2Score = -1;
    }

    Game()
        : id(-1)
        , eventId(-1)
        , turn(-1)
        , team1Id(-1)
        , team2Id(-1)
        , team1Score(-1)
        , team2Score(-1)
        , state(-1)
    {

    }

    static QString Table() {
        return "CREATE TABLE IF NOT EXISTS games (id INTEGER PRIMARY KEY AUTOINCREMENT, event_id INTEGER, turn INTEGER, team1_id INTEGER, "
                "team2_id INTEGER, team1_score INTEGER, team2_score INTEGER, state INTEGER, document TEXT);";
    }
};

struct Infos
{
    QString version;

    static QString Table() {
        return "CREATE TABLE IF NOT EXISTS infos (version TEXT);";
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

    // Player management
    static bool IsValid(const Player &player);
    bool AddPlayer(const Player &player, int id = -1); // you may specify an ID if you want
    bool EditPlayer(const Player &player);
    bool FindPlayer(int id, Player &player);
    QList<Player> &GetPlayerList();
    bool PlayerExists(const Player &player) const;
    bool DeletePlayer(int id);

    // Events management
    bool AddEvent(const Event &event);
    Event GetEvent(int id);
    QStringList GetSeasons();
    bool UpdateEventState(const Event &event);
    QList<Event> GetEvents(int year);
    bool EditEvent(const Event &event);
    bool DeleteEvent(int id);

    // Team management
    bool AddTeam(const Team &team);
    QList<Team> GetTeams(int eventId);
    QList<Team> GetTeamsByPlayerId(int playerId);
    bool EditTeam(const Team &team);
    bool DeleteTeam(int id);
    bool DeleteTeamByEventId(int eventId);

    // Game management
    QList<Game> GetGames(int event_id);
    QList<Game> GetGamesByTeamId(int teamId);
    bool AddGames(const QList<Game> &games);
    bool EditGame(const Game &game);
    bool DeleteGame(int id);
    bool DeleteGameByEventId(int eventId);

    // From ICities
    virtual QStringList GetCities(int postCode);

    // Static members
    static void CreateName(Team &team, const Player &p1, const Player &p2);

private:
    QSqlDatabase mDb;
    QSqlDatabase mCities;
    QList<Player> mPlayers; // Cached player list
    Infos mInfos;

    QList<Player> UpdatePlayerList();
    void Upgrade();
    bool EditInfos();
};

#endif // DBMANAGER_H

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
        return "CREATE TABLE IF NOT EXISTS players (id INTEGER PRIMARY KEY, uuid TEXT, name TEXT, last_name TEXT, nick_name TEXT, "
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

    Event()
        : id(-1)
        , year(-1)
        , state(cNotStarted)
        , type(cClubContest)
    {
        date = QDateTime::currentDateTime();
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
        else
        {
            return QObject::tr("");
        }
    }

    static QString Table() {
        return "CREATE TABLE IF NOT EXISTS events (id INTEGER PRIMARY KEY, date TEXT, year INTEGER, title TEXT, state INTEGER, type INTEGER, document TEXT);";
    }
};

struct Team
{
    int id;
    int eventId;
    QString teamName;
    int player1Id;
    int player2Id;
    int player3Id;
    int state;
    QString document;  // JSON document, reserved to store anything in the future

    Team()
        : id(-1)
        , eventId(-1)
        , player1Id(-1)
        , player2Id(-1)
        , player3Id(-1)
        , state(-1)
    {

    }

    // Create a team name using player names
    void CreateName(const QString &p1, const QString &p2)
    {
        teamName = p1 + " / " + p2;
    }


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

    static QString Table() {
        return "CREATE TABLE IF NOT EXISTS teams (id INTEGER PRIMARY KEY, event_id INTEGER, team_name TEXT, player1_id INTEGER, "
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
        return "CREATE TABLE IF NOT EXISTS games (id INTEGER PRIMARY KEY, event_id INTEGER, turn INTEGER, team1_id INTEGER, "
                "team2_id INTEGER, team1_score INTEGER, team2_score INTEGER, state INTEGER, document TEXT);";
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
    bool AddPlayer(const Player &player);
    bool EditPlayer(const Player &player);
    bool FindPlayer(int id, Player &player);
    QList<Player> &GetPlayerList();
    bool PlayerExists(const Player &player) const;

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
    bool EditTeam(const Team &team);
    bool DeleteTeam(int id);
    bool DeleteTeamByEventId(int eventId);

    // Game management
    QList<Game> GetGames(int event_id);
    bool AddGames(const QList<Game> &games);
    bool EditGame(const Game &game);
    bool DeleteGameByEventId(int eventId);

    // From ICities
    virtual QStringList GetCities(int postCode);

private:
    QSqlDatabase mDb;
    QSqlDatabase mCities;
    QList<Player> mPlayers; // Cached player list

    QList<Player> UpdatePlayerList();
};

#endif // DBMANAGER_H

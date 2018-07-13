#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QtCore>
#include <QSqlTableModel>
#include <QSqlQuery>

#include "IDataBase.h"



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

void FillFrom(const QSqlQuery &query)
{
    id = query.value("id").toInt();
    eventId = query.value("event_id").toInt();
    teamId = query.value("team_id").toInt();
    total = query.value("total").toInt();
    comment = query.value("comment").toString();
    state = query.value("state").toInt();
    document = query.value("document").toString();
}


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
    bool FindPlayer(int id, Player &player) const;
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
    QList<Team> GetTeams(int eventId) const;
    QList<Team> GetTeamsByPlayerId(int playerId);
    bool EditTeam(const Team &team);
    bool DeleteTeam(int id);
    bool DeleteTeamByEventId(int eventId);

    // Game management
    QList<Game> GetGamesByEventId(int event_id) const;
    Game GetGameById(int game_id) const;
    QList<Game> GetGamesByTeamId(int teamId);
    bool AddGames(const QList<Game> &games);
    bool EditGame(const Game &game);
    bool DeleteGame(int id);
    bool DeleteGameByEventId(int eventId);

    // Rewards
    QList<Reward> GetRewardsForTeam(int team_id);
    bool AddReward(const Reward &reward);
    bool DeleteReward(int id);

    // From ICities
    virtual QStringList GetCities(int postCode);

    // Static members
    static void CreateName(Team &team, const Player &p1, const Player &p2);
private:
    QSqlDatabase mDb;
    QSqlDatabase mCities;
    QList<Player> mPlayers; // Cached player list
    Infos mInfos;

    void UpdatePlayerList();
    void Upgrade();
    bool EditInfos();
};

#endif // DBMANAGER_H

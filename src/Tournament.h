#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include "DbManager.h"
#include <QObject>
#include <list>

struct Rank
{
public:
    int pointsWon;
    int pointsLost;
    int gamesWon;
    int gamesLost;
    int gamesDraw;
    int pointsOpponents;
    int id;

    QList<int> mGames;

    Rank()
        : pointsWon(0)
        , pointsLost(0)
        , gamesWon(0)
        , gamesLost(0)
        , gamesDraw(0)
        , pointsOpponents(0)
    {

    }

    bool operator==(const Rank &other) const {
        return (id == other.id);
    }

    int Difference();
    void AddPoints(int gameId, int score, int oppScore);
};


class Tournament : public QObject
{
    Q_OBJECT

public:
    Tournament();
    ~Tournament();

    QString ToJsonString(const QList<Game> &games, const QList<Team> &teams);

    void GenerateTeamRanking(const QList<Game> &games, const QList<Team> &teams);
    void GeneratePlayerRanking(const DbManager &mDb, const QList<Event> &events);

    QString BuildRoundRobinRounds(const QList<Team> &tlist, int nbRounds, QList<Game> &games);
    QString BuildSwissRounds(const QList<Game> &games, const QList<Team> &teams, QList<Game> &newRounds);

    void ComputeBuchholz(const QList<Game> &games);

    bool Contains(int id);
    int Find(int id);

    std::string RankingToString();
    static int Generate(int min, int max);
private:
    bool mIsTeam;

    std::vector<Rank> mList;
    std::vector<int> mByeTeams; // list of teams that has a bye for that event



    void Add(int id, int gameId, int score, int opponent);
};

#endif // TOURNAMENT_H

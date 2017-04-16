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

    int Difference() const;
    void AddPoints(int gameId, int score, int oppScore);
};


class Tournament : public QObject
{
    Q_OBJECT

public:
    Tournament();
    ~Tournament();

    void GeneratePlayerRanking(const DbManager &mDb, const QList<Event> &events);
    void GenerateTeamRanking(const QList<Game> &games, const QList<Team> &teams, int maxTurn);

    std::vector<Rank> GetRanking();
    bool GetTeamRank(int id, Rank &outRank);

    QString ToJsonString(const QList<Game> &games, const QList<Team> &teams);
    std::string RankingToString();

    QString BuildRoundRobinRounds(const QList<Team> &tlist, int nbRounds, QList<Game> &games);
    QString BuildSwissRounds(const QList<Game> &games, const QList<Team> &teams, QList<Game> &newRounds);

    static int Generate(int min, int max);
private:
    bool mIsTeam;

    std::vector<Rank> mRanking;
    std::vector<int> mByeTeamIds; // list of teams that has a bye for that event

    void ComputeBuchholz(const QList<Game> &games);
    bool Contains(int id);
    int FindRankIndex(int id);
    void Add(int id, int gameId, int score, int opponent);
    bool HasPlayed(const QList<Game> &games, const Rank &rank, int oppId);
    int FindtUnplayedIndex(const QList<Game> &games, const std::vector<int> &ranking);
    bool AlreadyPlayed(const QList<Game> &games, int p1Id, int p2Id);
    void BuildPairing(const std::vector<int> &ranking,
                      const std::vector<std::vector<int> > &cost,
                      std::vector<std::vector<int> > &pairing,
                      QList<Game> &newRounds);
    void BuildCost(const QList<Game> &games,
                   std::vector<int> &ranking,
                   std::vector<std::vector<int> > &cost_matrix,
                   std::vector<std::vector<int> > &pairing);
};

#endif // TOURNAMENT_H

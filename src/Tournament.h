#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include "DbManager.h"
#include <QObject>

struct Rank
{
public:
    int pointsWon;
    int pointsLost;
    int gamesWon;
    int gamesLost;
    int gamesDraw;
    int pointsOpponents;

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

    int Difference();
    void AddPoints(int gameId, int score, int oppScore);
};


class Tournament : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString rounds READ getRounds NOTIFY roundsChanged)

public:
    Tournament();
    ~Tournament();

    //
    QString getRounds();
    void SetGames(const QList<Game> &games);

    void GenerateTeamRanking(const DbManager &mDb, const QList<Event> &events, const Event &currentEvent);
    void GeneratePlayerRanking(const DbManager &mDb, const QList<Event> &events);

    QList<Game> BuildRoundRobinRounds(const QList<Team> &tlist, int nbRounds);
    QList<Game> BuildSwissRounds(const QList<Game> &previous, const QList<Team> &tlist);

    void ComputeBuchholz(const DbManager &mDb);

   // void Draw() { emit  sigDraw("coucou"); }

signals:
  void roundsChanged();

private:
    bool mIsTeam;
    QString mRounds;

    QMap<int, Rank> mList;  // player or team id, Rank
    void Add(int id, int gameId, int score, int opponent);
};

#endif // TOURNAMENT_H

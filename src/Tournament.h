/*=============================================================================
 * Tanca - Tournament.h
 *=============================================================================
 * Core algorithms to generate round-robin and swiss type tournament rounds
 *=============================================================================
 * Tanca ( https://github.com/belegar/tanca ) - This file is part of Tanca
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony.rabine@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include "IDataBase.h"
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

    std::deque<int> mGames;

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


class Tournament
{

public:
    Tournament();
    ~Tournament();

    void GeneratePlayerRanking(const std::deque<Game> &gameList, const std::deque<Team> &teamList, const std::deque<Event> &events);
    void GenerateTeamRanking(const std::deque<Game> &games, const std::deque<Team> &teams, int maxTurn);

    std::deque<Rank> GetRanking();
    bool GetTeamRank(int id, Rank &outRank);

   // std::string ToJsonString(const std::deque<Game> &games, const std::deque<Team> &teams);
    std::string RankingToString();

    std::string BuildRoundRobinRounds(const std::deque<Team> &tlist, uint32_t nbRounds, std::deque<Game> &games);
    std::string BuildSwissRounds(const std::deque<Game> &games, const std::deque<Team> &teams, std::deque<Game> &newRounds);

    static int Generate(int min, int max);
private:
    bool mIsTeam;

    std::deque<Rank> mRanking;
    std::deque<int> mByeTeamIds; // list of teams that has a bye for that event

    void ComputeBuchholz(const std::deque<Game> &games);
    bool Contains(int id);
    int FindRankIndex(int id);
    void Add(int id, int gameId, int score, int opponent);
    bool HasPlayed(const std::deque<Game> &games, const Rank &rank, int oppId);
    int FindtUnplayedIndex(const std::deque<Game> &games, const std::deque<int> &ranking);
    bool AlreadyPlayed(const std::deque<Game> &games, int p1Id, int p2Id);
    bool BuildPairing(const std::deque<int> &ranking,
                      const std::deque<std::deque<int> > &cost,
                      std::deque<Game> &newRounds);
    void BuildCost(const std::deque<Game> &games,
                   std::deque<int> &ranking,
                   std::deque<std::deque<int> > &cost_matrix);
};

#endif // TOURNAMENT_H

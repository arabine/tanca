/*=============================================================================
 * Tanca - Tournament.cpp
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

#include "Tournament.h"
#include "Log.h"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <random>

bool RankHighFirst (Rank &i, Rank &j)
{
    bool ret = false;
    if (i.pointsWon > j.pointsWon)
    {
        ret = true;
    }
    else if (i.pointsWon == j.pointsWon)
    {
        if (i.Difference() > j.Difference())
        {
            ret = true;
        }
        else if (i.Difference() == j.Difference())
        {
            if (i.pointsOpponents > j.pointsOpponents)
            {
                ret = true;
            }
        }
    }
    return ret;
}

int Rank::Difference() const
{
    return (pointsWon - pointsLost);
}


void Rank::AddPoints(int gameId, int score, int oppScore)
{
    pointsWon  += score;
    pointsLost += oppScore;

    if (oppScore > score)
    {
        gamesLost++;
    }
    else if (score > oppScore)
    {
        gamesWon++;
    }
    else
    {
        gamesDraw++;
    }

    mGames.push_back(gameId);
}



Tournament::Tournament()
    : mIsTeam(false)
{

}

Tournament::~Tournament()
{

}

static std::random_device rd;
static std::mt19937_64 gen(rd());


int Tournament::Generate(int min, int max)
{
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(gen);
}

#if 0
std::string Tournament::ToJsonString(const std::deque<Game> &games, const std::deque<Team> &teams)
{
    std::string rounds;
    if (games.size() > 0)
    {
        QJsonArray json;
        for( auto &g : games)
        {
            Team team1;
            Team team2;

            bool ok = Team::Find(teams, g.team1Id, team1);
            ok = ok && Team::Find(teams, g.team2Id, team2);

            if (ok)
            {
                QJsonObject t1;
                t1["name"] = team1.teamName;
                t1["number"] = team1.number;
                if (g.team1Score == -1)
                {
                    t1["score"] = "";
                }
                else
                {
                    t1["score"] = g.team1Score;
                }

                QJsonObject t2;
                t2["name"] = team2.teamName;
                t2["number"] = team2.number;
                if (g.team1Score == -1)
                {
                    t2["score"] = "";
                }
                else
                {
                    t2["score"] = g.team2Score;
                }

                QJsonObject game;
                game["round"] = g.turn;
                game["t1"] = t1;
                game["t2"] = t2;

                json.append(game);
            }
        }

        QJsonDocument toJson(json);
        rounds = toJson.toJson();
    }
    else
    {
        rounds = "[]";
    }

    return rounds;
}

#endif

void Tournament::Add(int id, int gameId, int score, int opponent)
{
    if (!Contains(id))
    {
        // Create entry
        Rank rank;
        rank.id = id;
        mRanking.push_back(rank);
    }

    int index = FindRankIndex(id);

    if (index >= 0)
    {
        mRanking[index].AddPoints(gameId, score, opponent);
    }
}

int Tournament::FindRankIndex(int id)
{
    int index = -1;
    for (unsigned int i = 0; i < mRanking.size(); i++)
    {
        if (mRanking[i].id == id)
        {
            index = (int)i;
            break;
        }
    }
    return index;
}
bool Tournament::Contains(int id)
{
    return (FindRankIndex(id) >= 0);
}

// Find all the matches played by each opponent
// Sum the points won, this is the Buchholz points
void Tournament::ComputeBuchholz(const std::deque<Game> &games)
{
    // Loop on each team
    for (auto &rank : mRanking)
    {
        // 1. Loop through all the games played
        for (auto gameId :  rank.mGames)
        {
            // 2. Get the game
            Game game;

            if (Game::Find(games, gameId, game))
            {
                // 3. Find the opponent for this game
                int oppTeam = (game.team1Id == rank.id) ? game.team2Id : game.team1Id;

                // 4. Add the opponent points, search for it in the list
                for (auto &rank2 : mRanking)
                {
                    if (rank2.id == oppTeam)
                    {
                        rank.pointsOpponents += rank2.pointsWon;
                    }
                }
            }
            else
            {
                TLogError("Internal Buchholz algorithm problem");
            }
        }
    }
}


void Tournament::GeneratePlayerRanking(const std::deque<Game> &gameList, const std::deque<Team> &teamList, const std::deque<Event> &events)
{
    mRanking.clear();

    mIsTeam = false;

    for (auto const &event : events)
    {
        if ((event.state != Event::cCanceled) && event.HasOption(Event::cOptionSeasonRanking))
        {
            bool ok = true;
            if (ok)
            {
                // Search for every game played for this event
                std::deque<Game> games = Game::FindByEventId(gameList, event.id);
                std::deque<Team> teams = Team::FindByEventId(teamList, event.id);

                for (auto const &game : games)
                {
                    if (game.IsPlayed())
                    {
                        Team team;

                        if (Team::Find(teams, game.team1Id, team))
                        {
                            Add(team.player1Id, game.id, game.team1Score, game.team2Score);
                            Add(team.player2Id, game.id, game.team1Score, game.team2Score);
                        }

                        if (Team::Find(teams, game.team2Id, team))
                        {
                            Add(team.player1Id, game.id, game.team2Score, game.team1Score);
                            Add(team.player2Id, game.id, game.team2Score, game.team1Score);
                        }
                    }
                }
            }
        }
    }

    std::sort(mRanking.begin(), mRanking.end(), RankHighFirst);
}

bool Tournament::GetTeamRank(int id, Rank &outRank)
{
    bool ret = false;
    for (auto &rank : mRanking)
    {
        if (rank.id == id)
        {
            outRank = rank;
            ret = true;
            break;
        }
    }
    return ret;
}

void Tournament::GenerateTeamRanking(const std::deque<Game> &games, const std::deque<Team> &teams, int maxTurn)
{
    mRanking.clear();
    mByeTeamIds.clear();
    mIsTeam = true;

    for (auto const &game : games)
    {
        if (game.IsPlayed() && (game.turn < maxTurn))
        {
            Team team;

            // Special case of a dummy team: means that the other team has a bye
            if (game.HasBye())
            {
                int bye = game.GetByeTeam();
                if (Team::Find(teams, bye, team))
                {
                    int byeScore = (bye == game.team1Id) ? game.team1Score : game.team2Score;
                    int dummyScore = (bye == game.team2Id) ? game.team1Score : game.team2Score;

                    Add(team.id, game.id, byeScore, dummyScore);
                    mByeTeamIds.push_back(team.id);
                }
            }
            else
            {
                if (Team::Find(teams, game.team1Id, team))
                {
                    Add(team.id, game.id, game.team1Score, game.team2Score);
                }

                if (Team::Find(teams, game.team2Id, team))
                {
                    Add(team.id, game.id, game.team2Score, game.team1Score);
                }
            }
        }
    }


    // Compute Buchholtz points for all players to avoid equalities
    ComputeBuchholz(games);

    //create a list of sorted players
    std::sort(mRanking.begin(), mRanking.end(), RankHighFirst);
}

std::deque<Rank> Tournament::GetRanking()
{
    return mRanking;
}

/*****************************************************************************/
std::string Tournament::BuildRoundRobinRounds(const std::deque<Team> &tlist, std::uint32_t nbRounds, std::deque<Game> &games)
{
    std::string error;
    std::deque<Team> teams = tlist; // Local copy to manipulate the list

    if (teams.size()%2)
    {
        // odd numeber of team, add dummy one
        Team dummy;
        teams.push_back(dummy);
    }

    int max_games = teams.size() / 2;

    // Create fuzzy, never start with the same team
    std::random_shuffle(teams.begin(), teams.end());

    // Check if there is enough teams for the desired number of rounds
    // Round-robin tournament algorithm
    if (teams.size() > nbRounds)
    {
        for (uint32_t i = 0; i < nbRounds; i++)
        {
            // Create matches for this turn
            for (int j = 0; j < max_games; j++)
            {
                const Team &team = teams[j];
                const Team &opp = teams[teams.size() - j - 1];

                Game game;

                game.eventId = opp.eventId;
                game.turn = i;
                game.team1Id = team.id;
                game.team2Id = opp.id;

                games.push_back(game);
            }

            // Then rotate, keep first player always at the first place
            Team first = teams.front();
            Team last = teams.back();

            teams.pop_front();
            teams.pop_back();

            teams.push_front(last);
            teams.push_front(first);
        }
    }
    else
    {
        error = "pas assez de joueurs pour joueur les tours demandés";
    }
    return error;
}

/*****************************************************************************/
template <typename T>
inline bool IsMultipleOf(const T i_value, const T i_multiple)
{
    return (i_multiple != static_cast<T>(0))
           ? (i_value % i_multiple) == static_cast<T>(0)
           : false;
}

bool Tournament::HasPlayed(const std::deque<Game> &games, const Rank &rank, int oppId)
{
    bool hasPlayed = false;
    // 1. Loop through all the games played
    for (auto gameId :  rank.mGames)
    {
        // 2. Get the game
        Game game;

        if (Game::Find(games, gameId, game))
        {
            // 3. Find the opponent for this game
            int oppTeam = (game.team1Id == rank.id) ? game.team2Id : game.team1Id;
            if (oppTeam == oppId)
            {
                hasPlayed = true;
            }
        }
    }

    return hasPlayed;
}

// Take the first player in the list
// Find the opponent, never played with it, with the nearest level
int Tournament::FindtUnplayedIndex(const std::deque<Game> &games, const std::deque<int> &ranking)
{
    int index = -1;
    if (ranking.size() >= 2)
    {
        // Get the rank for the first team
        int rankIndex = FindRankIndex(ranking[0]);

        if (rankIndex >= 0)
        {
            unsigned int i = 1;
            do
            {
                // look for all the games played
                if (!HasPlayed(games, mRanking[rankIndex], ranking[i]))
                {
                    index = (int)i;
                }
                i++;
            } while ((index < 0) && (i < ranking.size()));
        }
        else
        {
            TLogError("Internal rank index finder problem");
        }
    }
    else
    {
        TLogError("Not enough teams to create game");
    }

    return index;
}

bool Tournament::AlreadyPlayed(const std::deque<Game> &games, int p1Id, int p2Id)
{
    // Get the rank for the first team
    int rankIndex = FindRankIndex(p1Id);
    return HasPlayed(games, mRanking[rankIndex], p2Id);
}


void PrintMatrix(const std::deque<int> &row, const std::deque<std::deque<int>> &matrix)
{
    // Print header
    for (unsigned int i = 0; i < row.size(); i++)
    {
        std::cout << "\t" << row[i];
    }

    for (unsigned int i = 0; i < row.size(); i++)
    {
        std::cout << "\n" << row[i] << " [ ";
        for (unsigned int j = 0; j < row.size(); j++)
        {
            std::cout << "\t" << matrix[i][j];
        }
        std::cout << " ] ";
    }
    std::cout << std::endl;
}


struct Point
{
    int col;
    int row;
};

struct Solution
{
    std::uint64_t totalCost;
    std::uint64_t depth;
    std::deque<int> taken;  // 1 if column is taken
    std::deque<Point> tree;

    Solution(int size)
        : totalCost(0)
        , depth(0)
    {
        for (int i = 0; i < size; i++)
        {
            taken.push_back(0);
        }
    }

    std::uint32_t Size()
    {
        return taken.size();
    }

private:
    Solution();
};


void FindSolution(std::deque<Solution> &list, Solution &s, const std::deque<int> &teamIds, const std::deque<std::deque<int>> &cost, std::uint32_t col, std::uint32_t row)
{
    s.depth++;
    do
    {
        // Free team
        if (s.taken[row] == 0)
        {
            // try all the paths for this row
            do
            {
            //    std::cout << "Current: c=" << (int)col << ", r=" << (int)row << ", depth: " << (int)s.depth << std::endl;

                if (s.taken[col] == 0)
                {
                    Solution s2 = s; // start new solution branch

                    // This team is free, create match with it
                    // Create game
                    Point p;
                    p.col = col;
                    p.row = row;

                    s2.taken[col] = 1;
                    s2.taken[row] = 1;

                    int pair_cost = cost[row][col];
                //    std::cout << "Cost is: " << pair_cost << std::endl;
                    s2.totalCost += pair_cost;
                    s2.tree.push_back(p);

                    // continues this path for the other rows
                    std::uint32_t r = row + 1;
                    std::uint32_t c = r + 1; // start at the beginning of the row
                    if ((r < (s.Size()-1)) && (c < s.Size()))
                    {
                        FindSolution(list, s2, teamIds, cost, c, r);
               //         std::cout << "Return from recursive call" << std::endl;
                    }

                    // end of the matrix, append the solution
                    //if (r >= (s.Size()-1))
                    if (static_cast<std::uint32_t>(s2.tree.size()) == (s.Size() / 2U))
                    {
                        if (s2.totalCost < Rank::cHighCost)
                        {
                            std::cout << "Found one solution!" << std::endl;
                            list.push_back(s2);
                        }
                        else
                        {
                     //       std::cout << "Path cost too high, skip this: "  << s2.totalCost << std::endl;
                        }
                    }
                }
            }
            while(++col < s.Size());
        }
        row++;
        col = row + 1;
    }
    while (row < (s.Size() - 1));
}



bool Tournament::BuildPairing(const std::deque<int> &ranking,
                              const std::deque<std::deque<int>> &cost,
                              std::deque<Game> &newRounds)
{
    std::uint32_t size = cost[0].size();
    std::deque<Solution> solutions;
    std::deque<std::deque<int>> pairing(size);

    for (std::uint32_t i = 0; i < size; i++)
    {
        for (std::uint32_t j = 0; j < size; j++)
        {
            pairing[i].push_back(0);
        }
    }

    std::uint32_t row = 0;
    std::uint32_t col = 1;

    Solution s(size);
    FindSolution(solutions, s, ranking, cost, col, row);

    std::cout << "Found " << solutions.size() << " solutions" << std::endl;

    int i = 1;

    int minCost = Rank::cHighCost;
    Solution choice(size);

    for (auto &s : solutions)
    {
    //    std::cout << "\r\n ----------- Solution " << i << " cost is :" << s.totalCost << std::endl;

        if (s.totalCost < minCost)
        {
            minCost = s.totalCost;
            choice = s;
        }
/*
        for (auto &p : s.tree)
        {
            std::cout << ranking[p.col] << " <--> " << ranking[p.row] << std::endl;
        }
        */
        i++;
    }


  //  std::cout << "\r\n ========= Solution choosen has cost: " << choice.totalCost << std::endl;
    for (auto &p : choice.tree)
    {
  //      std::cout << ranking[p.col] << " <--> " << ranking[p.row] << std::endl;
        pairing[p.row][p.col] = 1;

        // Create game
        Game game;

        game.team1Id = ranking[p.col];
        game.team2Id = ranking[p.row];

        newRounds.push_back(game);
    }

    std::cout << "======>  COST MATRIX " << std::endl;
    PrintMatrix(ranking, cost);
    std::cout << "======>  PAIRING MATRIX" << std::endl;
    PrintMatrix(ranking, pairing);

    return (solutions.size() > 0);
}

void Tournament::BuildCost(const std::deque<Game> &games,
                           std::deque<int> &ranking,
                           std::deque<std::deque<int>> &cost_matrix)
{
    int size = ranking.size();

    // Init matrix
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            int cost = 0;

            if (ranking[j] == ranking[i])
            {
                // Same player
                cost = Rank::cHighCost;
            }
            else if (AlreadyPlayed(games, ranking[j], ranking[i]))
            {
                cost = Rank::cHighCost;
            }
            else
            {
                // compute cost
                cost = std::abs(mRanking[j].ComputeForce() - mRanking[i].ComputeForce());
            }

            cost_matrix[i].push_back(cost);
        }
    }
}


bool IsOdd(int size)
{
    return size%2;
}

std::string Tournament::BuildSwissRounds(const std::deque<Game> &games, const std::deque<Team> &teams, std::deque<Game> &newRounds)
{
    std::string error;
    int eventId;

    if (teams.size() >= 2)
    {
        eventId = teams.at(0).eventId; // memorize the current event id

        size_t nbGames = teams.size() / 2;

        // 1. Check where we are
        if (games.size() == 0)
        {
            // Firt round, compute random games
            error = BuildRoundRobinRounds(teams, 1, newRounds);
            std::cout << "First round" << std::endl;
        }
        else
        {
            // Check if all games are played
            bool allFinished = true;
            for (auto &game : games)
            {
                if (!game.IsPlayed())
                {
                    allFinished = false;
                }
            }

            // Figure out where we are
            if (IsMultipleOf(games.size(), nbGames) && allFinished)
            {
                int turn = (games.size() / nbGames);

                // Create ranking
                GenerateTeamRanking(games, teams, turn);

                // Create a local list of the ranking, keep only ids
                std::deque<int> ranking;

                for (auto &rank : mRanking)
                {
                    Team team;
                    Team::Find(teams, rank.id, team);
                    std::cout << rank.id << ". " << team.teamName << std::endl;
                    ranking.push_back(rank.id);
                }

//                // Create a matrix and fill it
//                std::deque<std::deque<int>> cost_matrix(ranking.size());
//                BuildCost(games, ranking, cost_matrix);
//                bool success = BuildPairing(ranking, cost_matrix, newRounds);



                // Split in two vectors
                unsigned int rank_size = ranking.size() / 2;

                if (IsOdd(rank_size))
                {
                    rank_size++;
                }

                std::deque<int> winners(ranking.begin(), ranking.begin() + rank_size);
                std::deque<int> loosers(ranking.begin() + rank_size, ranking.end());

                std::cout << "---------------  WINNERS -------------------" << std::endl;
                // Create a matrix and fill it
                std::deque<std::deque<int>> cost_matrix(rank_size);
                BuildCost(games, winners, cost_matrix);
                bool success = BuildPairing(winners, cost_matrix, newRounds);

                std::cout << "---------------  LOOSERS -------------------" << std::endl;
                // Create a matrix and fill it
                std::deque<std::deque<int>> cost_matrix2(loosers.size());
                BuildCost(games, loosers, cost_matrix2);
                success = success && BuildPairing(loosers, cost_matrix2, newRounds);

                for (auto &game : newRounds)
                {
                    game.eventId = eventId;
                    game.turn = turn;
                }

                if (!success)
                {
                    error = "erreur d'appariement : aucune solution trouvée.";
                }
            }
            else
            {
                error = "tour en cours non terminé.";
            }
        }
    }
    else
    {
        error = "pas assez d'équipe en jeu.";
    }

    return error;
}


std::string Tournament::RankingToString()
{
    std::stringstream ss;
    ss << "ID\t" << "Won\t" << "Lost\t" << "Draw\t" << "Diff\t" << "Buchholz\t" << std::endl;

    for (auto &rank : mRanking)
    {
        ss << rank.id << "\t"
           << rank.gamesWon << "\t"
           << rank.gamesLost << "\t"
           << rank.gamesDraw << "\t"
           << rank.Difference() << "\t"
           << rank.pointsOpponents << std::endl;
    }
    return ss.str();
}


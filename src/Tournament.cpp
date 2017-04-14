#include "Tournament.h"
#include "DbManager.h"
#include "Log.h"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <random>

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>

bool RankHighFirst (Rank &i, Rank &j)
{
    bool ret = false;
    if (i.gamesWon > j.gamesWon)
    {
        ret = true;
    }
    else if (i.gamesWon == j.gamesWon)
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

    mGames.append(gameId);
}



Tournament::Tournament()
    : mIsTeam(false)
{

}

Tournament::~Tournament()
{

}

std::random_device rd;
std::mt19937_64 gen(rd());


int Tournament::Generate(int min, int max)
{
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(gen);
}

QString Tournament::ToJsonString(const QList<Game> &games, const QList<Team> &teams)
{
    QString rounds;
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
            else
            {
                TLogError(std::string(__PRETTY_FUNCTION__) + std::string(": Internal problem: cannot find team"));
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
void Tournament::ComputeBuchholz(const QList<Game> &games)
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


void Tournament::GeneratePlayerRanking(const DbManager &mDb, const QList<Event> &events)
{
    mRanking.clear();

    mIsTeam = false;

    foreach (Event event, events)
    {
        if (event.state != Event::cCanceled)
        {
            bool ok = true;
            if (ok)
            {
                // Search for every game played for this event
                QList<Game> games = mDb.GetGamesByEventId(event.id);
                QList<Team> teams = mDb.GetTeams(event.id);

                foreach (Game game, games)
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
}


void Tournament::GenerateTeamRanking(const QList<Game> &games, const QList<Team> &teams, int maxTurn)
{
    mRanking.clear();
    mByeTeamIds.clear();
    mIsTeam = true;

    foreach (auto &game, games)
    {
        if (game.IsPlayed() && (game.turn <= maxTurn))
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

std::vector<Rank> Tournament::GetRanking()
{
    return mRanking;
}

/*****************************************************************************/
QString Tournament::BuildRoundRobinRounds(const QList<Team> &tlist, int nbRounds, QList<Game> &games)
{
    QString error;
    QList<Team> teams = tlist; // Local copy to manipulate the list

    if (teams.size()%2)
    {
        // odd numeber of team, add dummy one
        Team dummy;
        teams.append(dummy);
    }

    int max_games = teams.size() / 2;

    // Create fuzzy, never start with the same team
    std::random_shuffle(teams.begin(), teams.end());

    // Check if there is enough teams for the desired number of rounds
    // Round-robin tournament algorithm
    if (teams.size() > nbRounds)
    {
        for (int i = 0; i < nbRounds; i++)
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

                games.append(game);
            }

            // Then rotate, keep first player always at the first place
            Team first = teams.takeFirst();
            Team last = teams.takeLast();
            teams.prepend(last);
            teams.prepend(first);
        }
    }
    else
    {
        error = tr("pas assez de joueurs pour jouer %1 tours").arg(nbRounds);
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

bool Tournament::HasPlayed(const QList<Game> &games, const Rank &rank, int oppId)
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
int Tournament::FindtUnplayedIndex(const QList<Game> &games, const std::vector<int> &ranking)
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

bool Tournament::AlreadyPlayed(const QList<Game> &games, int p1Id, int p2Id)
{
    // Get the rank for the first team
    int rankIndex = FindRankIndex(p1Id);
    return HasPlayed(games, mRanking[rankIndex], p2Id);
}


void PrintMatrix(const std::vector<int> &row, const std::vector<std::vector<int>> &matrix)
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


void Tournament::BuildPairing(const std::vector<int> &ranking,
                              const std::vector<std::vector<int>> &cost,
                              std::vector<std::vector<int>> &pairing,
                              QList<Game> &newRounds)
{
    unsigned int size = cost[0].size();
    std::vector<int> columns_taken(size);

    for (unsigned int i = 0; i < size; i++)
    {
        columns_taken[i] = 0;
    }

    // Resolve, by line
    for (unsigned int i = 0; i < size; i++)
    {
        // If the team has already be assigned to a game, don't perform the research
        if (columns_taken[i] == 0)
        {
            int lowest_column = -1;
            int lowest_value = 999999999;
            // Find lowest column for this line
            for (unsigned int j = 0; j < size; j++)
            {
                if (cost[i][j] < lowest_value)
                {
                    // Skip column or line if taken
                    if (columns_taken[j] == 0)
                    {
                        lowest_value = cost[i][j];
                        lowest_column = j;
                    }
                }
            }
            if (lowest_column >= 0)
            {
                pairing[i][lowest_column] = 1; // Debug purpose
                columns_taken[lowest_column] = 1;
                columns_taken[i] = 1;

                // Create game
                Game game;

                game.team1Id = ranking[lowest_column];
                game.team2Id = ranking[i];

                newRounds.append(game);
            }
            else
            {
                std::cout << "Cannot find opponent" << std::endl;
            }
        }
    }
}

void Tournament::BuildCost(const QList<Game> &games,
                           std::vector<int> &ranking,
                           std::vector<std::vector<int>> &cost_matrix,
                           std::vector<std::vector<int>> &pairing)
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
                cost = 10000;
            }
            else if (AlreadyPlayed(games, ranking[j], ranking[i]))
            {
                cost = 1000;
            }
            else
            {
                // compute cost
                cost = std::abs(mRanking[j].Difference() - mRanking[i].Difference());
            }

            cost_matrix[i].push_back(cost);
            pairing[i].push_back(0);
        }
    }
}


bool IsOdd(int size)
{
    return size%2;
}

QString Tournament::BuildSwissRounds(const QList<Game> &games, const QList<Team> &teams, QList<Game> &newRounds)
{
    QString error;
    int eventId;

    if (teams.size() >= 2)
    {
        bool hasDummy = false;
        eventId = teams.at(0).eventId; // memorize the current event id

        int nbGames = teams.size() / 2;
        if (teams.size()%2)
        {
            hasDummy = true;
            nbGames += 1;
        }

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

                // Prepare dummy team if needed
                Team dummy;
                dummy.eventId = eventId;

                // Create a local list of the ranking, keep only ids
                std::vector<int> ranking;
                for (auto &rank : mRanking)
                {
                    ranking.push_back(rank.id);
                }

                int byeTeamId = 0;
                if (hasDummy)
                {
                    // Choose the bye team
                    bool found = false;
                    int max = ranking.size() - 1;
                    while (!found)
                    {
                        byeTeamId = ranking[Generate(0, max)];

                        // Make sure to give the bye to a different team each turn
                        if (std::find(mByeTeamIds.begin(), mByeTeamIds.end(), byeTeamId) == mByeTeamIds.end())
                        {
                            // Not found in previous match, this team has a bye for this round
                            found = true;
                        }
                    }
                }

                // Split in two vectors
                unsigned int rank_size = ranking.size() / 2;

                if (IsOdd(rank_size))
                {
                    rank_size++;
                }

                std::vector<int> winners(ranking.begin(), ranking.begin() + rank_size);
                std::vector<int> loosers(ranking.begin() + rank_size, ranking.end());

             //   ranking = winners;
             //   std::size_t const rank_size = ranking.size();

                // Create a matrix and fill it
                std::vector<std::vector<int>> cost_matrix(rank_size);
                std::vector<std::vector<int>> pairing_matrix(rank_size);

                std::cout << "---------------  WINNERS COST MATRIX -------------------" << std::endl;
                BuildCost(games, winners, cost_matrix, pairing_matrix);
                PrintMatrix(winners, cost_matrix);
                std::cout << "---------------  WINNERS PAIRING MATRIX -------------------" << std::endl;
                BuildPairing(winners, cost_matrix, pairing_matrix, newRounds);
                PrintMatrix(winners, pairing_matrix);

                std::cout << "---------------  LOOSERS COST MATRIX -------------------" << std::endl;
                // Create a matrix and fill it
                std::vector<std::vector<int>> cost_matrix2(loosers.size());
                std::vector<std::vector<int>> pairing_matrix2(loosers.size());

                BuildCost(games, loosers, cost_matrix2, pairing_matrix2);
                PrintMatrix(loosers, cost_matrix2);
                std::cout << "---------------  LOOSERS PAIRING MATRIX -------------------" << std::endl;
                BuildPairing(loosers, cost_matrix2, pairing_matrix2, newRounds);
                PrintMatrix(loosers, pairing_matrix2);


                for (auto &game : newRounds)
                {
                    game.eventId = eventId;
                    game.turn = turn;
                }


                /*
                while (ranking.size() > 0)
                {
                    // Create matches for this turn, start with first player of the list
                    int team1Id = ranking[0];
                    int team2Id = -1;

                    if ((team1Id == byeTeamId) && hasDummy)
                    {
                        // Use dummy for team2
                        team2Id = Team::cDummyTeam;
                    }
                    else
                    {
                        int index = FindtUnplayedIndex(games, ranking);
                        if (index >= 0)
                        {
                            team2Id = ranking[index];
                            ranking.erase(ranking.begin() + index);
                        }
                        else
                        {
                            TLogError("Cannot find next player to play with");
                        }
                    }

                    // Always remove first team of the list
                    ranking.erase(ranking.begin());

                    Game game;

                    game.eventId = eventId;
                    game.turn = turn;
                    game.team1Id = team1Id;
                    game.team2Id = team2Id;

                    newRounds.append(game);
                }

                */

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


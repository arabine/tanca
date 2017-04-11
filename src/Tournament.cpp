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

int Rank::Difference()
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
                t1["number"] = g.team1Id;
                t1["score"] = g.team1Score;

                QJsonObject t2;
                t2["name"] = team2.teamName;
                t2["number"] = g.team2Id;
                t2["score"] = g.team2Score;

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
        mList.push_back(rank);
    }

    int index = Find(id);

    if (index >= 0)
    {
        mList[index].AddPoints(gameId, score, opponent);
    }
}

int Tournament::Find(int id)
{
    int index = -1;
    for (unsigned int i = 0; i < mList.size(); i++)
    {
        if (mList[i].id == id)
        {
            index = (int)i;
            break;
        }
    }
    return index;
}
bool Tournament::Contains(int id)
{
    return (Find(id) >= 0);
}

// Find all the matches played by each opponent
// Sum the points won, this is the Buchholz points
void Tournament::ComputeBuchholz(const QList<Game> &games)
{
    // Loop on each team
    for (auto &rank : mList)
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
                for (auto &rank2 : mList)
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
    mList.clear();

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


void Tournament::GenerateTeamRanking(const QList<Game> &games, const QList<Team> &teams)
{
    mList.clear();
    mByeTeams.clear();
    mIsTeam = true;

    foreach (auto &game, games)
    {
        if (game.IsPlayed())
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
                    mByeTeams.push_back(team.id);
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
            // Figure out where we are
            if (IsMultipleOf(games.size(), nbGames))
            {
                int turn = (games.size() / nbGames);

                // Create ranking
                GenerateTeamRanking(games, teams);

                // Compute Buchholtz points for all players to avoid equalities
                ComputeBuchholz(games);

                //create a list of sorted players
                std::sort(mList.begin(), mList.end(), RankHighFirst);

                QList<Team> tt = teams;

                // Prepare dummy team if needed
                Team dummy;
                dummy.eventId = eventId;

                int byeTeam = 0;
                if (hasDummy)
                {
                    // Choose the bye team
                    bool found = false;
                    int max = teams.size() - 1;
                    while (!found)
                    {
                        byeTeam = Generate(0, max);

                        if (std::find(mByeTeams.begin(), mByeTeams.end(), byeTeam) == mByeTeams.end())
                        {
                            // Not found in previous match, this team has a bye for this round
                            found = true;
                        }
                    }
                }

                // FIXME: add the dummy team in the Rank at a random place
                // Make sure to give the bye to a different team each turn

                // Create matches for this turn
                int j = 0;
                for (; j < tt.size();)
                {
                    Rank team1;
                    Rank team2;

                    if ((j == byeTeam) && hasDummy)
                    {
                        // Use dummy
                    }
                    else
                    {
                       team1 = mList[j];
                       j++;
                    }

                    team2 = mList[j];
                    j++;

                    Game game;

                    game.eventId = eventId;
                    game.turn = turn;
                    game.team1Id = team1.id;
                    game.team2Id = team2.id;

                    newRounds.append(game);
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

    for (auto &rank : mList)
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


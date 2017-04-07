#include "Tournament.h"
#include "DbManager.h"

#include <iostream>

#include <QMessageBox>


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



Tournament::Tournament(const DbManager &db)
    : mDb(db)
    , mIsTeam(false)
{

}

void Tournament::Add(int id, int gameId, int score, int opponent)
{
    if (!mList.contains(id))
    {
        // Create entry
        mList[id] = Rank();
    }

    mList[id].AddPoints(gameId, score, opponent);
}

// Find all the matches played by each opponent
// Sum the points won, this is the Buchholz points
void Tournament::ComputeBuchholz()
{
    // Loop on each team
    auto iter = mList.begin();
    while (iter != mList.end())
    {
        // 1. Loop through all the games played
        for (auto gameId :  iter.value().mGames)
        {
            // 2. Get the game
            Game game = mDb.GetGameById(gameId);

            // 3. Find the opponent for this game
            int oppTeam = (game.team1Id == iter.key()) ? game.team2Id : game.team1Id;

            // 4. Add the opponent points, search for it in the list
            auto iter2 = mList.begin();
            while (iter2 != mList.end())
            {
                if (iter2.key() == oppTeam)
                {
                    iter.value().pointsOpponents += iter2.value().pointsWon;
                }
                ++iter2;
            }
        }

        ++iter;
    }
}


void Tournament::GeneratePlayerRanking(const QList<Event> &events)
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


void Tournament::GenerateTeamRanking(const QList<Event> &events,
                                      const Event &currentEvent)
{
    mList.clear();

    mIsTeam = true;

    foreach (Event event, events)
    {
        if (event.state != Event::cCanceled)
        {
            bool ok = true;
            // We want to show only the current event ranking
            if (event.id != currentEvent.id)
            {
                ok = false;
            }

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
    }
}

/*****************************************************************************/
QList<Game> Tournament::BuildRoundRobinRounds(const QList<Team> &tlist, int nbRounds)
{
    QList<Game> games;
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
    // Swiss tournament algorithm
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

                std::cout << "Turn: " << (i+1) << ", Game: " << team.teamName.toStdString() << " <- vs -> " << opp.teamName.toStdString() << std::endl;
            }

            // Then rotate, keep first player always at the first place
            Team first = teams.takeFirst();
            Team last = teams.takeLast();
            teams.prepend(last);
            teams.prepend(first);
        }
    }
    return games;
}

/*****************************************************************************/
QList<Game> Tournament::BuildSwissRounds(const QList<Game> &previous, const QList<Team> &tlist)
{
    QList<Game> newRound;
    // todo!

    return newRound;
}

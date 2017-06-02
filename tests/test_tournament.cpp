
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <random>

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>

#include "Tournament.h"
#include "DbManager.h"
#include "Log.h"
#include "Brackets.h"

void ReadFile(const std::string &filename, std::vector<std::string> &output)
{
    std::string line;
    std::ifstream ifs(filename);

    while(getline(ifs, line)) {
        output.push_back(line);
    }
}


std::string GenerateName(const std::vector<std::string> &first_names, const std::vector<std::string> &last_names)
{
    int first = Tournament::Generate(0, first_names.size());
    int last = Tournament::Generate(0, last_names.size());

    return first_names[first] + " " + last_names[last];
}

void GenerateTeams(QList<Team> &t, int size, const std::vector<std::string> &first_names, const std::vector<std::string> &last_names)
{
    static int teamIds = 0;

    for (int i = 0; i < size; i++)
    {
        Team team;
        team.eventId = 0;
        team.id = teamIds;
        team.teamName = QString(GenerateName(first_names, last_names).c_str());

        t.push_back(team);

        teamIds++;
    }
}

void DumpTeams(const QList<Team> &t)
{
    for(int i = 0; i < t.size(); i++)
    {
        std::cout << "(" << t.at(i).id  << ") " << t.at(i).teamName.toStdString() << std::endl;
    }
}

void DumpGames(const QList<Game> &g, const QList<Team> &teams)
{
    for(int i = 0; i < g.size(); i++)
    {
        Team team1;
        Team team2;

        Team::Find(teams, g.at(i).team1Id, team1);
        Team::Find(teams, g.at(i).team2Id, team2);
        std::cout << "Turn: " << g.at(i).turn  << ", Game (id: " << g.at(i).id << ") "
                  << " (id: " << team1.id << ") " << team1.teamName.toStdString() << " (" << g.at(i).team1Score << ") <-> "
                  << " (id: " << team2.id << ") " << team2.teamName.toStdString() << " (" << g.at(i).team2Score << ")" << std::endl;
    }
}

void PlayGames(QList<Game> &g)
{
    static int gameIds = 0;

    for(int i = 0; i < g.size(); i++)
    {
        // FIXME: special score for bye team (won 13-7)

        g[i].team1Score = Tournament::Generate(0, 12);
        g[i].team2Score = 13;
        g[i].id = gameIds;
        gameIds++;
    }
}

void RandomMatches()
{
    std::vector<std::string> first_names;
    std::vector<std::string> last_names;

    QList<Team> teams;
    QList<Game> games;

    ReadFile("../../tanca/tests/first_names.txt", first_names);
    ReadFile("../../tanca/tests/last_names.txt", last_names);

    GenerateTeams(teams, 10, first_names, last_names);
    DumpTeams(teams);

    Tournament trn;

    int turns = 0;
    while(1)
    {
        QList<Game> newGames;
        QString result = trn.BuildSwissRounds(games, teams, newGames);

        // Print ranking for turns after the first one
        std::cout << "------------- RANKING -----------------" << std::endl;
        std::cout << trn.RankingToString();

        if (turns == 4)
        {
            break;
        }

        std::cout << "SwissRound result: " << result.toStdString() << std::endl;
        PlayGames(newGames);
        games.append(newGames);
        DumpGames(games, teams);
        turns++;
    }
}


void PairingProblem()
{
    QList<Team> teams;
    QList<Game> games;

    // Add the games of the first round
    Game gamesTurn0[5] = {
        {245, 19, 0, 164, 169, 13, 10},
        {246, 19, 0, 163, 166, 0, 13},
        {247, 19, 0, 167, 161, 8, 13},
        {248, 19, 0, 168, 162, 13, 7},
        {249, 19, 0, 160, 165, 13, 12},
    };

    for (int i = 0; i < 5; i++)
    {
        Team t1, t2;

        t1.eventId = gamesTurn0[0].eventId;
        t1.id = gamesTurn0[i].team1Id;

        t2.eventId = gamesTurn0[0].eventId;
        t2.id = gamesTurn0[i].team2Id;

        // Add teams and game played
        teams.push_back(t1);
        teams.push_back(t2);
        games.push_back(gamesTurn0[i]);
    }

    Tournament trn;

    int turns = 1;

    trn.GenerateTeamRanking(games, teams, turns);
    // Print ranking for turns after the first one
    std::cout << "------------- RANKING -----------------" << std::endl;
    std::cout << trn.RankingToString();

    while(1)
    {
        QList<Game> newGames;
        QString result = trn.BuildSwissRounds(games, teams, newGames);

        // Print ranking for turns after the first one
        std::cout << "------------- RANKING -----------------" << std::endl;
        std::cout << trn.RankingToString();

        if (turns == 3)
        {
            break;
        }

        std::cout << "SwissRound result: " << result.toStdString() << std::endl;
        PlayGames(newGames);
        games.append(newGames);
        DumpGames(games, teams);
        turns++;
    }

}

void RunTests()
{
  //  RandomMatches();

    PairingProblem();
}


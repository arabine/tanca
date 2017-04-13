/**
 *  Tanca, a petanque contests manager
 *  Copyright (C) 2016  Anthony Rabine <anthony.rabine@tarotclub.fr>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MainWindow.h"
#include <QApplication>
#include <QtQuick>

#include <iostream>
#include <fstream>
/*
#include <chrono>
#include <thread>
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
*/
#include "Brackets.h"
#include "Tournament.h"
#include "Log.h"

//#define UNIT_TESTS


#ifdef UNIT_TESTS
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
#endif

/*****************************************************************************/
class Logger : public Observer<std::string>
{
public:
    Logger()
        : Observer(0xFFU)
    {

    }

    void Update(const std::string &info)
    {
        std::cout << info << std::endl;
    }
};

int main(int argc, char *argv[])
{

    Logger logger;
    Log::RegisterListener(logger);

#ifdef UNIT_TESTS
    (void) argc;
    (void) argv;

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
    return 0;
#else
    QApplication a(argc, argv);

    // Register our component type with QML.
    qmlRegisterType<Brackets>("Tanca", 1, 0, "Brackets");

    MainWindow w;
    w.Initialize();
    w.show();

    return a.exec();
#endif

}

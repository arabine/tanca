#ifndef I_DATA_BASE_H
#define I_DATA_BASE_H

#include <string>
#include <deque>
#include <chrono>

#include <Util.h>

struct Player
{
    static const int cDummyPlayer = 999999999;

    int id;
    std::string uuid;
    std::string name;
    std::string lastName;
    std::string nickName;
    std::string email;
    std::string mobilePhone;
    std::string homePhone;
    std::chrono::system_clock::time_point birthDate;
    std::string road;
    int postCode;
    std::string city;
    std::string membership;    ///< semi_column separated list, season start on 01/09 of each year.
    std::string comments;
    int state;
    std::string document; // JSON document, reserved to store anything in the future

    Player()
    {
        Reset();
    }

    void Reset()
    {
        id = cDummyPlayer;
        postCode = -1;
        uuid = "";
        name = "";
        lastName = "--";
        nickName = "";
        email = "";
        mobilePhone = "";
        homePhone = "";
        birthDate = std::chrono::system_clock::now();
        road = "";
        city = "";
        membership = "";
        comments = "";
        state = -1;
        document = "";
    }

    std::string FullName()
    {
        return name + " " + lastName;
    }

    static bool Find(const std::deque<Player> &players, const int id, Player &player)
    {
        bool found = false;

        if (id == Player::cDummyPlayer)
        {
            found = true;
            player.lastName = "--";
        }
        else
        {
            for (auto const & p : players)
            {
                if (p.id == id)
                {
                    player = p;
                    found = true;
                    break;
                }
            }
        }
        return found;
    }

    static bool Index(const std::deque<Player> &players, const int id, int &index)
    {
        bool found = false;

        index = 0;
        for (auto const & p : players)
        {
            if (p.id == id)
            {
                found = true;
                break;
            }
            index++;
        }
        return found;
    }


};

struct Event
{
    int id;
    int year;
    std::chrono::system_clock::time_point date;
    std::string title;
    int state;
    int type;
    int option;
    std::string document;  // JSON document, reserved to store anything in the future

    // State
    static const int cNotStarted = 0;
    static const int cStarted = 1;
    static const int cCanceled = 2;

    // Type
    static const int cRoundRobin   = 0;
    // type 1 was used in the past
    static const int cSwissRounds   = 2;

    // Option, maybe manage it with a bitmask so that is can be used in many ways
    static const int cNoOption   = 0;
    static const int cOptionSeasonRanking = 1; // if set, count this event for the season ranking

    Event()
        : id(-1)
        , year(-1)
        , state(cNotStarted)
        , type(cRoundRobin)
        , option(cOptionSeasonRanking) // add default options with OR operator
    {
        date = std::chrono::system_clock::now();
    }

    bool HasOption(int opt) const
    {
        return (option & opt) != 0;
    }

    bool IsValid()
    {
        return (id != -1);
    }


};

struct Team
{
    // Version 0.0
    int id;
    int eventId;
    std::string teamName;
    int player1Id;
    int player2Id;
    int player3Id;
    int state;
    std::string document;  // JSON document, reserved to store anything in the future

    // Version 1.0
    int number;

    static const int cDummyTeam = Player::cDummyPlayer;

    // State of the team
    static const int cStateValid    = 0;   // Valid team
    static const int cStateInvalid  = 1; // Invalid team (excluded etc.)

    Team()
        : id(cDummyTeam)
        , eventId(-1)
        , teamName("Dummy")
        , player1Id(Player::cDummyPlayer)
        , player2Id(Player::cDummyPlayer)
        , player3Id(Player::cDummyPlayer)
        , state(cStateValid)
    {

    }

    /**
     * @brief Find a team id within a list
     * @param teams
     * @param id
     * @param team
     * @return
     */
    static bool Find(const std::deque<Team> &teams, const int id, Team &team)
    {
        bool found = false;
        for (auto const &i : teams)
        {
            if (i.id == id)
            {
                found = true;
                team = i;
                break;
            }
        }

        return found;
    }

    static std::deque<Team> FindByEventId(const std::deque<Team> &teams, const int eventId)
    {
        std::deque<Team> t;
        for (auto const &i : teams)
        {
            if (i.eventId == eventId)
            {
                t.push_back(i);
            }
        }

        return t;
    }

    /**
     * @brief Find a team index in the list by its id
     * @param players
     * @param id
     * @param index
     * @return
     */
    static bool Index(const std::deque<Team> &teams, const int id, int &index)
    {
        bool found = false;

        index = 0;
        for (auto const & t : teams)
        {
            if (t.id == id)
            {
                found = true;
                break;
            }
            index++;
        }
        return found;
    }


};


struct Reward
{
    static const int cStateNoReward = -1;
    static const int cStateRewardOk = 0;
    static const int cStateRewardCanceled = 1;

    int id;
    int eventId;
    int teamId;
    int total; // total of the reward offered to the team for this event
    int state;
    std::string comment; // comment for this reward
    std::string document;  // JSON document, reserved to store anything in the future

    Reward()
        : id(-1)
        , eventId(-1)
        , teamId(-1)
        , total(0)
        , state(cStateNoReward)
    {

    }

};

struct Game
{
    int id;
    int eventId; // Also include the event id to avoid too complex DB queries
    int turn;       // Round turn (1 turn, 2nd turn ...)
    int team1Id;
    int team2Id;
    int team1Score;
    int team2Score;
    int state;
    std::string document;  // JSON document, reserved to store anything in the future


    bool HasBye() const
    {
        bool ret = false;
        if ((team1Id == Team::cDummyTeam) ||
            (team2Id == Team::cDummyTeam))
        {
            ret = true;
        }
        return ret;
    }

    int GetByeTeam() const
    {
        return (team1Id == Team::cDummyTeam) ? team2Id : team1Id;
    }


    /**
     * @brief Find a game id within a list
     * @param teams
     * @param id
     * @param team
     * @return
     */
    static bool Find(const std::deque<Game> &games, const int id, Game &game)
    {
        bool found = false;
        for (auto const &i : games)
        {
            if (i.id == id)
            {
                found = true;
                game = i;
                break;
            }
        }

        return found;
    }

    static std::deque<Game> FindByEventId(const std::deque<Game> &games, const int eventId)
    {
        std::deque<Game> g;
        for (auto const &i : games)
        {
            if (i.eventId == eventId)
            {
                g.push_back(i);
            }
        }

        return g;
    }


    bool IsPlayed() const
    {
        bool active = false;
        if ((team1Score != -1) && (team2Score != -1))
        {
            if ((team1Score + team2Score) > 0)
            {
                active = true;
            }
        }
        return active;
    }

    void Cancel()
    {
        team1Score = -1;
        team2Score = -1;
    }

    Game()
        : id(-1)
        , eventId(-1)
        , turn(-1)
        , team1Id(-1)
        , team2Id(-1)
        , team1Score(-1)
        , team2Score(-1)
        , state(-1)
    {

    }

    Game(int i,
        int e,
        int t,
        int t1Id,
        int t2Id,
        int t1Score,
        int t2Score)
        : id(i)
        , eventId(e)
        , turn(t)
        , team1Id(t1Id)
        , team2Id(t2Id)
        , team1Score(t1Score)
        , team2Score(t2Score)
        , state(-1)
    {

    }

};


#endif // I_DATA_BASE_H

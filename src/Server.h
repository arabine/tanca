#ifndef SERVER_H
#define SERVER_H

#include "TcpServer.h"
#include "DbManager.h"

class Server : public tcp::TcpServer::IEvent
{
public:
    Server();

    void Initialize();
    void Stop();

    // From tcp::TcpServer::IEvent
    virtual void NewConnection(const tcp::Conn &conn);
    virtual void ReadData(const tcp::Conn &conn);
    virtual void ClientClosed(const tcp::Conn &conn);
    virtual void ServerTerminated(tcp::TcpServer::IEvent::CloseType type);

    void SetGames(const std::string &games) {
        mCurrentGames = games;
    }

private:
    tcp::TcpServer mServer;

    std::string mCurrentGames;
};

#endif // SERVER_H

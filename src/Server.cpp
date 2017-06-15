#include "Server.h"


Server::Server()
    : mServer(*this)
{

}

void Server::Initialize()
{
    tcp::TcpSocket::Initialize();
    mServer.Start(10, true, 9620, 9621);
}

void Server::Stop()
{
    mServer.Stop();
}

void Server::NewConnection(const tcp::Conn &conn)
{
    (void) conn;
}

void Server::ReadData(const tcp::Conn &conn)
{
    tcp::TcpSocket::Send(mCurrentGames, conn.peer);

//    tcp::TcpSocket::Send("coucou", conn.peer);
}

void Server::ClientClosed(const tcp::Conn &conn)
{
    (void) conn;
}

void Server::ServerTerminated(tcp::TcpServer::IEvent::CloseType type)
{
    (void) type;
}

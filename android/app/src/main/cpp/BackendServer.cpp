//
// Created by anthony on 16/05/19.
//

#include "BackendServer.h"
#include "HttpFileServer.h"

BackendServer::BackendServer()
        : HttpFileServer(std::string())
{


}

BackendServer::~BackendServer()
{

}

void BackendServer::WsReadData(const tcp::Conn &conn)
{

}

bool BackendServer::ReadDataPath(const tcp::Conn &conn, const HttpRequest &request)
{
    return false;
}

void BackendServer::NewConnection(const tcp::Conn &conn)
{

}

void BackendServer::ClientClosed(const tcp::Conn &conn)
{

}

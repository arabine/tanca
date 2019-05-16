//
// Created by anthony on 16/05/19.
//

#ifndef ANDROID_BACKENDSERVER_H
#define ANDROID_BACKENDSERVER_H

// ICL
#include "HttpFileServer.h"

class BackendServer : public HttpFileServer
{
public:
    BackendServer();
    ~BackendServer();

    BackendServer(const std::string &rootDir);

private:
    void WsReadData(const tcp::Conn &conn);
    bool ReadDataPath(const tcp::Conn &conn, const HttpRequest &request);
    void NewConnection(const tcp::Conn &conn);
    void ClientClosed(const tcp::Conn &conn);
};


#endif //ANDROID_BACKENDSERVER_H

//
// Created by anthony on 16/05/19.
//

#include "BackendServer.h"
#include "HttpFileServer.h"
#include <android/log.h>

extern "C" {
const char *find_embedded_file(const char *name, size_t *size, const char **mime);
}

static const char* kTAG = "TancaLog";
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))

BackendServer::BackendServer()
        : HttpFileServer(std::string())
{


}

void BackendServer::Initialize()
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
    bool success = false;
    size_t size = 0;
    const char *mime;

    std::string query = request.query;
    if (query == "/")
    {
        query = "/index.html";
    }

    const char *fileContents = find_embedded_file(query.c_str(), &size, &mime);

    if (fileContents != nullptr)
    {
        success = true;

        LOGI("[TANCA] Serving file: %s", query.c_str());

        std::stringstream ss;
        std::string data(fileContents, size);

        ss << "HTTP/1.1 200 OK\r\n";
        ss << "Content-type: " << mime << "\r\n";
        ss << "Content-length: " << data.size() << "\r\n\r\n";
        ss << data;

        tcp::TcpSocket::SendToSocket(ss.str(), conn.peer);

    }
    else
    {
        LOGE("[TANCA] Cannot find file: %s", query.c_str());
    }
    return success;
}

void BackendServer::NewConnection(const tcp::Conn &conn)
{

}

void BackendServer::ClientClosed(const tcp::Conn &conn)
{

}

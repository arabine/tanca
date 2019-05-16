#[[
 CMake file to build ICL and Tanca source files on Android
]]

cmake_minimum_required(VERSION 3.4.1)
project(tanca_server LANGUAGES C CXX)

# New path definition
set(ICL_DIR ../../../../../src/icl)

set(ICL_DIR_UTILS   ${ICL_DIR}/util)
set(ICL_DIR_NET     ${ICL_DIR}/network)
set(ICL_DIR_ZIP     ${ICL_DIR}/zip)
set(ICL_DIR_SEC     ${ICL_DIR}/security)
set(ICL_DIR_JS      ${ICL_DIR}/jsengine)
set(ICL_DIR_PROTO   ${ICL_DIR}/protocol)
set(ICL_DIR_JSON    ${ICL_DIR}/json)
set(ICL_DIR_IO      ${ICL_DIR}/io)
set(ICL_DIR_DB      ${ICL_DIR}/db)


add_library(tanca_server SHARED
        BackendServer.cpp
        main_backend.cpp
            efs.c)

include_directories(${ICL_DIR_UTILS} ${ICL_DIR_NET} ${ICL_DIR_ZIP} ${ICL_DIR_SEC} ${ICL_DIR_JS} ${ICL_DIR_PROTO} ${ICL_DIR_JSON} ${ICL_DIR_IO} ${ICL_DIR_DB})

set(ICL_DIR_UTILS_SRC ${ICL_DIR_UTILS}/Util.cpp
                      ${ICL_DIR_UTILS}/Log.cpp
                      ${ICL_DIR_UTILS}/Value.cpp
                      ${ICL_DIR_UTILS}/UniqueId.cpp
                      ${ICL_DIR_UTILS}/Console.cpp
                      ${ICL_DIR_UTILS}/EventLoop.cpp)

set(ICL_DIR_NET_SRC ${ICL_DIR_NET}/TcpSocket.cpp
                    ${ICL_DIR_NET}/TcpServerEpoll.cpp
                    ${ICL_DIR_NET}/TcpClient.cpp
                    ${ICL_DIR_NET}/WebSocket.cpp
                    ${ICL_DIR_NET}/TcpServerBase.cpp)

set(ICL_DIR_ZIP_SRC ${ICL_DIR_ZIP}/Zip.cpp)


set(ICL_DIR_SEC_SRC ${ICL_DIR_SEC}/Base64.cpp
                    ${ICL_DIR_SEC}/Sha256.cpp
                    ${ICL_DIR_SEC}/Sha1.cpp)

set(ICL_DIR_JS_SRC  ${ICL_DIR_JS}/JSEngine.cpp
                    ${ICL_DIR_JS}/duktape.c)

set(ICL_DIR_PROTO_SRC ${ICL_DIR_PROTO}/Http.cpp
                      ${ICL_DIR_PROTO}/HttpFileServer.cpp)

set(ICL_DIR_JSON_SRC    ${ICL_DIR_JSON}/JsonWriter.cpp
                        ${ICL_DIR_JSON}/JsonReader.cpp
                        ${ICL_DIR_JSON}/JsonValue.cpp)

set(ICL_DIR_IO_SRC  ${ICL_DIR_IO}/ByteArray.cpp
                    ${ICL_DIR_IO}/ByteStreamReader.cpp
                    ${ICL_DIR_IO}/ByteStreamWriter.cpp)

set(ICL_DIR_DB_SRC  ${ICL_DIR_DB}/DataBase.cpp
                    ${ICL_DIR_DB}/sqlite3.c)

# Add the ICL library and specify the source files to build
add_library(icl SHARED  ${ICL_DIR_UTILS_SRC}
                        ${ICL_DIR_NET_SRC}
                        ${ICL_DIR_SEC_SRC}
                        ${ICL_DIR_ZIP_SRC}
                        ${ICL_DIR_JS_SRC}
                        ${ICL_DIR_PROTO_SRC}
                        ${ICL_DIR_JSON_SRC}
                        ${ICL_DIR_IO_SRC}
                        ${ICL_DIR_DB_SRC})


target_link_libraries(icl)

# Include libraries needed for TarotClub lib
target_link_libraries(tanca_server
                      android
                      icl
                      log)

# End of Cmake file

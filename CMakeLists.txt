cmake_minimum_required(VERSION 3.31)
project(KeyDB)

set(CMAKE_CXX_STANDARD 20)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(KeyDB main.cpp
        connection/Connection.cpp
        connection/Connection.h
        command_parser/CommandParser.cpp
        command_parser/CommandParser.h
        inmem_database/InMemoryDB.cpp
        inmem_database/InMemoryDB.h
        handlers/handleClientMessage.cpp
        handlers/handleClientMessage.h
        result/Result.h
        session/Session.cpp
        session/Session.h
        resp_parser/RespParser.h
        resp_parser/RespObject.h
        resp_parser/RespObject.cpp
        resp_parser/RespParser.cpp
)

target_link_libraries(KeyDB PRIVATE Threads::Threads)

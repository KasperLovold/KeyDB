cmake_minimum_required(VERSION 3.31)
project(KeyDB)

set(CMAKE_CXX_STANDARD 20)

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
)

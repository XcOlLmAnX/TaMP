cmake_minimum_required(VERSION 3.16)
project(TaMP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.11.0/mingw_64")

find_package(Qt6 REQUIRED COMPONENTS Core Network Sql)

qt_standard_project_setup()

add_executable(TaMP
        main.cpp
        protocol.cpp
        db_handler.cpp
        server_logic.cpp
        server_interface.cpp
        smtpclient_mailru.cpp
        smtpclient_mailru.h
)

target_include_directories(TaMP PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(TaMP PRIVATE
        Qt6::Core
        Qt6::Network
        Qt6::Sql
        ws2_32
)

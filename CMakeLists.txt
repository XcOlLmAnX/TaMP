cmake_minimum_required(VERSION 3.16)
project(TaMP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Путь к Qt

set(CMAKE_PREFIX_PATH “C:/Qt/6.11.0/mingw_64”)

add_executable(TaMP
        main.cpp
        protocol.cpp
        db_handler.cpp
        server_logic.cpp
        server_interface.cpp
)

target_include_directories(TaMP PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(TaMP PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/libsqlite3.a
        ws2_32
)
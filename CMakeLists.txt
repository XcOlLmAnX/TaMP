cmake_minimum_required(VERSION 3.16)
project(TaMP_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.11.0/mingw_64")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network PrintSupport)

add_executable(TaMP_client
        main.cpp
        tcpclient.cpp
        loginwindow.cpp
        registerwindow.cpp
        taskwindow.cpp
        qcustomplot.cpp
        # statwindow.cpp  <- раскомментируй когда добавим
)

# QCustomPlot слишком большой для debug режима — O1 уменьшает объектный файл
set_source_files_properties(qcustomplot.cpp PROPERTIES COMPILE_FLAGS "-w -O1")

target_include_directories(TaMP_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(TaMP_client PRIVATE
        Qt6::Widgets
        Qt6::Network
        Qt6::PrintSupport
)
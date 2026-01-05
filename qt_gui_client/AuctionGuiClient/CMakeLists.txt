cmake_minimum_required(VERSION 3.16)

project(AuctionGuiClient VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(SRC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

# ---- UI file: support both root/mainwindow.ui and Forms/mainwindow.ui
set(UI_FILE "${SRC_ROOT}/mainwindow.ui")
if(EXISTS "${SRC_ROOT}/Forms/mainwindow.ui")
    set(UI_FILE "${SRC_ROOT}/Forms/mainwindow.ui")
endif()

set(CMAKE_AUTOUIC_SEARCH_PATHS
    "${SRC_ROOT}"
    "${SRC_ROOT}/Forms"
)

set(PROJECT_SOURCES
    "${SRC_ROOT}/main.cpp"
    "${SRC_ROOT}/mainwindow.cpp"
    "${SRC_ROOT}/mainwindow.h"
    "${SRC_ROOT}/socketclient.cpp"
    "${SRC_ROOT}/socketclient.h"
    "${UI_FILE}"
    "${SRC_ROOT}/authwindow.cpp"
    "${SRC_ROOT}/authwindow.h"
)

foreach(f IN LISTS PROJECT_SOURCES)
    if(NOT EXISTS "${f}")
        message(FATAL_ERROR "Missing file: ${f}")
    endif()
endforeach()

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(AuctionGuiClient
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(AuctionGuiClient
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(AuctionGuiClient PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

target_include_directories(AuctionGuiClient PRIVATE
    "${SRC_ROOT}"
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(AuctionGuiClient)
endif()

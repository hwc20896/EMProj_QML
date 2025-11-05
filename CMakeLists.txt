cmake_minimum_required(VERSION 3.29)
project(EMProj_QML)

set(CMAKE_CXX_STANDARD 23)

find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick Multimedia)

set(FILES
    main.cpp
    resources.qrc
    src/headers/managementbackend.hpp
    src/cpp/managementbackend.cpp
        src/headers/questiondata.hpp
    src/cpp/database.cpp
    src/headers/database.hpp
)

qt_standard_project_setup()

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    qt_add_executable(EMProj_QML WIN32 ${FILES})
else()
    qt_add_executable(EMProj_QML ${FILES})
endif()

set_target_properties(EMProj_QML PROPERTIES
    AUTORCC ON
)

target_include_directories(EMProj_QML PRIVATE
    src/headers
)

target_link_libraries(
    EMProj_QML PRIVATE
        Qt6::Core
        Qt6::Qml
        Qt6::Quick
        Qt6::Multimedia
        ${CMAKE_BINARY_DIR}/duckdb.lib
)
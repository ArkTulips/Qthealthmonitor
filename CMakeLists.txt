cmake_minimum_required(VERSION 3.16)

project(QtHealthMonitor)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)

qt_standard_project_setup()

qt_add_executable(QtHealthMonitor
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    asynctask.cpp
    asynctask.h
)

target_link_libraries(QtHealthMonitor
    PRIVATE Qt6::Widgets Qt6::Charts
)
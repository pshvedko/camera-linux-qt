cmake_minimum_required(VERSION 3.4)

project(camera LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS
        Widgets
        Network
        Multimedia
        MultimediaWidgets
        LinguistTools
        REQUIRED)

find_package(ALSA REQUIRED)

set(TS_FILES camera_ru_RU.ts)

add_executable(camera
        Camera.cpp
        main.cpp
        main.qrc
#       ${TS_FILES}
        Surface.cpp
        QJsonWebToken.cpp
        QrCode.cpp
        Show.cpp
        Capture.cpp
        Queue.cpp
        Frame.cpp
        Loop.cpp
        Stream.cpp
        Audio.cpp
        Play.cpp
        Video.cpp
        Pulse.cpp)

target_link_libraries(camera PRIVATE
        Qt5::Widgets
        Qt5::Network
        Qt5::Multimedia
        Qt5::MultimediaWidgets
        vpx
        opus
        ALSA::ALSA
        pulse
        yuv
        jpeg)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

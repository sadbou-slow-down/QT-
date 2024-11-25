cmake_minimum_required(VERSION 3.5)

project(video_player VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(video_player
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET video_player APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(video_player SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(video_player
            ${PROJECT_SOURCES}
            video_play.h video_play.cpp

        )
    endif()
endif()

# FFmpeg and SDL2 settings
set(FFMPEG_INCLUDE_DIR "D:\\Tools\\FFmpeg\\ffmpeg-6.1.1-full_build-shared\\include")
set(FFMPEG_LIB_DIR "D:\\Tools\\FFmpeg\\ffmpeg-6.1.1-full_build-shared\\lib")
set(SDL_INCLUDE_DIR "D:\\Tools\\SDL\\SDL2-devel-2.0.14-mingw\\SDL2-2.0.14\\x86_64-w64-mingw32\\include")
set(SDL_LIB_DIR "D:\\Tools\\SDL\\SDL2-devel-2.0.14-mingw\\SDL2-2.0.14\\x86_64-w64-mingw32\\lib")

# Include directories and link libraries
target_include_directories(video_player PRIVATE ${FFMPEG_INCLUDE_DIR} ${SDL_INCLUDE_DIR})
target_link_directories(video_player PRIVATE ${FFMPEG_LIB_DIR} ${SDL_LIB_DIR})
target_link_libraries(video_player PRIVATE Qt${QT_VERSION_MAJOR}::Widgets avcodec avformat avutil swscale SDL2)



# target_link_libraries(video_player PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.video_player)
endif()
set_target_properties(video_player PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS video_player
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(video_player)
endif()


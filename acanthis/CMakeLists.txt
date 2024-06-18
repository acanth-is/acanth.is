# SPDX-FileCopyrightText: 2021 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

set(projectName acanthis)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Qt 6.4.3 required for sane TreeView handling, but sticking
# with 6.4.2 for the moment as this is what debian stable supports
find_package(Qt6 6.4.2 COMPONENTS Core Qml Quick Gui OpenGL Widgets REQUIRED)
find_package(OpenGL REQUIRED)

add_compile_definitions(_ACANTHIS)

set(acanthis_HDRS
    coreapplication.h
    settingsimpl.h
    settings.h
    documentmanager.h
    graphmodel.h
    graphviewmodel.h
    interfaceversion.h
    treeitem.h
    maplayer.h
    shapemaplayer.h
    shapegraphlayer.h
    pixelmaplayer.h
    aqmapviewmodel.h
    attributeitem.h
)

set(acanthis_SRCS
    main.cpp
    coreapplication.cpp
    settingsimpl.cpp
    documentmanager.cpp
    graphmodel.cpp
    shapemaplayer.cpp
    shapegraphlayer.cpp
    pixelmaplayer.cpp
    aqmapviewmodel.cpp
)

qt6_add_resources(acanthis_RSRC resource.qrc dialogs/settings/settingsdialog.qrc)

set(projectFiles ${acanthis_HDRS} ${acanthis_SRCS} ${acanthis_RSRC})

if (WIN32)
    add_executable(${projectName} WIN32 ${projectFiles} icons.rc)
    if(MINGW)
        # https://stackoverflow.com/a/18138926
        # MinGW compilation does not bring in the required DLLs to make the executable self-contained
        # The missing DLLs are:
        # - libstdc++-6.dll
        # - libwinpthread-1.dll
        # - libgcc_s_seh-1.dll
        # Additionally, windeployqt does not drag them into the build directory, so it might be
        # necessary to manually find and copy them after compilation
        set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ -lwsock32 -lws2_32 ${CMAKE_CXX_STANDARD_LIBRARIES}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
    endif(MINGW)
endif(WIN32)
if(UNIX AND NOT APPLE)
    # qml6-module-qt-labs-settings qml6-module-qtquick-dialogs qml6-module-qtquick-templates
    # qml6-module-qtquick-window qml6-module-qtqml qt6-base-dev qt6-declarative-dev
    # libglu1-mesa-dev
    add_executable(${projectName} ${projectFiles})
    if (Qt6_POSITION_INDEPENDENT_CODE)
        SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
    endif()

    set_target_properties(${depthmapX} PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wpedantic -Weverything")
endif()
if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE acanthis.icns)
    set(acanthis_ICON ${CMAKE_CURRENT_SOURCE_DIR}/icons/acanthis.icns)
    set_source_files_properties(${acanthis_ICON} PROPERTIES
           MACOSX_PACKAGE_LOCATION "Resources")
    set(graph_ICON ${CMAKE_CURRENT_SOURCE_DIR}/icons/graph.icns)
    set_source_files_properties(${graph_ICON} PROPERTIES
           MACOSX_PACKAGE_LOCATION "Resources")
    add_definitions(-DGL_SILENCE_DEPRECATION)
    add_executable(${projectName} MACOSX_BUNDLE ${acanthis_ICON} ${graph_ICON}
        ${projectFiles})
    set_target_properties(${projectName} PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/resources/Info.plist)
endif(APPLE)

target_compile_options(${projectName} PRIVATE ${COMPILE_WARNINGS})

set_target_properties(${projectName} PROPERTIES
    QT_QML_MODULE_VERSION 1.0
    QT_QML_MODULE_URI acanthis
)

target_compile_features(${projectName} PRIVATE cxx_std_17)

find_package(OpenGL REQUIRED)

target_link_libraries(${projectName} salalib genlib Qt6::Core Qt6::Gui
    Qt6::Qml Qt6::Quick Qt6::OpenGL Qt6::Widgets
    OpenGL::GL OpenGL::GLU ${modules_gui} ${modules_core})

add_subdirectory(dialogs)
set(CMAKE_AUTOUIC_SEARCH_PATHS dialogs)
add_subdirectory(agl)

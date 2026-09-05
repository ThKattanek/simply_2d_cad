# Simply 2D CAD
# Copyright (C) 2026 Thorsten Kattanek
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

cmake_minimum_required(VERSION 3.19)

# Stummschalten der MXE-spezifischen Policy-Warnungen
if(CMAKE_POLICY_DEFAULT_CMP0017)
    set(dummy_cmp0017 ${CMAKE_POLICY_DEFAULT_CMP0017})
endif()
if(CMAKE_POLICY_DEFAULT_CMP0020)
    set(dummy_cmp0020 ${CMAKE_POLICY_DEFAULT_CMP0020})
endif()

# --- Hier wird der Projektname ZENTRAL definiert ---
project(simply_2d_cad LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools Svg)

qt_standard_project_setup()

# --- 1. Target für 3rdparty: dxflib ---
set(DXFLIB_SOURCES
    3rdparty/dxflib/dl_dxf.cpp
    3rdparty/dxflib/dl_writer_ascii.cpp
)

add_library(dxflib STATIC ${DXFLIB_SOURCES})
target_compile_options(dxflib PRIVATE -w)
target_include_directories(dxflib SYSTEM PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/dxflib
)

# --- 2. Haupt-Anwendung: Dynamisch über ${PROJECT_NAME} ---
set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp src/mainwindow.h
    src/mainwindow.ui
    src/cad_view.h src/cad_view.cpp
    src/cad_scene.h src/cad_scene.cpp
    src/cad_tool.h
    src/cad_tool_manager.h src/cad_tool_manager.cpp
    src/line_tool.h src/line_tool.cpp
    src/select_tool.h src/select_tool.cpp
    src/crosshair_item.h src/crosshair_item.cpp
    src/cad_point_item.h src/cad_point_item.cpp
    src/point_tool.h src/point_tool.cpp
    src/cad_document/cad_entity.h
    src/cad_document/cad_line.h
    src/cad_document/cad_point.h
    src/cad_document/cad_document.h src/cad_document/cad_document.cpp
    src/dxf_manager.h src/dxf_manager.cpp
    src/snap_types.h
    src/snap_manager.h src/snap_manager.cpp
    src/app_settings_dialog.h src/app_settings_dialog.cpp src/app_settings_dialog.ui
)

qt_add_resources(MY_RESOURCES src/resources.qrc)

# Verwendet automatisch "simply_2d_cad"
qt_add_executable(${PROJECT_NAME}
    WIN32 MACOSX_BUNDLE
    ${PROJECT_SOURCES}
    ${MY_RESOURCES}
)

# Strenge Compiler-Regeln für dein Target
target_compile_options(${PROJECT_NAME} PRIVATE
    -O2
    -Wall
    -Wextra
    -Werror
    -pedantic
)

# Optional: Übergibt den Projektnamen als C++ Macro an deinen Quellcode
target_compile_definitions(${PROJECT_NAME} PRIVATE
    APP_NAME="${PROJECT_NAME}"
)

# Bibliotheken verlinken
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Svg
        dxflib
)

# Übersetzungen
qt_add_translations(
    TARGETS ${PROJECT_NAME}
    TS_FILES    src/simply_2d_cad_de_DE.ts
                src/simply_2d_cad_en_US.ts
)

# Installation & Deployment
install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
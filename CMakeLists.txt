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

project(simply_2d_cad LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools Svg)

qt_standard_project_setup()

# --- 2. Haupt-Anwendung: Dynamisch über ${PROJECT_NAME} ---
set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp src/mainwindow.h src/mainwindow.ui
    src/app_settings_dialog.h src/app_settings_dialog.cpp src/app_settings_dialog.ui
    # CAD view and scene
    src/cad_view.h src/cad_view.cpp
    src/cad_scene.h src/cad_scene.cpp
    # scene items (crosshair, cad point, etc.)
    src/crosshair_item.h src/crosshair_item.cpp
    src/cad_point_item.h src/cad_point_item.cpp
    # cad document and entities
    src/cad_document/cad_document.h src/cad_document/cad_document.cpp
    src/cad_document/cad_entity.h
    src/cad_document/cad_line.h
    src/cad_document/cad_point.h
    src/cad_document/cad_circle.h
    # cad construction entities
    src/cad_document/cad_construction_hv_line.h
    src/cad_document/cad_construction_line.h
    # undo/redo command system
    src/undo_stack.h src/undo_stack.cpp
    src/commands/command.h
    src/commands/macro_command.h
    src/commands/add_entity_command.h src/commands/add_entity_command.cpp
    src/commands/remove_entity_command.h src/commands/remove_entity_command.cpp
    # DXF import/export
    src/dxf_manager.h src/dxf_manager.cpp
    # snap system
    src/snap_types.h
    src/snap_manager.h src/snap_manager.cpp
    src/snap_marker_point_item.h src/snap_marker_point_item.cpp
    src/snap_marker_endpoint_item.h src/snap_marker_endpoint_item.cpp
    src/snap_marker_intersection_item.h src/snap_marker_intersection_item.cpp
    src/snap_marker_midpoint_item.h src/snap_marker_midpoint_item.cpp
    src/snap_marker_tangent_item.h src/snap_marker_tangent_item.cpp
    src/snap_marker_perpendicular_item.h src/snap_marker_perpendicular_item.cpp
    # all cad tools (point, line, select, etc.)
    src/cad_tools/cad_tool.h
    src/cad_tools/cad_tool_manager.h src/cad_tools/cad_tool_manager.cpp
    src/cad_tools/select_tool.h src/cad_tools/select_tool.cpp
    src/cad_tools/point_tool.h src/cad_tools/point_tool.cpp
    src/cad_tools/line_tool.h src/cad_tools/line_tool.cpp
    src/cad_tools/parallel_line_tool.h src/cad_tools/parallel_line_tool.cpp
    src/cad_tools/circle_tool.h src/cad_tools/circle_tool.cpp
    src/cad_tools/rectangle_tool.h src/cad_tools/rectangle_tool.cpp
    # all construction tools (construction line, construction hv line, etc.)
    src/cad_tools/construction_hv_line_tool.h src/cad_tools/construction_hv_line_tool.cpp
    src/cad_tools/construction_line_tool.h src/cad_tools/construction_line_tool.cpp
    src/cad_tools/parallel_construction_line_tool.h src/cad_tools/parallel_construction_line_tool.cpp
    src/cad_tools/perpendicular_construction_line_tool.h src/cad_tools/perpendicular_construction_line_tool.cpp
)

qt_add_resources(MY_RESOURCES src/resources.qrc)

set(DXFLIB_SOURCES
    3rdparty/dxflib/dl_dxf.cpp
    3rdparty/dxflib/dl_writer_ascii.cpp
)

add_library(dxflib STATIC ${DXFLIB_SOURCES})
target_compile_options(dxflib PRIVATE -w)
target_include_directories(dxflib SYSTEM PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/dxflib
)

qt_add_executable(${PROJECT_NAME}
    WIN32 MACOSX_BUNDLE
    ${PROJECT_SOURCES}
    ${MY_RESOURCES}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -O2
    -Wall
    -Wextra
    -Werror
    -pedantic
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    APP_NAME="${PROJECT_NAME}"
)

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
    LUPDATE_OPTIONS
        -no-obsolete
        -extensions cpp,h,ui
)

# --- Handbuch-Target (Asciidoctor + Python) ---
find_package(Python3 COMPONENTS Interpreter)

if(Python3_FOUND)
    add_custom_target(generate_docs
        COMMAND Python3::Interpreter ${CMAKE_SOURCE_DIR}/doc/scripts/build_docs.py "${CMAKE_BINARY_DIR}/bin/doc"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generiere Dokumentation (PDFs) im Build-Verzeichnis..."
    )
    add_dependencies(${PROJECT_NAME} generate_docs)
endif()

# Platformspezifischer Installationspfad für die Dokumentation
if(WIN32)
    set(DOC_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}/doc")
    set(DOC_DIR_INSTALL_VAL "${CMAKE_INSTALL_PREFIX}/${DOC_INSTALL_DIR}")
elseif(APPLE)
    set(DOC_INSTALL_DIR "${PROJECT_NAME}.app/Contents/Resources/doc")
    set(DOC_DIR_INSTALL_VAL "${DOC_INSTALL_DIR}")
else() # Linux / UNIX FHS Standard
    set(DOC_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/doc/${PROJECT_NAME}")
    # CMAKE_INSTALL_FULL_DOCDIR liefert den ABSOLUTEN Pfad (z.B. /usr/local/share/doc/simply_2d_cad)
    set(DOC_DIR_INSTALL_VAL "${CMAKE_INSTALL_FULL_DOCDIR}")
endif()

# Definiere den relativen oder absoluten Pfad für den Quell-/Buildcode
target_compile_definitions(simply_2d_cad PRIVATE
    DOC_DIR_BUILD="${CMAKE_BINARY_DIR}/bin/doc"
    DOC_DIR_INSTALL="${DOC_INSTALL_DIR}"
)

# Installation der erzeugten PDFs
install(FILES
    ${CMAKE_BINARY_DIR}/bin/doc/simply_2d_cad_manual_de.pdf
    ${CMAKE_BINARY_DIR}/bin/doc/simply_2d_cad_manual_en.pdf
    DESTINATION ${DOC_INSTALL_DIR}
    OPTIONAL
)

# Installation der Hauptanwendung (Standard FHS)
install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Deployment-Skript NUR für Windows (MXE) oder macOS aktivieren,
# da Linux bei System-Installationen die Qt-Bibliotheken vom System nutzt.
if(WIN32 OR APPLE)
    qt_generate_deploy_app_script(
        TARGET ${PROJECT_NAME}
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif()

# --- CPack Konfiguration (Erstellt ZIP oder NSIS Installer unter Linux) ---
set(CPACK_PACKAGE_NAME "Simply2DCAD")
set(CPACK_PACKAGE_VENDOR "Thorsten Kattanek")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")

if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "Simply 2D CAD")
    set(CPACK_NSIS_PACKAGE_NAME "Simply 2D CAD")
else()
    set(CPACK_GENERATOR "TGZ;DEB")
endif()

include(CPack)
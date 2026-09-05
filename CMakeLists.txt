cmake_minimum_required(VERSION 3.19)
project(simply_2d_cad LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra -Werror -pedantic")

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools Svg)

qt_standard_project_setup()

set(DXFLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/dxflib)
include_directories(${DXFLIB_INCLUDE_DIR})

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
)

set(DXFLIB_SOURCES
    3rdparty/dxflib/dl_dxf.cpp
    3rdparty/dxflib/dl_writer_ascii.cpp
)

qt_add_resources(MY_RESOURCES src/resources.qrc)

qt_add_executable(simply_2d_cad
    WIN32 MACOSX_BUNDLE
    ${PROJECT_SOURCES}
    ${DXFLIB_SOURCES}
    ${MY_RESOURCES}
    src/snap_types.h
    src/snap_manager.h src/snap_manager.cpp
)

qt_add_translations(
    TARGETS simply_2d_cad
    TS_FILES    src/simply_2d_cad_de_DE.ts
                src/simply_2d_cad_en_US.ts
)

target_link_libraries(simply_2d_cad
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Svg
)

install(TARGETS simply_2d_cad
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET simply_2d_cad
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

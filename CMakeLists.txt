cmake_minimum_required(VERSION 3.19)
project(simply_2d_cad LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)

qt_standard_project_setup()

qt_add_executable(simply_2d_cad
    WIN32 MACOSX_BUNDLE
    src/main.cpp
    src/mainwindow.cpp src/mainwindow.h
    src/mainwindow.ui
    src/cadview.h src/cadview.cpp
    src/cadscene.h src/cadscene.cpp
    src/cadtool.h
    src/cadtoolmanager.h src/cadtoolmanager.cpp
    src/linetool.h src/linetool.cpp
    src/selecttool.h src/selecttool.cpp
    src/crosshairitem.h src/crosshairitem.cpp
    src/cadpointitem.h src/cadpointitem.cpp
    src/pointtool.h src/pointtool.cpp
    src/cadentity.h
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

cmake_minimum_required(VERSION 3.19)
project(simply_2d_cad LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)

qt_standard_project_setup()

qt_add_executable(simply_2d_cad
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

qt_add_translations(
    TARGETS simply_2d_cad
    TS_FILES simply_2d_cad_de_DE.ts
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

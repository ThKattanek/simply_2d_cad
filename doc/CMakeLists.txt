# doc/CMakeLists.txt

# 1. Alle AsciiDoc-Quellen auflisten
set(DOC_SOURCES
    manual/pdf-theme.yml
    manual/de/main.adoc manual/en/main.adoc
    manual/de/tools/line_2points.adoc # manual/en/tools/line_2points.adoc
)

# 2. Python suchen & Dokumentations-Target definieren
find_package(Python3 COMPONENTS Interpreter)

if(Python3_FOUND)
    add_custom_target(generate_docs
        COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/scripts/build_docs.py "${CMAKE_BINARY_DIR}/bin/doc"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        SOURCES ${DOC_SOURCES}
        COMMENT "Generiere Dokumentation (PDFs) im Build-Verzeichnis..."
    )

    # Verknüpfung mit dem Haupt-Target herstellen
    add_dependencies(${PROJECT_NAME} generate_docs)
endif()

# 3. Installation der erzeugten PDFs definieren
install(FILES
    ${CMAKE_BINARY_DIR}/bin/doc/simply_2d_cad_manual_de.pdf
    ${CMAKE_BINARY_DIR}/bin/doc/simply_2d_cad_manual_en.pdf
    DESTINATION ${DOC_INSTALL_DIR}
    OPTIONAL
)

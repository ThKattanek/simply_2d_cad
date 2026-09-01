/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "./dxf_manager.h"
#include "dl_dxf.h"

#include "./cad_document/cad_point.h"
#include "./cad_document/cad_line.h"

// ==========================================
// IMPORT
// ==========================================

bool DxfManager::importFile(const QString& filePath, DxfData& outData) {
    outData.clear();

    DL_Dxf dxf;
    DxfImportAdapter adapter(outData);

    // DXF einlesen (toLocal8Bit() stellt korrekte Pfad-Zeichenkodierung sicher)
    return dxf.in(filePath.toLocal8Bit().constData(), &adapter);
}

bool DxfManager::importEntities(const QString& filePath,
                                std::vector<std::unique_ptr<CadEntity>>& outEntities)
{
    DxfData importedData;

    // 1. Rohdaten per dxflib einlesen
    if (!importFile(filePath, importedData)) {
        return false;
    }

    // 2. Rohdaten in konkrete CadEntities umwandeln
    outEntities.reserve(outEntities.size() + importedData.points.size() + importedData.lines.size());

    for (const auto& pt : importedData.points) {
        outEntities.push_back(std::make_unique<CadPoint>(pt));
    }

    for (const auto& line : importedData.lines) {
        outEntities.push_back(std::make_unique<CadLine>(line.p1(), line.p2()));
    }

    return true;
}

// ==========================================
// EXPORT
// ==========================================

bool DxfManager::exportEntities(const QString& filePath,
                                const std::vector<std::unique_ptr<CadEntity>>& entities)
{
    DxfData exportData;

    // Entities nach Typ trennen und in die DxfData-Struktur konvertieren
    for (const auto& entityPtr : entities) {
        if (!entityPtr) continue;

        switch (entityPtr->type()) {
        case EntityType::Point: {
            auto* pointEntity = static_cast<const CadPoint*>(entityPtr.get());
            exportData.points.push_back(pointEntity->position());
            break;
        }
        case EntityType::Line: {
            auto* lineEntity = static_cast<const CadLine*>(entityPtr.get());
            exportData.lines.emplace_back(lineEntity->start(), lineEntity->end());
            break;
        }
        default:
            // Nicht unterstützte Typen vorerst ignorieren
            break;
        }
    }

    return exportFile(filePath, exportData);
}

bool DxfManager::exportFile(const QString& filePath, const DxfData& data) {
    DL_Dxf dxf;
    DL_Codes::version exportVersion = DL_Codes::AC1015; // AutoCAD 2000 Format

    // Writer für die Datei erstellen
    DL_WriterA* dw = dxf.out(filePath.toLocal8Bit().constData(), exportVersion);
    if (!dw) {
        return false;
    }

    // 1. HEADER SEKTION
    dxf.writeHeader(*dw);
    dw->sectionEnd();

    // 2. TABLES SEKTION
    dw->sectionTables();
    dxf.writeVPort(*dw);

    // Linetypes Tabelle
    dw->tableLinetypes(3);
    dxf.writeLinetype(*dw, DL_LinetypeData("BYBLOCK", "BYBLOCK", 0, 0, 0.0));
    dxf.writeLinetype(*dw, DL_LinetypeData("BYLAYER", "BYLAYER", 0, 0, 0.0));
    dxf.writeLinetype(*dw, DL_LinetypeData("CONTINUOUS", "Continuous", 0, 0, 0.0));
    dw->tableEnd();

    // Layer Tabelle (Standard-Layer "0")
    int numberOfLayers = 1;
    dw->tableLayers(numberOfLayers);
    dxf.writeLayer(*dw,
                   DL_LayerData("0", 0),
                   DL_Attributes(
                       std::string(""),
                       DL_Codes::black,
                       100,
                       "CONTINUOUS", 1.0));
    dw->tableEnd();

    // Textstyles, View, UCS, AppID
    dw->tableStyle(1);
    dxf.writeStyle(*dw, DL_StyleData("standard", 0, 2.5, 1.0, 0.0, 0, 2.5, "txt", ""));
    dw->tableEnd();

    dxf.writeView(*dw);
    dxf.writeUcs(*dw);

    dw->tableAppid(1);
    dxf.writeAppid(*dw, "ACAD");
    dw->tableEnd();

    dxf.writeDimStyle(*dw, 1, 1, 1, 1, 1);

    // Block Records
    dxf.writeBlockRecord(*dw);
    dw->tableEnd();

    dw->sectionEnd();

    // 3. BLOCKS SEKTION
    dw->sectionBlocks();
    dxf.writeBlock(*dw, DL_BlockData("*Model_Space", 0, 0.0, 0.0, 0.0));
    dxf.writeEndBlock(*dw, "*Model_Space");
    dxf.writeBlock(*dw, DL_BlockData("*Paper_Space", 0, 0.0, 0.0, 0.0));
    dxf.writeEndBlock(*dw, "*Paper_Space");
    dxf.writeBlock(*dw, DL_BlockData("*Paper_Space0", 0, 0.0, 0.0, 0.0));
    dxf.writeEndBlock(*dw, "*Paper_Space0");
    dw->sectionEnd();

    // 4. ENTITIES SEKTION (Geometrien)
    dw->sectionEntities();

    // Standard-Attribute für Layer "0"
    DL_Attributes defaultAttribs("0", 256, -1, "BYLAYER", 1.0);

    // --- Punkte exportieren ---
    for (const auto& pt : data.points) {
        dxf.writePoint(*dw,
                       DL_PointData(pt.x(), pt.y(), 0.0),
                       defaultAttribs);
    }

    // --- Linien exportieren ---
    for (const auto& line : data.lines) {
        dxf.writeLine(*dw,
                      DL_LineData(line.x1(), line.y1(), 0.0,
                                  line.x2(), line.y2(), 0.0),
                      defaultAttribs);
    }

    dw->sectionEnd();

    // 5. OBJECTS SEKTION & ABSCHLUSS
    dxf.writeObjects(*dw);
    dxf.writeObjectsEnd(*dw);

    dw->dxfEOF();
    dw->close();
    delete dw;

    return true;
}
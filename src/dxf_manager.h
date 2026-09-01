/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

#include <QString>
#include <QPointF>
#include <QLineF>
#include <vector>

#include "dl_creationadapter.h"
#include "./cad_document//cad_entity.h"

// Reine Datenstruktur für dxflib (Zwischenspeicher)
struct DxfData {
    std::vector<QPointF> points;
    std::vector<QLineF> lines;

    void clear() {
        points.clear();
        lines.clear();
    }
};

// Adapter-Klasse für den dxflib-Import
class DxfImportAdapter : public DL_CreationAdapter {
public:
    explicit DxfImportAdapter(DxfData& targetData) : m_data(targetData) {}

    void addPoint(const DL_PointData& data) override {
        m_data.points.emplace_back(data.x, data.y);
    }

    void addLine(const DL_LineData& data) override {
        m_data.lines.emplace_back(QPointF(data.x1, data.y1), QPointF(data.x2, data.y2));
    }

private:
    DxfData& m_data;
};

// Hauptklasse für DXF-Import und -Export
class DxfManager {
public:
    DxfManager() = default;

    // --- IMPORT ---
    // Liest eine DXF-Datei in das DxfData-Zwischenformat ein
    static bool importFile(const QString& filePath, DxfData& outData);

    // Liest eine DXF-Datei ein und liefert direkt eine Liste ersteller CadEntities zurück
    static bool importEntities(const QString& filePath,
                               std::vector<std::unique_ptr<CadEntity>>& outEntities);

    // --- EXPORT ---
    // Exportiert Rohdaten (Punkte & Linien)
    static bool exportFile(const QString& filePath, const DxfData& data);

    // Exportiert direkt die Liste abstrakter CadEntities aus dem CadDocument
    static bool exportEntities(const QString& filePath,
                               const std::vector<std::unique_ptr<CadEntity>>& entities);
};
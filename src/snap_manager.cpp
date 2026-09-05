/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "./snap_manager.h"
#include "./cad_document/cad_document.h"
#include "./cad_document/cad_line.h"
#include "./cad_document/cad_point.h"

SnapResult SnapManager::findSnapPoint(const QPointF& mouseWorldPos, const CadDocument& doc, double zoomFactor)
{
    SnapResult bestSnap;

    // Bildschirm-Toleranz (Pixel) in Weltkoordinaten umrechnen (Zoom-abhängig)
    const double maxDistWorld = m_tolerancePixels / zoomFactor;
    double minDistance = maxDistWorld;

    auto checkPoint = [&](const QPointF& candidate, SnapType type) {
        double dist = std::hypot(candidate.x() - mouseWorldPos.x(),
                                 candidate.y() - mouseWorldPos.y());
        if (dist < minDistance) {
            minDistance = dist;
            bestSnap.snapped = true;
            bestSnap.point = candidate;
            bestSnap.type = type;
            bestSnap.distance = dist;
        }
    };

    // Durch alle Entities im Dokument iterieren
    for (const auto& entityPtr : doc.getEntities()) {
        if (!entityPtr) continue;

        switch (entityPtr->type()) {
        case EntityType::Point: {
            auto* pt = static_cast<const CadPoint*>(entityPtr.get());
            checkPoint(pt->position(), SnapType::Endpoint);
            break;
        }
        case EntityType::Line: {
            auto* line = static_cast<const CadLine*>(entityPtr.get());
            // Endpunkte fangen
            checkPoint(line->start(), SnapType::Endpoint);
            checkPoint(line->end(), SnapType::Endpoint);

            // Optional: Mittelpunkt fangen
            QPointF midPoint = (line->start() + line->end()) * 0.5;
            checkPoint(midPoint, SnapType::Midpoint);
            break;
        }
        default:
            break;
        }
    }

    return bestSnap;
}

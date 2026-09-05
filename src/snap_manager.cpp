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

    const auto& entities = doc.getEntities();

    // 1. DURCHGANG: Endpunkte & Mittelpunkte & Punkte
    if(m_endpointSnapEnabled || m_midpointSnapEnabled || m_pointSnapEnabled) {
        for (const auto& entityPtr : entities) {
            if (!entityPtr) continue;

            switch (entityPtr->type()) {
            case EntityType::Point: {
                auto* pt = static_cast<const CadPoint*>(entityPtr.get());
                if(m_pointSnapEnabled)
                    checkPoint(pt->position(), SnapType::Point);
                break;
            }
            case EntityType::Line: {
                auto* line = static_cast<const CadLine*>(entityPtr.get());
                // Endpunkte fangen
                if (m_endpointSnapEnabled) {
                checkPoint(line->start(), SnapType::Endpoint);
                checkPoint(line->end(), SnapType::Endpoint);
                }

                // Optional: Mittelpunkt fangen
                QPointF midPoint = (line->start() + line->end()) * 0.5;
                if (m_midpointSnapEnabled)
                    checkPoint(midPoint, SnapType::Midpoint);
                break;
            }
            default:
                break;
            }
        }
    }

    // 2. DURCHGANG: Schnittpunkte zwischen Linien
    if (m_intersectionSnapEnabled && entities.size() >= 2) {
        // Hilfs-Lambda für Linien-Segment-Schnittpunkt
        auto getIntersection = [](const QPointF& p1, const QPointF& p2,
                                  const QPointF& p3, const QPointF& p4,
                                  QPointF& outPt) -> bool
        {
            double denominator = (p4.y() - p3.y()) * (p2.x() - p1.x()) -
                                 (p4.x() - p3.x()) * (p2.y() - p1.y());

            if (std::abs(denominator) < 1e-9) return false; // Parallel

            double ua = ((p4.x() - p3.x()) * (p1.y() - p3.y()) -
                         (p4.y() - p3.y()) * (p1.x() - p3.x())) / denominator;
            double ub = ((p2.x() - p1.x()) * (p1.y() - p3.y()) -
                         (p2.y() - p1.y()) * (p1.x() - p3.x())) / denominator;

            // Liegt der Schnittpunkt auf BEIDEN Liniensegmenten?
            if (ua >= 0.0 && ua <= 1.0 && ub >= 0.0 && ub <= 1.0) {
                outPt = QPointF(p1.x() + ua * (p2.x() - p1.x()),
                                p1.y() + ua * (p2.y() - p1.y()));
                return true;
            }
            return false;
        };

        // Paarschleife über alle Entities
        for (size_t i = 0; i < entities.size(); ++i) {
            if (!entities[i] || entities[i]->type() != EntityType::Line) continue;
            auto* line1 = static_cast<const CadLine*>(entities[i].get());

            for (size_t j = i + 1; j < entities.size(); ++j) {
                if (!entities[j] || entities[j]->type() != EntityType::Line) continue;
                auto* line2 = static_cast<const CadLine*>(entities[j].get());

                QPointF intersectPt;
                if (getIntersection(line1->start(), line1->end(),
                                    line2->start(), line2->end(),
                                    intersectPt))
                {
                    checkPoint(intersectPt, SnapType::Intersection);
                }
            }
        }
    }

    return bestSnap;
}

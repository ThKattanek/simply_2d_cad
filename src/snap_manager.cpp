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
#include "./cad_document/cad_circle.h"

SnapResult SnapManager::findSnapPoint(const QPointF& mouseWorldPos, const CadDocument& doc, double zoomFactor)
{
    SnapResult bestSnap;

    // Bildschirm-Toleranz (Pixel) in Weltkoordinaten umrechnen
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

    // 1. DURCHGANG: Endpunkte, Mittelpunkte & Einzelpunkte
    if (m_endpointSnapEnabled || m_midpointSnapEnabled || m_pointSnapEnabled) {
        for (const auto& entityPtr : entities) {
            if (!entityPtr) continue;

            switch (entityPtr->type()) {
            case EntityType::Point: {
                auto* pt = static_cast<const CadPoint*>(entityPtr.get());
                if (m_pointSnapEnabled)
                    checkPoint(pt->position(), SnapType::Point);
                break;
            }
            case EntityType::Line: {
                auto* line = static_cast<const CadLine*>(entityPtr.get());
                if (m_endpointSnapEnabled) {
                    checkPoint(line->start(), SnapType::Endpoint);
                    checkPoint(line->end(), SnapType::Endpoint);
                }
                if (m_midpointSnapEnabled) {
                    QPointF midPoint = (line->start() + line->end()) * 0.5;
                    checkPoint(midPoint, SnapType::Midpoint);
                }
                break;
            }
            case EntityType::Circle: { // <-- KREIS-MITTELPUNKT
                auto* circle = static_cast<const CadCircle*>(entityPtr.get());
                if (m_midpointSnapEnabled) {
                    checkPoint(circle->center(), SnapType::Point); // Kreis-Mittelpunkt als Punkt-Snap
                }
                break;
            }
            default:
                break;
            }
        }
    }

    // 2. DURCHGANG: Schnittpunkte (Linie-Linie, Linie-Kreis, Kreis-Kreis)
    if (m_intersectionSnapEnabled && entities.size() >= 2) {

        // --- Helper A: Linie / Linie ---
        auto getLineLineIntersection = [](const QPointF& p1, const QPointF& p2,
                                          const QPointF& p3, const QPointF& p4,
                                          QPointF& outPt) -> bool
        {
            double denominator = (p4.y() - p3.y()) * (p2.x() - p1.x()) -
                                 (p4.x() - p3.x()) * (p2.y() - p1.y());

            if (std::abs(denominator) < 1e-9) return false;

            double ua = ((p4.x() - p3.x()) * (p1.y() - p3.y()) -
                         (p4.y() - p3.y()) * (p1.x() - p3.x())) / denominator;
            double ub = ((p2.x() - p1.x()) * (p1.y() - p3.y()) -
                         (p2.y() - p1.y()) * (p1.x() - p3.x())) / denominator;

            if (ua >= 0.0 && ua <= 1.0 && ub >= 0.0 && ub <= 1.0) {
                outPt = QPointF(p1.x() + ua * (p2.x() - p1.x()),
                                p1.y() + ua * (p2.y() - p1.y()));
                return true;
            }
            return false;
        };

        // --- Helper B: Linie / Kreis ---
        auto getLineCircleIntersections = [](const QPointF& p1, const QPointF& p2,
                                             const QPointF& center, double radius,
                                             std::vector<QPointF>& outPts)
        {
            QPointF d = p2 - p1;
            QPointF f = p1 - center;

            double a = d.x() * d.x() + d.y() * d.y();
            double b = 2 * (f.x() * d.x() + f.y() * d.y());
            double c = (f.x() * f.x() + f.y() * f.y()) - radius * radius;

            double discriminant = b * b - 4 * a * c;
            if (discriminant < 0 || std::abs(a) < 1e-9) return; // Keine echten Schnittpunkte

            discriminant = std::sqrt(discriminant);
            double t1 = (-b - discriminant) / (2 * a);
            double t2 = (-b + discriminant) / (2 * a);

            if (t1 >= 0.0 && t1 <= 1.0) outPts.push_back(p1 + t1 * d);
            if (t2 >= 0.0 && t2 <= 1.0) outPts.push_back(p1 + t2 * d);
        };

        // --- Helper C: Kreis / Kreis ---
        auto getCircleCircleIntersections = [](const QPointF& c1, double r1,
                                               const QPointF& c2, double r2,
                                               std::vector<QPointF>& outPts)
        {
            double d = std::hypot(c2.x() - c1.x(), c2.y() - c1.y());

            // Zu weit auseinander, ineinander oder identisch
            if (d > (r1 + r2) || d < std::abs(r1 - r2) || d < 1e-9) return;

            double a = (r1 * r1 - r2 * r2 + d * d) / (2 * d);
            double h = std::sqrt(std::max(0.0, r1 * r1 - a * a));

            QPointF p2 = c1 + a * (c2 - c1) / d;

            // Erster Schnittpunkt
            outPts.push_back(QPointF(p2.x() + h * (c2.y() - c1.y()) / d,
                                     p2.y() - h * (c2.x() - c1.x()) / d));

            // Zweiter Schnittpunkt (falls spürbarer Abstand)
            if (h > 1e-6) {
                outPts.push_back(QPointF(p2.x() - h * (c2.y() - c1.y()) / d,
                                         p2.y() + h * (c2.x() - c1.x()) / d));
            }
        };

        // Paar-Iterationen über alle Entity-Kombinationen
        for (size_t i = 0; i < entities.size(); ++i) {
            if (!entities[i]) continue;

            for (size_t j = i + 1; j < entities.size(); ++j) {
                if (!entities[j]) continue;

                auto type1 = entities[i]->type();
                auto type2 = entities[j]->type();

                // 1. Linie - Linie
                if (type1 == EntityType::Line && type2 == EntityType::Line) {
                    auto* line1 = static_cast<const CadLine*>(entities[i].get());
                    auto* line2 = static_cast<const CadLine*>(entities[j].get());

                    QPointF intersectPt;
                    if (getLineLineIntersection(line1->start(), line1->end(),
                                                line2->start(), line2->end(),
                                                intersectPt)) {
                        checkPoint(intersectPt, SnapType::Intersection);
                    }
                }
                // 2. Linie - Kreis (oder Kreis - Linie)
                else if ((type1 == EntityType::Line && type2 == EntityType::Circle) ||
                         (type1 == EntityType::Circle && type2 == EntityType::Line)) {

                    const CadLine* line = static_cast<const CadLine*>((type1 == EntityType::Line) ? entities[i].get() : entities[j].get());
                    const CadCircle* circle = static_cast<const CadCircle*>((type1 == EntityType::Circle) ? entities[i].get() : entities[j].get());

                    std::vector<QPointF> intersects;
                    getLineCircleIntersections(line->start(), line->end(), circle->center(), circle->radius(), intersects);

                    for (const auto& pt : intersects) {
                        checkPoint(pt, SnapType::Intersection);
                    }
                }
                // 3. Kreis - Kreis
                else if (type1 == EntityType::Circle && type2 == EntityType::Circle) {
                    auto* circle1 = static_cast<const CadCircle*>(entities[i].get());
                    auto* circle2 = static_cast<const CadCircle*>(entities[j].get());

                    std::vector<QPointF> intersects;
                    getCircleCircleIntersections(circle1->center(), circle1->radius(),
                                                 circle2->center(), circle2->radius(), intersects);

                    for (const auto& pt : intersects) {
                        checkPoint(pt, SnapType::Intersection);
                    }
                }
            }
        }
    }

    return bestSnap;
}

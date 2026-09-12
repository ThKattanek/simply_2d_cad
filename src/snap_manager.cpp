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
#include "./cad_scene.h"
#include "./cad_document/cad_line.h"
#include "./cad_document/cad_point.h"
#include "./cad_document/cad_circle.h"

#include <QGraphicsItem>
#include <vector>
#include <cmath>

SnapResult SnapManager::findSnapPoint(const QPointF& mouseWorldPos, CadScene &scene, double zoomFactor)
{
    SnapResult bestSnap;

    // 1. Bildschirm-Toleranz in Welt-Koordinaten umrechnen
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

    // 2. SPATIAL QUERY: Erstelle ein Such-Rechteck um den Mauszeiger
    QRectF searchRect(mouseWorldPos.x() - maxDistWorld,
                      mouseWorldPos.y() - maxDistWorld,
                      maxDistWorld * 2.0,
                      maxDistWorld * 2.0);

    // Qt findet blitzschnell über den BSP-Tree NUR die Items im Fang-Bereich!
    QList<QGraphicsItem*> nearbyGraphicsItems = scene.items(searchRect);

    // Filtere relevante CadEntities heraus (System-Items wie Fadenkreuz/Grid ignorieren)
    std::vector<const CadEntity*> nearbyEntities;
    nearbyEntities.reserve(nearbyGraphicsItems.size());

    std::vector<std::unique_ptr<CadLine>> systemLinesStorage;

    for (QGraphicsItem* item : nearbyGraphicsItems) {
        // SystemItems überspringen
        if (item->data(Qt::UserRole + 1).toString() == "SystemItem") {
            // Prüfen, ob das Item eine QGraphicsLineItem ist (z. B. deine Mittellinien)
            if (auto* lineItem = dynamic_cast<QGraphicsLineItem*>(item)) {
                // Nur horizontale/vertikale Achsen zulassen (Fadenkreuz ausschließen!)
                if (lineItem != scene.getCrosshairItem()->parentItem()) {
                    QLineF line = lineItem->line();

                    // Temporäre CadLine für die Snap-Berechnung erzeugen
                    auto tempCadLine = std::make_unique<CadLine>(line.p1(), line.p2());
                    nearbyEntities.push_back(tempCadLine.get());
                    systemLinesStorage.push_back(std::move(tempCadLine));
                }
            }
            continue; // Andere SystemItems (Fadenkreuz, Marker) weiterhin ignorieren
        }

        // CAD-Entity aus dem Custom-Data-Pointer oder Dynamic Cast ermitteln
        // (Falls deine CadEntity das QGraphicsItem besitzt oder speichert):
        auto entityPtr = item->data(Qt::UserRole).value<CadEntity*>();
        if (entityPtr) {
            nearbyEntities.push_back(entityPtr);
        }
    }

    // 3. DURCHGANG: Endpunkte, Mittelpunkte & Punkte im Umkreis
    if (m_endpointSnapEnabled || m_midpointSnapEnabled || m_pointSnapEnabled) {
        for (const CadEntity* entity : nearbyEntities) {
            switch (entity->type()) {
            case EntityType::Point: {
                auto* pt = static_cast<const CadPoint*>(entity);
                if (m_pointSnapEnabled)
                    checkPoint(pt->position(), SnapType::Point);
                break;
            }
            case EntityType::Line: {
                auto* line = static_cast<const CadLine*>(entity);
                if (m_endpointSnapEnabled) {
                    checkPoint(line->start(), SnapType::Endpoint);
                    checkPoint(line->end(), SnapType::Endpoint);
                }

                if (m_midpointSnapEnabled) {
                    QPointF midPoint = (line->start() + line->end()) * 0.5;
                    checkPoint(midPoint, SnapType::Midpoint);
                }

                if (m_tangentSnapEnabled) {
                    QPointF lastPt = scene.getLastPoint();
                    QPointF A = line->start();
                    QPointF B = line->end();

                    double dx = B.x() - A.x();
                    double dy = B.y() - A.y();
                    double lengthSq = dx * dx + dy * dy;

                    if (lengthSq > 1e-9) { // Division durch Null verhindern
                        // Skalarprodukt berechnen, um den Parameter t auf der Linie zu finden
                        double t = ((lastPt.x() - A.x()) * dx + (lastPt.y() - A.y()) * dy) / lengthSq;

                        // Fange nur dann, wenn der Lotpunkt tatsächlich auf dem gezeichneten Liniensegment liegt
                        if (t >= 0.0 && t <= 1.0) {
                            QPointF tangentPt(A.x() + t * dx, A.y() + t * dy);
                            checkPoint(tangentPt, SnapType::Tangent);
                        }
                    }
                }
                break;
            }
            case EntityType::Circle: {
                auto* circle = static_cast<const CadCircle*>(entity);
                if (m_midpointSnapEnabled) {
                    checkPoint(circle->center(), SnapType::Midpoint);
                }

                if (m_tangentSnapEnabled) {
                    QPointF lastPt = scene.getLastPoint();
                    QPointF c = circle->center();
                    double r = circle->radius();

                    double dx = lastPt.x() - c.x();
                    double dy = lastPt.y() - c.y();
                    double dist2 = dx * dx + dy * dy;

                    // Nur berechnen, wenn der letzte Punkt außerhalb des Kreises liegt
                    if (dist2 >= r * r && dist2 > 1e-6) {
                        double L = std::sqrt(dist2 - r * r);

                        // Erster Tangentenpunkt T1
                        QPointF t1(
                            c.x() + (r / dist2) * (r * dx - L * dy),
                            c.y() + (r / dist2) * (r * dy + L * dx)
                            );
                        checkPoint(t1, SnapType::Tangent);

                        // Zweiter Tangentenpunkt T2
                        QPointF t2(
                            c.x() + (r / dist2) * (r * dx + L * dy),
                            c.y() + (r / dist2) * (r * dy - L * dx)
                            );
                        checkPoint(t2, SnapType::Tangent);
                    }
                }
                break;
            }
            default:
                break;
            }
        }
    }

    // 4. DURCHGANG: Schnittpunkte (NUR zwischen den nahegelegenen Objekten!)
    if (m_intersectionSnapEnabled && nearbyEntities.size() >= 2) {

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
            if (discriminant < 0 || std::abs(a) < 1e-9) return;

            discriminant = std::sqrt(discriminant);
            double t1 = (-b - discriminant) / (2 * a);
            double t2 = (-b + discriminant) / (2 * a);

            if (t1 >= 0.0 && t1 <= 1.0) outPts.push_back(p1 + t1 * d);
            if (t2 >= 0.0 && t2 <= 1.0) outPts.push_back(p1 + t2 * d);
        };

        auto getCircleCircleIntersections = [](const QPointF& c1, double r1,
                                               const QPointF& c2, double r2,
                                               std::vector<QPointF>& outPts)
        {
            double d = std::hypot(c2.x() - c1.x(), c2.y() - c1.y());
            if (d > (r1 + r2) || d < std::abs(r1 - r2) || d < 1e-9) return;

            double a = (r1 * r1 - r2 * r2 + d * d) / (2 * d);
            double h = std::sqrt(std::max(0.0, r1 * r1 - a * a));

            QPointF p2 = c1 + a * (c2 - c1) / d;

            outPts.push_back(QPointF(p2.x() + h * (c2.y() - c1.y()) / d,
                                     p2.y() - h * (c2.x() - c1.x()) / d));

            if (h > 1e-6) {
                outPts.push_back(QPointF(p2.x() - h * (c2.y() - c1.y()) / d,
                                         p2.y() + h * (c2.x() - c1.x()) / d));
            }
        };

        for (size_t i = 0; i < nearbyEntities.size(); ++i) {
            for (size_t j = i + 1; j < nearbyEntities.size(); ++j) {
                auto type1 = nearbyEntities[i]->type();
                auto type2 = nearbyEntities[j]->type();

                if (type1 == EntityType::Line && type2 == EntityType::Line) {
                    auto* line1 = static_cast<const CadLine*>(nearbyEntities[i]);
                    auto* line2 = static_cast<const CadLine*>(nearbyEntities[j]);
                    QPointF intersectPt;
                    if (getLineLineIntersection(line1->start(), line1->end(), line2->start(), line2->end(), intersectPt)) {
                        checkPoint(intersectPt, SnapType::Intersection);
                    }
                }
                else if ((type1 == EntityType::Line && type2 == EntityType::Circle) ||
                         (type1 == EntityType::Circle && type2 == EntityType::Line)) {
                    const CadLine* line = static_cast<const CadLine*>((type1 == EntityType::Line) ? nearbyEntities[i] : nearbyEntities[j]);
                    const CadCircle* circle = static_cast<const CadCircle*>((type1 == EntityType::Circle) ? nearbyEntities[i] : nearbyEntities[j]);
                    std::vector<QPointF> intersects;
                    getLineCircleIntersections(line->start(), line->end(), circle->center(), circle->radius(), intersects);
                    for (const auto& pt : intersects) {
                        checkPoint(pt, SnapType::Intersection);
                    }
                }
                else if (type1 == EntityType::Circle && type2 == EntityType::Circle) {
                    auto* circle1 = static_cast<const CadCircle*>(nearbyEntities[i]);
                    auto* circle2 = static_cast<const CadCircle*>(nearbyEntities[j]);
                    std::vector<QPointF> intersects;
                    getCircleCircleIntersections(circle1->center(), circle1->radius(), circle2->center(), circle2->radius(), intersects);
                    for (const auto& pt : intersects) {
                        checkPoint(pt, SnapType::Intersection);
                    }
                }
            }
        }
    }

    return bestSnap;
}

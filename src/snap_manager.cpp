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
#include "./cad_document/cad_construction_hv_line.h"
#include "./cad_document/cad_construction_line.h"

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
            if  (!m_constructionLineSnapEnabled &&
                (entityPtr->type() == EntityType::ConstructionHvLine ||
                 entityPtr->type() == EntityType::ConstructionLine)) {
                continue;
            }
            nearbyEntities.push_back(entityPtr);
        }
    }

    // 3. DURCHGANG: Endpunkte, Mittelpunkte, Tangenten & Lotpunkte
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

                    if (lengthSq > 1e-9) {
                        double t = ((lastPt.x() - A.x()) * dx + (lastPt.y() - A.y()) * dy) / lengthSq;
                        if (t >= 0.0 && t <= 1.0) {
                            QPointF tangentPt(A.x() + t * dx, A.y() + t * dy);
                            checkPoint(tangentPt, SnapType::Tangent);
                        }
                    }
                }
                break;
            }
            case EntityType::ConstructionHvLine: {
                // Bei Hilfslinien: Lotfußpunkt/Perpendikular von der vorherigen Position fangen
                if (m_tangentSnapEnabled) {
                    auto* cline = static_cast<const CadConstructionHvLine*>(entity);
                    QPointF lastPt = scene.getLastPoint();
                    if (cline->getOrientation() == ConstructionLineOrientation::Horizontal) {
                        checkPoint(QPointF(lastPt.x(), cline->getPosition().y()), SnapType::Tangent);
                    } else {
                        checkPoint(QPointF(cline->getPosition().x(), lastPt.y()), SnapType::Tangent);
                    }
                }
                break;
            }
            case EntityType::ConstructionLine: {
                if (m_tangentSnapEnabled) {
                    auto* cline = static_cast<const CadConstructionLine*>(entity);
                    QPointF lastPt = scene.getLastPoint();
                    QPointF A = cline->p1();
                    QPointF B = cline->p2();

                    double dx = B.x() - A.x();
                    double dy = B.y() - A.y();
                    double lengthSq = dx * dx + dy * dy;

                    if (lengthSq > 1e-9) {
                        double t = ((lastPt.x() - A.x()) * dx + (lastPt.y() - A.y()) * dy) / lengthSq;
                        QPointF perpPt(A.x() + t * dx, A.y() + t * dy);
                        checkPoint(perpPt, SnapType::Tangent);
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

                    if (dist2 >= r * r && dist2 > 1e-6) {
                        double L = std::sqrt(dist2 - r * r);
                        QPointF t1(
                            c.x() + (r / dist2) * (r * dx - L * dy),
                            c.y() + (r / dist2) * (r * dy + L * dx)
                            );
                        checkPoint(t1, SnapType::Tangent);

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

    // 4. DURCHGANG: Schnittpunkte (inklusive Hilfslinien!)
    if (m_intersectionSnapEnabled && nearbyEntities.size() >= 2) {

        // Helper: Holt Start- und Endpunkt/Richtung aus Line, ConstructionHvLine oder ConstructionLine
        auto getGenericLineParams = [](const CadEntity* entity, QPointF& p1, QPointF& p2, bool& isInfinite) -> bool {
            if (entity->type() == EntityType::Line) {
                auto* line = static_cast<const CadLine*>(entity);
                p1 = line->start();
                p2 = line->end();
                isInfinite = false;
                return true;
            } else if (entity->type() == EntityType::ConstructionHvLine) {
                auto* cline = static_cast<const CadConstructionHvLine*>(entity);
                p1 = cline->getPosition();
                if (cline->getOrientation() == ConstructionLineOrientation::Horizontal) {
                    p2 = QPointF(p1.x() + 1.0, p1.y());
                } else {
                    p2 = QPointF(p1.x(), p1.y() + 1.0);
                }
                isInfinite = true;
                return true;
            } else if (entity->type() == EntityType::ConstructionLine) {
                auto* cline = static_cast<const CadConstructionLine*>(entity);
                p1 = cline->p1();
                p2 = cline->p2();
                isInfinite = true;
                return true;
            }
            return false;
        };

        auto getLineLineIntersection = [&](const CadEntity* e1, const CadEntity* e2, QPointF& outPt) -> bool {
            QPointF p1, p2, p3, p4;
            bool inf1 = false, inf2 = false;

            if (!getGenericLineParams(e1, p1, p2, inf1) || !getGenericLineParams(e2, p3, p4, inf2))
                return false;

            double denominator = (p4.y() - p3.y()) * (p2.x() - p1.x()) -
                                 (p4.x() - p3.x()) * (p2.y() - p1.y());

            if (std::abs(denominator) < 1e-9) return false;

            double ua = ((p4.x() - p3.x()) * (p1.y() - p3.y()) -
                         (p4.y() - p3.y()) * (p1.x() - p3.x())) / denominator;
            double ub = ((p2.x() - p1.x()) * (p1.y() - p3.y()) -
                         (p2.y() - p1.y()) * (p1.x() - p3.x())) / denominator;

            bool validA = inf1 || (ua >= 0.0 && ua <= 1.0);
            bool validB = inf2 || (ub >= 0.0 && ub <= 1.0);

            if (validA && validB) {
                outPt = QPointF(p1.x() + ua * (p2.x() - p1.x()),
                                p1.y() + ua * (p2.y() - p1.y()));
                return true;
            }
            return false;
        };

        auto getLineCircleIntersections = [&](const CadEntity* lineEntity, const CadCircle* circle, std::vector<QPointF>& outPts) {
            QPointF p1, p2;
            bool isInfinite = false;
            if (!getGenericLineParams(lineEntity, p1, p2, isInfinite)) return;

            QPointF d = p2 - p1;
            QPointF f = p1 - circle->center();

            double a = d.x() * d.x() + d.y() * d.y();
            double b = 2 * (f.x() * d.x() + f.y() * d.y());
            double c = (f.x() * f.x() + f.y() * f.y()) - circle->radius() * circle->radius();

            double discriminant = b * b - 4 * a * c;
            if (discriminant < 0 || std::abs(a) < 1e-9) return;

            discriminant = std::sqrt(discriminant);
            double t1 = (-b - discriminant) / (2 * a);
            double t2 = (-b + discriminant) / (2 * a);

            if (isInfinite || (t1 >= 0.0 && t1 <= 1.0)) outPts.push_back(p1 + t1 * d);
            if (isInfinite || (t2 >= 0.0 && t2 <= 1.0)) outPts.push_back(p1 + t2 * d);
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

                bool isLineLike1 = (type1 == EntityType::Line ||
                                    type1 == EntityType::ConstructionHvLine ||
                                    type1 == EntityType::ConstructionLine);
                bool isLineLike2 = (type2 == EntityType::Line ||
                                    type2 == EntityType::ConstructionHvLine ||
                                    type2 == EntityType::ConstructionLine);

                if (isLineLike1 && isLineLike2) {
                    QPointF intersectPt;
                    if (getLineLineIntersection(nearbyEntities[i], nearbyEntities[j], intersectPt)) {
                        checkPoint(intersectPt, SnapType::Intersection);
                    }
                }
                else if (isLineLike1 && type2 == EntityType::Circle) {
                    auto* circle = static_cast<const CadCircle*>(nearbyEntities[j]);
                    std::vector<QPointF> intersects;
                    getLineCircleIntersections(nearbyEntities[i], circle, intersects);
                    for (const auto& pt : intersects) {
                        checkPoint(pt, SnapType::Intersection);
                    }
                }
                else if (type1 == EntityType::Circle && isLineLike2) {
                    auto* circle = static_cast<const CadCircle*>(nearbyEntities[i]);
                    std::vector<QPointF> intersects;
                    getLineCircleIntersections(nearbyEntities[j], circle, intersects);
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

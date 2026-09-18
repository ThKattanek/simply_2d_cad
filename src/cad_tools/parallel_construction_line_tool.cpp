/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "parallel_construction_line_tool.h"
#include "../cad_document/cad_construction_line.h"
#include "../cad_document/cad_construction_hv_line.h"
#include "../cad_document/cad_line.h"
#include "../commands/add_entity_command.h"
#include "../commands/macro_command.h"
#include "../undo_stack.h"

#include <QGraphicsSceneMouseEvent>
#include <QRegularExpression>
#include <QGraphicsItem>
#include <QKeyEvent>
#include <QPen>
#include <cmath>
#include <qgraphicsview.h>

void ParallelConstructionLineTool::retranslate()
{
    m_promptMsg01 = tr("Parallel Construction Line: Select a line or construction line.");
    m_promptMsg02 = tr("Parallel Construction Line: Enter distance (Return) or move mouse to set side/distance. Click to apply.");
}

void ParallelConstructionLineTool::activate(CadScene* scene)
{
    Q_UNUSED(scene);

    // WICHTIG: Erst beim Neuaufruf des Tools wird der Speicher geleert!
    m_distances.clear();

    cancelDrawing(scene);
}

void ParallelConstructionLineTool::deactivate(CadScene* scene)
{
    cancelDrawing(scene);
}

void ParallelConstructionLineTool::cancel(CadScene* scene)
{
    // Bei ESC oder Abbrechen: Gesamtes Werkzeug zurücksetzen & Abstände leeren
    m_distances.clear();
    cancelDrawing(scene);
}

void ParallelConstructionLineTool::mousePressEvent(CadScene* scene, QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QPointF mousePos = scene->getSnapOrPosition(event->scenePos());

        if (m_state == State::SelectBaseEntity) {
            if (m_hoveredEntity) {
                m_selectedEntity = m_hoveredEntity;

                if (getEntityLineParams(m_selectedEntity, m_basePoint, m_dirVector, m_normalVector)) {
                    m_state = State::PositionLines;
                    clearHover();
                    emit promptTextChanged(m_promptMsg02);

                    // Direkt die Vorschau mit den evtl. bereits vorhandenen Abständen zeichnen
                    updatePreview(scene, mousePos);
                }
            }
        }
        else if (m_state == State::PositionLines) {
            // Klick platziert alle aktuellen Linien final
            if (!m_previewItems.empty()) {
                auto macroCmd = std::make_unique<MacroCommand>(tr("Add Parallel Construction Lines"));

                QPointF mouseVector = mousePos - m_basePoint;
                double side = ((mouseVector.x() * m_normalVector.x() + mouseVector.y() * m_normalVector.y()) >= 0) ? 1.0 : -1.0;

                std::vector<double> finalDistances = m_distances;
                if (finalDistances.empty()) {
                    // Fallback: Dynamischer Mausabstand, falls nie ein Wert eingegeben wurde
                    double dist = std::abs(mouseVector.x() * m_normalVector.x() + mouseVector.y() * m_normalVector.y());
                    finalDistances.push_back(dist);
                }

                for (double dist : finalDistances) {
                    QPointF offsetPt = m_basePoint + m_normalVector * (dist * side);
                    QPointF p2 = offsetPt + m_dirVector;

                    auto cline = std::make_unique<CadConstructionLine>(offsetPt, p2);
                    auto addCmd = std::make_unique<AddEntityCommand>(
                        scene->getDocument(), std::move(cline), tr("Add Parallel Construction Line"));
                    macroCmd->addCommand(std::move(addCmd));
                }

                if (scene->getUndoStack()) {
                    scene->getUndoStack()->push(std::move(macroCmd));
                } else {
                    macroCmd->execute();
                }
            }

            // NACH DEM KLICK: Zurück auf Elementauswahl, ABER m_distances BLEIBT ERHALTEN!
            resetForNextEntity(scene);
        }
    }
    else if (event->button() == Qt::RightButton) {
        // Rechtsklick bricht die aktuelle Runde/das Tool ab
        cancel(scene);
    }
}

void ParallelConstructionLineTool::mouseMoveEvent(CadScene* scene, QGraphicsSceneMouseEvent* event)
{
    m_currentMousePos = event->scenePos();

    if (m_state == State::SelectBaseEntity) {
        updateHoverEntity(scene, m_currentMousePos);
    } else if (m_state == State::PositionLines) {
        // Snapping der Szene nutzen, damit die freie Vorschau an Kreuzungen einrastet!
        QPointF snappedMousePos = scene->getSnapOrPosition(m_currentMousePos);
        updatePreview(scene, snappedMousePos);
    }
}

void ParallelConstructionLineTool::keyPressEvent(CadScene* scene, QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        cancel(scene);
    }
}

void ParallelConstructionLineTool::handlePointInput(CadScene* scene, const QPointF& point)
{
    if (m_state == State::PositionLines) {
        updatePreview(scene, point);
    }
}

void ParallelConstructionLineTool::handleValueInput(CadScene* scene, double value)
{
    if (m_state == State::PositionLines) {
        if (m_distances.size() < 255) {
            m_distances.push_back(std::abs(value));
        }
        updatePreview(scene, m_currentMousePos);
    }
}

void ParallelConstructionLineTool::resetForNextEntity(CadScene* scene)
{
    // Setzt den Status für das nächste Element zurück, behält aber m_distances!
    m_state = State::SelectBaseEntity;
    m_selectedEntity = nullptr;

    clearHover();

    for (auto* item : m_previewItems) {
        scene->removeItem(item);
        delete item;
    }
    m_previewItems.clear();

    emit promptTextChanged(m_promptMsg01);
}

void ParallelConstructionLineTool::cancelDrawing(CadScene* scene)
{
    resetForNextEntity(scene);
}

void ParallelConstructionLineTool::updateHoverEntity(CadScene* scene, const QPointF& mousePos)
{
    // 1. Zoom-Faktor aus der QGraphicsView der Szene ermitteln
    double zoomFactor = 1.0;
    if (!scene->views().isEmpty() && scene->views().first()) {
        zoomFactor = scene->views().first()->transform().m11(); // Skalierung der X-Achse
    }

    // 2. Toleranz maßstabsunabhängig in Weltkoordinaten umrechnen (immer exakt 10 Pixel auf dem Schirm)
    const double toleranceWorld = 10.0 / std::abs(zoomFactor);

    QRectF searchRect(mousePos.x() - toleranceWorld,
                      mousePos.y() - toleranceWorld,
                      toleranceWorld * 2.0,
                      toleranceWorld * 2.0);

    QList<QGraphicsItem*> items = scene->items(searchRect);
    CadEntity* newHover = nullptr;

    for (QGraphicsItem* item : items) {
        // System-Items (Fadenkreuz, Snap-Marker) überspringen
        if (item->data(Qt::UserRole + 1).toString() == "SystemItem") {
            continue;
        }

        auto entity = item->data(Qt::UserRole).value<CadEntity*>();
        if (entity) {
            auto type = entity->type();
            if (type == EntityType::Line ||
                type == EntityType::ConstructionHvLine ||
                type == EntityType::ConstructionLine)
            {
                newHover = entity;
                break;
            }
        }
    }

    // 3. Wenn sich das gehoverte Element geändert hat, Farbe aktualisieren
    if (m_hoveredEntity != newHover) {
        clearHover();
        m_hoveredEntity = newHover;

        if (m_hoveredEntity && m_hoveredEntity->getGraphicsItem()) {
            if (auto lineItem = dynamic_cast<QGraphicsLineItem*>(m_hoveredEntity->getGraphicsItem())) {
                QPen pen = lineItem->pen();
                pen.setColor(Qt::magenta); // Lila-Hervorhebung
                pen.setWidth(0);
                lineItem->setPen(pen);
            }
        }
    }
}

void ParallelConstructionLineTool::clearHover()
{
    if (m_hoveredEntity && m_hoveredEntity->getGraphicsItem()) {
        if (auto lineItem = dynamic_cast<QGraphicsLineItem*>(m_hoveredEntity->getGraphicsItem())) {
            if (m_hoveredEntity->type() == EntityType::Line) {
                lineItem->setPen(QPen(Qt::white, 0));
            } else {
                lineItem->setPen(QPen(Qt::red, 0, Qt::DashLine));
            }
        }
    }
    m_hoveredEntity = nullptr;
}

bool ParallelConstructionLineTool::getEntityLineParams(const CadEntity* entity, QPointF& basePoint, QPointF& dirVector, QPointF& normalVector) const
{
    if (!entity) return false;

    QPointF p1, p2;
    if (entity->type() == EntityType::Line) {
        auto* line = static_cast<const CadLine*>(entity);
        p1 = line->start();
        p2 = line->end();
    } else if (entity->type() == EntityType::ConstructionLine) {
        auto* cline = static_cast<const CadConstructionLine*>(entity);
        p1 = cline->p1();
        p2 = cline->p2();
    } else if (entity->type() == EntityType::ConstructionHvLine) {
        auto* cline = static_cast<const CadConstructionHvLine*>(entity);
        p1 = cline->getPosition();
        p2 = (cline->getOrientation() == ConstructionLineOrientation::Horizontal)
                 ? QPointF(p1.x() + 1.0, p1.y())
                 : QPointF(p1.x(), p1.y() + 1.0);
    } else {
        return false;
    }

    basePoint = p1;
    QPointF diff = p2 - p1;
    double len = std::hypot(diff.x(), diff.y());
    if (len < 1e-9) return false;

    dirVector = diff / len;
    normalVector = QPointF(-dirVector.y(), dirVector.x());
    return true;
}

void ParallelConstructionLineTool::updatePreview(CadScene* scene, const QPointF& mousePos)
{
    for (auto* item : m_previewItems) {
        scene->removeItem(item);
        delete item;
    }
    m_previewItems.clear();

    if (!m_selectedEntity) return;

    QPointF mouseVector = mousePos - m_basePoint;
    double projection = mouseVector.x() * m_normalVector.x() + mouseVector.y() * m_normalVector.y();
    double side = (projection >= 0) ? 1.0 : -1.0;

    std::vector<double> currentDistances = m_distances;
    if (currentDistances.empty()) {
        currentDistances.push_back(std::abs(projection));
    }

    constexpr double maxDist = 1000000.0;

    QPen previewPen(Qt::gray, 0, Qt::SolidLine);

    for (double dist : currentDistances) {
        QPointF offsetPt = m_basePoint + m_normalVector * (dist * side);
        QLineF previewLine(offsetPt - m_dirVector * maxDist, offsetPt + m_dirVector * maxDist);

        auto* item = scene->addLine(previewLine, previewPen);
        m_previewItems.push_back(item);
    }
}
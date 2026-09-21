/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "perpendicular_construction_line_tool.h"
#include "../cad_document/cad_construction_line.h"
#include "../cad_document/cad_construction_hv_line.h"
#include "../cad_document/cad_line.h"
#include "../commands/add_entity_command.h"
#include "../undo_stack.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QKeyEvent>
#include <QPen>
#include <cmath>
#include <QGraphicsView>

void PerpendicularConstructionLineTool::retranslate()
{
    m_promptMsg01 = tr("Perpendicular Construction Line: Select a straight element.");
    m_promptMsg02 = tr("Perpendicular Construction Line: Click a point or enter coordinates for the perpendicular line.");
}

void PerpendicularConstructionLineTool::activate(CadScene* scene)
{
    Q_UNUSED(scene);
    cancelDrawing(scene);
}

void PerpendicularConstructionLineTool::deactivate(CadScene* scene)
{
    cancelDrawing(scene);
}

void PerpendicularConstructionLineTool::cancel(CadScene* scene)
{
    cancelDrawing(scene);
}

void PerpendicularConstructionLineTool::mousePressEvent(CadScene* scene, QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QPointF mousePos = scene->getSnapOrPosition(event->scenePos());

        if (m_state == State::SelectBaseEntity) {
            if (m_hoveredEntity || m_hoveredSystemLine) {
                if (m_hoveredEntity) {
                    m_selectedEntity = m_hoveredEntity;
                    m_selectedSystemLine = nullptr;
                } else {
                    m_selectedEntity = nullptr;
                    m_selectedSystemLine = m_hoveredSystemLine;
                }

                if (getEntityLineParams(m_selectedEntity, m_basePoint, m_dirVector, m_normalVector)) {
                    m_state = State::PositionLine;
                    clearHover();
                    emit promptTextChanged(m_promptMsg02);
                    updatePreview(scene, mousePos);
                }
            }
        }
        else if (m_state == State::PositionLine) {
            // Eine rechtwinklige Hilfslinie am Durchgangspunkt (mousePos) erzeugen
            auto cline = std::make_unique<CadConstructionLine>(mousePos, mousePos + m_normalVector);
            auto addCmd = std::make_unique<AddEntityCommand>(
                scene->getDocument(), std::move(cline), tr("Add Perpendicular Construction Line"));

            if (scene->getUndoStack()) {
                scene->getUndoStack()->push(std::move(addCmd));
            } else {
                addCmd->execute();
            }

            resetForNextEntity(scene);
        }
    }
    else if (event->button() == Qt::RightButton) {
        cancel(scene);
    }
}

void PerpendicularConstructionLineTool::mouseMoveEvent(CadScene* scene, QGraphicsSceneMouseEvent* event)
{
    m_currentMousePos = event->scenePos();

    if (m_state == State::SelectBaseEntity) {
        updateHoverEntity(scene, m_currentMousePos);
    } else if (m_state == State::PositionLine) {
        QPointF snappedMousePos = scene->getSnapOrPosition(m_currentMousePos);
        updatePreview(scene, snappedMousePos);
    }
}

void PerpendicularConstructionLineTool::keyPressEvent(CadScene* scene, QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        cancel(scene);
    }
}

void PerpendicularConstructionLineTool::handlePointInput(CadScene* scene, const QPointF& point)
{
    if (m_state == State::PositionLine) {
        // Eine rechtwinklige Hilfslinie am Durchgangspunkt (mousePos) erzeugen
        auto cline = std::make_unique<CadConstructionLine>(point, point + m_normalVector);
        auto addCmd = std::make_unique<AddEntityCommand>(
            scene->getDocument(), std::move(cline), tr("Add Perpendicular Construction Line"));

        if (scene->getUndoStack()) {
            scene->getUndoStack()->push(std::move(addCmd));
        } else {
            addCmd->execute();
        }

        resetForNextEntity(scene);
    }
}

void PerpendicularConstructionLineTool::resetForNextEntity(CadScene* scene)
{
    m_state = State::SelectBaseEntity;
    m_selectedEntity = nullptr;
    m_selectedSystemLine = nullptr;

    clearHover();

    if (m_previewItem) {
        scene->removeItem(m_previewItem);
        delete m_previewItem;
        m_previewItem = nullptr;
    }

    emit promptTextChanged(m_promptMsg01);
}

void PerpendicularConstructionLineTool::cancelDrawing(CadScene* scene)
{
    resetForNextEntity(scene);
}

void PerpendicularConstructionLineTool::updateHoverEntity(CadScene* scene, const QPointF& mousePos)
{
    double zoomFactor = 1.0;
    const auto views = scene->views();
    if (!views.isEmpty() && views.first()) {
        zoomFactor = views.first()->transform().m11();
    }

    const double toleranceWorld = 10.0 / std::abs(zoomFactor);

    QRectF searchRect(mousePos.x() - toleranceWorld,
                      mousePos.y() - toleranceWorld,
                      toleranceWorld * 2.0,
                      toleranceWorld * 2.0);

    const QList<QGraphicsItem*> items = scene->items(searchRect);
    CadEntity* newHoverEntity = nullptr;
    QGraphicsLineItem* newHoverSystemLine = nullptr;

    for (QGraphicsItem* item : items) {
        if (item == scene->getCrosshairItem()) continue;

        auto entity = item->data(Qt::UserRole).value<CadEntity*>();
        if (entity) {
            auto type = entity->type();
            if (type == EntityType::Line ||
                type == EntityType::ConstructionHvLine ||
                type == EntityType::ConstructionLine)
            {
                newHoverEntity = entity;
                break;
            }
        }
        else if (item->data(Qt::UserRole + 1).toString() == "SystemItem") {
            if (auto* lineItem = dynamic_cast<QGraphicsLineItem*>(item)) {
                newHoverSystemLine = lineItem;
                break;
            }
        }
    }

    if (m_hoveredEntity != newHoverEntity || m_hoveredSystemLine != newHoverSystemLine) {
        clearHover();
        m_hoveredEntity = newHoverEntity;
        m_hoveredSystemLine = newHoverSystemLine;

        QGraphicsLineItem* targetItem = nullptr;
        if (m_hoveredEntity) {
            targetItem = dynamic_cast<QGraphicsLineItem*>(m_hoveredEntity->getGraphicsItem());
        } else if (m_hoveredSystemLine) {
            targetItem = m_hoveredSystemLine;
        }

        if (targetItem) {
            QPen pen = targetItem->pen();
            pen.setColor(Qt::magenta);
            pen.setWidth(0);
            targetItem->setPen(pen);
        }
    }
}

void PerpendicularConstructionLineTool::clearHover()
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
    else if (m_hoveredSystemLine) {
        QList<qreal> pattern;
        pattern << 9.0 << 3.0 << 3.0 << 3.0 << 3.0 << 3.0;
        QPen redPen(Qt::red, 0);
        redPen.setDashPattern(pattern);
        m_hoveredSystemLine->setPen(redPen);
    }

    m_hoveredEntity = nullptr;
    m_hoveredSystemLine = nullptr;
}

bool PerpendicularConstructionLineTool::getEntityLineParams(const CadEntity* entity, QPointF& basePoint, QPointF& dirVector, QPointF& normalVector) const
{
    QPointF p1, p2;

    if (entity) {
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
    }
    else if (m_selectedSystemLine) {
        QLineF line = m_selectedSystemLine->line();
        p1 = line.p1();
        p2 = line.p2();
    } else {
        return false;
    }

    basePoint = p1;
    QPointF diff = p2 - p1;
    double len = std::hypot(diff.x(), diff.y());
    if (len < 1e-9) return false;

    dirVector = diff / len;
    // Der Normalenvektor entspricht der 90°-Richtung zur ausgewählten Linie
    normalVector = QPointF(-dirVector.y(), dirVector.x());
    return true;
}

void PerpendicularConstructionLineTool::updatePreview(CadScene* scene, const QPointF& mousePos)
{
    if (m_previewItem) {
        scene->removeItem(m_previewItem);
        delete m_previewItem;
        m_previewItem = nullptr;
    }

    if (!m_selectedEntity && !m_selectedSystemLine) return;

    constexpr double maxDist = 1000000.0;
    QPen previewPen(Qt::gray, 0, Qt::SolidLine);

    // Hilfslinie verläuft durch den Klick-/Mauspunkt in Richtung des Normalenvektors (90°)
    QLineF previewLine(mousePos - m_normalVector * maxDist, mousePos + m_normalVector * maxDist);
    m_previewItem = scene->addLine(previewLine, previewPen);
}
/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "construction_line_tool.h"
#include "../cad_document/cad_construction_line.h"
#include "../commands/add_entity_command.h"
#include "../undo_stack.h"
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QPen>

void ConstructionLineTool::retranslate()
{
    m_promptMsg01 = tr("Construction Line 2 Points: Click first point or enter coordinates (x, y).");
    m_promptMsg02 = tr("Construction Line 2 Points: Click second point or enter coordinates (x, y).");
}

void ConstructionLineTool::activate(CadScene* scene)
{
    Q_UNUSED(scene);
    cancelDrawing(scene);
    emit promptTextChanged(m_promptMsg01);
}

void ConstructionLineTool::deactivate(CadScene* scene)
{
    cancelDrawing(scene);
}

void ConstructionLineTool::cancel(CadScene* scene)
{
    cancelDrawing(scene);
}

void ConstructionLineTool::mousePressEvent(CadScene* scene, QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QPointF pos = scene->getSnapOrPosition(event->scenePos());
        stateMachine(scene, pos);
    }
}

void ConstructionLineTool::mouseMoveEvent(CadScene* scene, QGraphicsSceneMouseEvent* event)
{
    m_currentMousePos = scene->getSnapOrPosition(event->scenePos());

    // Vorschau aktualisieren: Ganz normale Linie von m_p1 zur aktuellen Mausposition
    if (m_tempLine) {
        m_tempLine->setLine(QLineF(m_p1, m_currentMousePos));
    }
}

void ConstructionLineTool::keyPressEvent(CadScene* scene, QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape && m_state == ToolState::Drawing) {
        cancelDrawing(scene);
    }
}

void ConstructionLineTool::handlePointInput(CadScene* scene, const QPointF& point)
{
    stateMachine(scene, point);
}

void ConstructionLineTool::cancelDrawing(CadScene* scene)
{
    m_state = ToolState::Idle;
    if (m_tempLine) {
        scene->removeItem(m_tempLine);
        delete m_tempLine;
        m_tempLine = nullptr;
    }
    emit promptTextChanged(m_promptMsg01);
}

void ConstructionLineTool::stateMachine(CadScene* scene, const QPointF& point)
{
    if (m_state == ToolState::Idle) {
        m_state = ToolState::Drawing;
        m_p1 = point;
        scene->setLastPoint(m_p1);

        // Vorschau erzeugen: Einfache gestrichelte temporäre Linie zwischen m_p1 und sich selbst
        m_tempLine = scene->addLine(QLineF(m_p1, m_p1), QPen(Qt::gray, 0, Qt::SolidLine));
        emit promptTextChanged(m_promptMsg02);
    }
    else if (m_state == ToolState::Drawing) {
        m_state = ToolState::Idle;

        // Temporäre Vorschau-Linie entfernen
        if (m_tempLine) {
            scene->removeItem(m_tempLine);
            delete m_tempLine;
            m_tempLine = nullptr;
        }

        // Erst HIER entsteht das echte unendliche Hilfslinien-Objekt
        if (QLineF(m_p1, point).length() > 1e-9) {
            auto cline = std::make_unique<CadConstructionLine>(m_p1, point);
            auto command = std::make_unique<AddEntityCommand>(
                scene->getDocument(), std::move(cline), tr("Add 2-Point Construction Line"));

            if (scene->getUndoStack()) {
                scene->getUndoStack()->push(std::move(command));
            } else {
                command->execute();
            }
        }
        emit promptTextChanged(m_promptMsg01);
    }
}
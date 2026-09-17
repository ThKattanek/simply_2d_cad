/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
*/

#include "construction_hv_line_tool.h"
#include "../cad_document/cad_construction_hv_line.h"
#include "../commands/add_entity_command.h"
#include "../undo_stack.h"
#include <QGraphicsSceneMouseEvent>

void ConstructionHvLineTool::retranslate()
{
    m_promptMsg01 = tr("Construction Horizont Line: Click a point or enter y value.");
    m_promptMsg02 = tr("Construction Vertical Line: Click a point or enter x value.");
}

void ConstructionHvLineTool::activate(CadScene* scene)
{
    Q_UNUSED(scene);
    if(getToolMode() == 1) { // Vertical
        emit promptTextChanged(m_promptMsg02);
    } else { // Horizontal
        emit promptTextChanged(m_promptMsg01);
    }
}

void ConstructionHvLineTool::mousePressEvent(CadScene* scene, QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QPointF pos = scene->getSnapOrPosition(event->scenePos());
        createLine(scene, pos);
    }
}

void ConstructionHvLineTool::handlePointInput(CadScene* scene, const QPointF& point)
{
    createLine(scene, point);
}

void ConstructionHvLineTool::handleValueInput(CadScene *scene, double value)
{
    QPointF pos;
    if (getToolMode() == 1) { // Vertical
        pos = QPointF(value, 0); // y-coordinate will be ignored for vertical lines
    } else { // Horizontal
        pos = QPointF(0, value); // x-coordinate will be ignored for horizontal lines
    }

    createLine(scene, pos);
}

void ConstructionHvLineTool::createLine(CadScene* scene, const QPointF& pos)
{
    auto orientation = (getToolMode() == 1) ? ConstructionLineOrientation::Vertical
                                            : ConstructionLineOrientation::Horizontal;

    auto cline = std::make_unique<CadConstructionHvLine>(pos, orientation);
    auto command = std::make_unique<AddEntityCommand>(
        scene->getDocument(), std::move(cline), tr("Add Construction Line"));

    if (scene->getUndoStack()) {
        scene->getUndoStack()->push(std::move(command));
    } else {
        command->execute();
    }
}
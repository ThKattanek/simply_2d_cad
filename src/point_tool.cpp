/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "./point_tool.h"
#include "./cad_scene.h"
#include "./cad_document/cad_point.h"

#include <QGraphicsSceneMouseEvent>

void PointTool::mousePressEvent(CadScene *scene, QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QPointF currentPosition = scene->getSnapOrPosition(event->scenePos());
        pointStateMachine(scene, currentPosition);
    }
}

void PointTool::handlePointInput(CadScene *scene, const QPointF &point)
{
    pointStateMachine(scene, point);
}

void PointTool::deactivate(CadScene *)
{
}

void PointTool::pointStateMachine(CadScene *scene, const QPointF &point)
{
    auto newPoint = std::make_unique<CadPoint>(point);
    scene->getDocument()->addEntity(std::move(newPoint));
}

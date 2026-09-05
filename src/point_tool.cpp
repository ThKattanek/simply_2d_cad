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
        // Add a point at the clicked position
        QPointF currentPosition = scene->getSnapOrPosition(event->scenePos());
        auto newPoint = std::make_unique<CadPoint>(currentPosition);
        scene->getDocument()->addEntity(std::move(newPoint));
    }
}

void PointTool::deactivate(CadScene *)
{
}

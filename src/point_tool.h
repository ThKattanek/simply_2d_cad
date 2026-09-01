/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

#include "cad_tool.h"

class QGraphicsLineItem;

class PointTool : public CadTool
{
public:
    void mousePressEvent(CadScene* scene, QGraphicsSceneMouseEvent* event) override;
    void deactivate(CadScene* scene) override;
};

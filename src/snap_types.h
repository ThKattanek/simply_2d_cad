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

#include <QPointF>

enum class SnapType
{
    None = 0,
    Grid,               // Snap to grid points
    Endpoint,           // Snap to endpoints of lines
    Midpoint,           // Snap to midpoints of lines
    Center,             // Snap to center of circles or arcs
    Intersection        // Snap to intersection points of lines
};

struct SnapResult
{
    bool snapped = false;
    QPointF point;
    SnapType type = SnapType::None;
    double distance = 0.0; // Distance from the original point to the snapped point
};
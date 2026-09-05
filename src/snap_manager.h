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
#include "./snap_types.h"

class CadDocument;

class SnapManager
{
public:
    SnapManager() = default;

    void setSnapTolerancePixels(double pixels) {m_tolerancePixels = pixels;}
    SnapResult findSnapPoint(const QPointF& mouseWorldPos, const CadDocument& doc, double zoomFactor);

private:
    double m_tolerancePixels = 10.0; // Default snap tolerance in pixels

    void checkEndpoints(const QPointF& mousePos, const CadDocument& doc, double maxDistWorld, SnapResult& result);
    void checkGrid(const QPointF& mousePos, double maxDistWorld, SnapResult& result);
};
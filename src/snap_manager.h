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

    SnapResult findSnapPoint(const QPointF& mouseWorldPos, const CadDocument& doc, double zoomFactor);

    void setSnapTolerancePixels(double pixels) {m_tolerancePixels = pixels;}
    void setGridSnapEnabled(bool enabled) { m_gridSnapEnabled = enabled; }
    void setPointSnapEnabled(bool enabled) { m_pointSnapEnabled = enabled; }
    void setEndpointSnapEnabled(bool enabled) { m_endpointSnapEnabled = enabled; }
    void setMidpointSnapEnabled(bool enabled) { m_midpointSnapEnabled = enabled; }
    void setIntersectionSnapEnabled(bool enabled) { m_intersectionSnapEnabled = enabled; }

private:
    void checkEndpoints(const QPointF& mousePos, const CadDocument& doc, double maxDistWorld, SnapResult& result);
    void checkGrid(const QPointF& mousePos, double maxDistWorld, SnapResult& result);

    double m_tolerancePixels = 10.0; // Default snap tolerance in pixels

    bool m_gridSnapEnabled = true;
    bool m_pointSnapEnabled = true;
    bool m_endpointSnapEnabled = true;
    bool m_midpointSnapEnabled = true;
    bool m_intersectionSnapEnabled = true;
};
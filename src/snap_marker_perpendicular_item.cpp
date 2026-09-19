/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "snap_marker_perpendicular_item.h"

#include <QPainter>

SnapMarkerPerpendicularItem::SnapMarkerPerpendicularItem(QGraphicsItem *parent)
{
    Q_UNUSED(parent);
}

QRectF SnapMarkerPerpendicularItem::boundingRect() const
{
    return QRectF(-m_size/2.0, -m_size/2.0, m_size, m_size);
}

void SnapMarkerPerpendicularItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    const double half = m_size / 2.0;

    painter->setRenderHint(QPainter::Antialiasing, false);
    QPen pen(m_color, 2);
    pen.setCosmetic(true);
    painter->setPen(pen);

    // Zeichnet einen kleinen 90°-Winkel (wie das klassische Perpendicular-Icon)
    QPainterPath path;
    path.moveTo(-half, half);
    path.lineTo(-half, -half);
    path.lineTo(half, -half);

    // Kleines Rechteck im Winkel für das 90°-Symbol
    path.moveTo(-half, 0);
    path.lineTo(0, 0);
    path.lineTo(0, -half);

    painter->drawPath(path);
}

void SnapMarkerPerpendicularItem::setSize(double size)
{
    if (m_size == size) return;
    m_size = size;

    prepareGeometryChange();    // Notify the scene that the item's geometry is about to change
    update();
}

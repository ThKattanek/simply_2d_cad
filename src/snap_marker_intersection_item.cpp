/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "snap_marker_intersection_item.h"

#include <QPainter>

SnapMarkerIntersectionItem::SnapMarkerIntersectionItem(QGraphicsItem *parent) : QGraphicsItem(parent) {}

QRectF SnapMarkerIntersectionItem::boundingRect() const
{
    return QRectF(-m_size/2.0, -m_size/2.0, m_size, m_size);
}

void SnapMarkerIntersectionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    const double half = m_size / 2.0;

    painter->setRenderHint(QPainter::Antialiasing, false); // Für scharfe Pixel-Linien

    QPen pen(m_color, 2);
    pen.setCosmetic(true); // Bleibt beim Zoomen immer 2 Pixel dick!

    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    // 1. Mittleres 'X' (Hauptlinien)
    painter->drawLine(QPointF(-half, -half), QPointF(half, half));
    painter->drawLine(QPointF(-half, half), QPointF(half, -half));
}

void SnapMarkerIntersectionItem::setSize(double size)
{
    if (m_size == size) return;
    m_size = size;

    prepareGeometryChange();    // Notify the scene that the item's geometry is about to change
    update();
}

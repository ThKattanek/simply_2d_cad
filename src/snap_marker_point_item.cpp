/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "snap_marker_point_item.h"

#include <QPainter>

SnapMarkerPointItem::SnapMarkerPointItem(QGraphicsItem *parent) : QGraphicsItem(parent) {}

QRectF SnapMarkerPointItem::boundingRect() const
{
    return QRectF(-m_size/2.0, -m_size/2.0, m_size, m_size);
}

void SnapMarkerPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    const double half = m_size / 2.0;

    painter->setRenderHint(QPainter::Antialiasing, false); // Für scharfe Pixel-Kanten

    QPen pen(m_color, 2);
    pen.setCosmetic(true); // Bleibt beim Zoomen immer 2 Pixel dick!

    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    painter->drawRect(QRectF(-half, -half, m_size, m_size));
}

void SnapMarkerPointItem::setSize(double size)
{
    if (m_size == size) return;
    m_size = size;

    prepareGeometryChange();    // Notify the scene that the item's geometry is about to change
    update();
}

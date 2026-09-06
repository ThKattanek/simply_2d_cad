/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "snap_marker_midpoint_item.h"

#include <QPainter>

SnapMarkerMidpointItem::SnapMarkerMidpointItem(QGraphicsItem *parent) : QGraphicsItem(parent) {}

QRectF SnapMarkerMidpointItem::boundingRect() const
{
    return QRectF(-m_size/2.0, -m_size/2.0, m_size, m_size);
}

void SnapMarkerMidpointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing, false); // Für scharfe Pixel-Kanten
    painter->setPen(QPen(m_color, 0)); // Kosmischer Pen (immer 1px breit)

    const double halfWidth = m_size / 2.0;

    // Höhe eines gleichseitigen Dreiecks (h = sqrt(3)/2 * a)
    const double height = (std::sqrt(3.0) / 2.0) * m_size;
    const double halfHeight = height / 2.0;

    // Drei Eckpunkte (Spitze oben, Basis unten) zentriert um (0,0)
    QPolygonF triangle;
    triangle << QPointF(0.0, -halfHeight)           // Spitze oben
             << QPointF(halfWidth, halfHeight)      // Unten rechts
             << QPointF(-halfWidth, halfHeight);    // Unten links

    // Umriss zeichnen
    painter->drawPolygon(triangle);

    // Dicker-Effekt durch 1px versetztes inneres Dreieck (wie beim X)
    const double offset = m_size / 10.0;
    if (halfWidth - offset > 0 && halfHeight - offset > 0) {
        QPolygonF innerTriangle;
        innerTriangle << QPointF(0.0, -halfHeight + offset)
                      << QPointF(halfWidth - offset, halfHeight - (offset / 2.0))
                      << QPointF(-halfWidth + offset, halfHeight - (offset / 2.0));
        painter->drawPolygon(innerTriangle);
    }
}

void SnapMarkerMidpointItem::setSize(double size)
{
    if (m_size == size) return;
    m_size = size;

    prepareGeometryChange();    // Notify the scene that the item's geometry is about to change
    update();
}

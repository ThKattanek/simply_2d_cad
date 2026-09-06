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
#include <QPen>

SnapMarkerIntersectionItem::SnapMarkerIntersectionItem(QGraphicsItem *parent) : QGraphicsItem(parent)
{
}

QRectF SnapMarkerIntersectionItem::boundingRect() const
{
    return QRectF(-m_size/2.0, -m_size/2.0, m_size, m_size);
}

void SnapMarkerIntersectionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->setRenderHint(QPainter::Antialiasing, false); // Für scharfe Pixel-Linien
    painter->setPen(QPen(Qt::magenta, 0));

    const double half = m_size / 2.0;

    // Wir berechnen einen Offset von genau 1 Bildschirm-Pixel in Weltkoordinaten
    // (m_size ist ja bereits in Weltkoordinaten für ~10px Bildschirmgröße umgerechnet)
    // Wenn m_size z. B. 10 Pixel auf dem Schirm entspricht, ist 1 Pixel = m_size / 10.0
    const double pixelOffset = m_size / 10.0;

    // 1. Mittleres 'X' (Hauptlinien)
    painter->drawLine(QPointF(-half, -half), QPointF(half, half));
    painter->drawLine(QPointF(-half, half), QPointF(half, -half));

    // 2. Leicht nach links/rechts versetztes 'X' (macht die Striche optisch 2-3px dick)
    painter->drawLine(QPointF(-half + pixelOffset, -half), QPointF(half + pixelOffset, half));
    painter->drawLine(QPointF(-half - pixelOffset, -half), QPointF(half - pixelOffset, half));

    painter->drawLine(QPointF(-half + pixelOffset, half), QPointF(half + pixelOffset, -half));
    painter->drawLine(QPointF(-half - pixelOffset, half), QPointF(half - pixelOffset, -half));
}

void SnapMarkerIntersectionItem::setSize(double size)
{
    if (m_size == size) return;
    m_size = size;

    prepareGeometryChange();    // Notify the scene that the item's geometry is about to change
    update();
}

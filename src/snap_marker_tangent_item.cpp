/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "snap_marker_tangent_item.h"
#include <QPainter>

SnapMarkerTangentItem::SnapMarkerTangentItem(QGraphicsItem *parent) : QGraphicsItem(parent) {}

QRectF SnapMarkerTangentItem::boundingRect() const
{
    return QRectF(-m_size/2.0, -m_size/2.0, m_size, m_size);
}

void SnapMarkerTangentItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    const double half = m_size / 2.0;

    painter->setRenderHint(QPainter::Antialiasing, true);

    QPen pen(m_color, 2);
    pen.setCosmetic(true);

    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    // 1. Radius und Zentrierung
    double r = half * 0.85;
    QPointF center(-half * 0.25, -half * 0.25);
    painter->drawEllipse(center, r, r);

    // 2. Berührpunkt P auf dem Kreis bei 45° (rechts oben)
    const double invSqrt2 = 0.70710678118;
    QPointF touchPoint(center.x() + r * invSqrt2, center.y() + r * invSqrt2);

    // 3. Tangentenvektor mit verlängerter Linie (1.1 statt 0.7)
    double lineHalfLength = half * 1.1;
    QPointF dir(-invSqrt2 * lineHalfLength, invSqrt2 * lineHalfLength);

    // Tangentenlinie zeichnen
    QPointF lineStart = touchPoint - dir;
    QPointF lineEnd = touchPoint + dir;

    painter->drawLine(lineStart, lineEnd);
}

void SnapMarkerTangentItem::setSize(double size)
{
    if (m_size == size) return;
    m_size = size;

    prepareGeometryChange();
    update();
}
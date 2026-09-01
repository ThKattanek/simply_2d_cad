/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "crosshair_item.h"
#include <QPainter>
#include <QBitmap>

#define CROSSHAIR_SIZE 25   // Size of the crosshair bitmap (25x25 pixels). Must always be an odd number

CrosshairItem::CrosshairItem(QGraphicsItem *parent) : QGraphicsItem(parent)
{
    setAcceptedMouseButtons(Qt::NoButton);              // Crosshair should not accept mouse events
    setFlag(QGraphicsItem::ItemIsSelectable, false);    // Crosshair should not be selectable

    // Set the point to be always not affected by transformations (like scaling or rotation)
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

    m_pen = QPen(Qt::white, 0); // Set the pen color to green and width to 0 (cosmetic pen)
}

QRectF CrosshairItem::boundingRect() const {
    // Sehr großes Bounding-Rechteck um das Fadenkreuz herum

    int offset = -(CROSSHAIR_SIZE / 2); // -12 bei Größe 25
    return QRectF(offset, offset, CROSSHAIR_SIZE, CROSSHAIR_SIZE);
}

void CrosshairItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {    
    painter->setRenderHint(QPainter::Antialiasing, false);

    painter->setPen(m_pen);

    int offset = -(CROSSHAIR_SIZE / 2); // -12 bei Größe 25
    painter->drawPixmap(offset, offset, getPatternBitmap());
}

void CrosshairItem::setPosition(const QPointF &pos) {
    setPos(pos);
    update();
}

const QBitmap &CrosshairItem::getPatternBitmap()
{
    static std::unique_ptr<QBitmap> s_bitmap = nullptr;

    if (!s_bitmap) {

        // Create a 11x11 bitmap with transparent background
        auto bitmap = std::make_unique<QBitmap>(CROSSHAIR_SIZE, CROSSHAIR_SIZE);
        bitmap->fill(Qt::color0); // Fill with transparent color

        // Draw a simple pattern (cross) on the image
        QPainter p(bitmap.get());
        p.setPen(QPen(Qt::color1, 1));

        int center = (CROSSHAIR_SIZE - 1) / 2; // Pixel 12 bei Größe 25
        p.drawLine(0, center, CROSSHAIR_SIZE - 1, center);
        p.drawLine(center, 0, center, CROSSHAIR_SIZE - 1);

        // Create the pixmap from the image and store it in the static unique_ptr
        s_bitmap = std::move(bitmap);
    }

    return *s_bitmap;
}
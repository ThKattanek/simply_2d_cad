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

#include <QGraphicsItem>

class SnapMarkerIntersectionItem : public QGraphicsItem
{
public:
    explicit SnapMarkerIntersectionItem(QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setSize(double size);
    void setColor(const QColor &newColor) { m_color = newColor; update(); }

private:
    double m_size = 10.0;
    QColor m_color = Qt::red;
};

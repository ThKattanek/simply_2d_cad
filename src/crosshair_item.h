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
#include <QPen>

class CrosshairItem : public QGraphicsItem
{
public:
    explicit CrosshairItem(QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setPosition(const QPointF &position);
    void setColor(const QColor &color) { m_pen.setColor(color); update(); }

private:
    static const QBitmap& getPatternBitmap();

    QPointF m_position;
    QPen m_pen;
};
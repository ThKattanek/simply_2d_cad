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

#include "cad_entity.h"
#include <QPen>
#include <QLineF>
#include <cmath>

class CadConstructionLine : public CadEntity
{
public:
    CadConstructionLine(const QPointF& p1, const QPointF& p2)
        : m_p1(p1), m_p2(p2) {}

    EntityType type() const override { return EntityType::ConstructionLine; }

    void serialize(QDataStream& stream) const override {
        stream << static_cast<quint8>(EntityType::ConstructionLine);
        stream << m_p1 << m_p2 << m_layer;
    }

    void deserialize(QDataStream& stream) override {
        stream >> m_p1 >> m_p2 >> m_layer;
        updateGraphicsItem();
    }

    QGraphicsItem* createGraphicsItem() override {
        auto item = new QGraphicsLineItem(calculateVisualLine());

        QPen pen(Qt::red, 0, Qt::DashLine);
        pen.setCosmetic(true);
        item->setPen(pen);

        item->setData(Qt::UserRole, QVariant::fromValue(static_cast<CadEntity*>(this)));
        item->setData(Qt::UserRole + 1, "ConstructionItem");
        m_graphicsItem = item;
        return item;
    }

    void updateGraphicsItem() override {
        if (auto item = dynamic_cast<QGraphicsLineItem*>(m_graphicsItem)) {
            item->setLine(calculateVisualLine());
        }
    }

    QPointF p1() const { return m_p1; }
    QPointF p2() const { return m_p2; }
    void setP1(const QPointF& p1) { m_p1 = p1; updateGraphicsItem(); }
    void setP2(const QPointF& p2) { m_p2 = p2; updateGraphicsItem(); }

private:
    QLineF calculateVisualLine() const {
        QPointF dir = m_p2 - m_p1;
        double len = std::hypot(dir.x(), dir.y());
        if (len < 1e-9) {
            dir = QPointF(1.0, 0.0);
        } else {
            dir /= len;
        }

        constexpr double maxDist = 1000000.0;
        QPointF startPt = m_p1 - dir * maxDist;
        QPointF endPt = m_p1 + dir * maxDist;

        return QLineF(startPt, endPt);
    }

    QPointF m_p1;
    QPointF m_p2;
};

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

enum class ConstructionLineOrientation
{
    Horizontal,
    Vertical
};

class CadConstructionHvLine : public CadEntity
{
public:
    CadConstructionHvLine(const QPointF& pos, ConstructionLineOrientation orientation)
        : m_pos(pos), m_orientation(orientation) {}

    EntityType type() const override { return EntityType::Line; }

    void serialize(QDataStream& stream) const override {
        stream << static_cast<quint8>(EntityType::Line);
        stream << m_pos << static_cast<quint8>(m_orientation) << m_layer;
    }

    void deserialize(QDataStream& stream) override {
        quint8 orient;
        stream >> m_pos >> orient >> m_layer;
        m_orientation = static_cast<ConstructionLineOrientation>(orient);
        updateGraphicsItem();
    }

    QGraphicsItem* createGraphicsItem() override {
        // Riesige Koordinaten wählen, um den Bildschirm in alle Richtungen abzudecken
        QLineF line;
        if (m_orientation == ConstructionLineOrientation::Horizontal) {
            line = QLineF(-1000000, m_pos.y(), 1000000, m_pos.y());
        } else if(m_orientation == ConstructionLineOrientation::Vertical) {
            line = QLineF(m_pos.x(), -1000000, m_pos.x(), 1000000);
        }

        auto item = new QGraphicsLineItem(line);

        // Gestrichelte rote Linie definieren
        QPen pen(Qt::red, 0, Qt::DashLine);
        pen.setCosmetic(true); // Bleibt beim Zoomen immer gleich dick
        item->setPen(pen);

        item->setData(Qt::UserRole, QVariant::fromValue(static_cast<CadEntity*>(this)));
        m_graphicsItem = item;
        return item;
    }

    void updateGraphicsItem() override {
        if (auto item = dynamic_cast<QGraphicsLineItem*>(m_graphicsItem)) {
            if (m_orientation == ConstructionLineOrientation::Horizontal) {
                item->setLine(QLineF(-1000000, m_pos.y(), 1000000, m_pos.y()));
            } else {
                item->setLine(QLineF(m_pos.x(), -1000000, m_pos.x(), 1000000));
            }
        }
    }

    QPointF getPosition() const { return m_pos; }
    ConstructionLineOrientation getOrientation() const { return m_orientation; }

private:
    QPointF m_pos;
    ConstructionLineOrientation m_orientation;
};
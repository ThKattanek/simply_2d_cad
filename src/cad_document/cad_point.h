#pragma once

#include "./cad_entity.h"
#include "../cad_point_item.h"
#include <QPointF>

// CadPoint class represents a point entity in the CAD document.

class CadPoint : public CadEntity {
public:
    explicit CadPoint(const QPointF& pos) : m_pos(pos) {}

    EntityType type() const override { return EntityType::Point; }

    void serialize(QDataStream& stream) const override {
        stream << static_cast<quint8>(EntityType::Point);
        stream << m_pos << m_layer;
    }

    void deserialize(QDataStream& stream) override {
        stream >> m_pos >> m_layer;
        updateGraphicsItem();
    }

    QGraphicsItem* createGraphicsItem() override {
        auto item = new CadPointItem();
        item->setPos(m_pos);

        item->setData(Qt::UserRole, QVariant::fromValue(static_cast<void*>(this)));

        m_graphicsItem = item;
        return item;
    }

    void updateGraphicsItem() override {
        if (m_graphicsItem) {
            m_graphicsItem->setPos(m_pos);
        }
    }

    QPointF position() const { return m_pos; }
    void setPosition(const QPointF& pos) { m_pos = pos; updateGraphicsItem(); }

private:
    QPointF m_pos;
};
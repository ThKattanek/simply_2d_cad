#pragma once

#include "./cad_entity.h"
#include <QPen>

class CadCircle : public CadEntity {

public:
explicit CadCircle(const QPointF& center, const qreal& radius)
    : m_center(center), m_radius(radius) {}

    EntityType type() const override { return EntityType::Circle; }

    void serialize(QDataStream& stream) const override {
        // 1. Typ schreiben (1 Byte)
        stream << static_cast<quint8>(EntityType::Circle);
        // 2. Geometriedaten binär schreiben
        stream << m_center << m_radius << m_layer;
    }

    void deserialize(QDataStream& stream) override {
        // Geometriedaten in exakt derselben Reihenfolge auslesen
        stream >> m_center >> m_radius >> m_layer;
        updateGraphicsItem();
    }

    QGraphicsItem* createGraphicsItem() override {
        auto item = new QGraphicsEllipseItem(m_center.x() - m_radius, m_center.y() - m_radius, 2.0 * m_radius, 2.0 * m_radius);
        item->setPen(QPen(Qt::white, 0)); // Cosmetic Pen
        //item->setFlag(QGraphicsItem::ItemIsSelectable);

        // Rückverweis auf dieses Datenobjekt speichern
        item->setData(Qt::UserRole, QVariant::fromValue(static_cast<void*>(this)));

        m_graphicsItem = item;
        return item;
    }

    void updateGraphicsItem() override {
        if (auto item = dynamic_cast<QGraphicsEllipseItem*>(m_graphicsItem)) {
            item->setRect(QRectF(m_center.x() - m_radius, m_center.y() - m_radius, 2.0 * m_radius, 2.0 * m_radius));
        }
    }

    QPointF center() const { return m_center; }
    qreal radius() const { return m_radius; }
    void setCenter(QPointF center) { m_center = center; }
    void setRadius(qreal radius) { m_radius = radius; }

private:
    QPointF m_center;
    qreal m_radius;
};

#pragma once

#include "./cad_entity.h"
#include <QPointF>
#include <QPen>

// CadLine class represents a line entity in the CAD document.

class CadLine : public CadEntity {
public:
    CadLine(const QPointF& start, const QPointF& end)
        : m_start(start), m_end(end) {}

    EntityType type() const override { return EntityType::Line; }

    void serialize(QDataStream& stream) const override {
        // 1. Typ schreiben (1 Byte)
        stream << static_cast<quint8>(EntityType::Line);
        // 2. Geometriedaten binär schreiben
        stream << m_start << m_end << m_layer;
    }

    void deserialize(QDataStream& stream) override {
        // Geometriedaten in exakt derselben Reihenfolge auslesen
        stream >> m_start >> m_end >> m_layer;
        updateGraphicsItem();
    }

    QGraphicsItem* createGraphicsItem() override {
        auto item = new QGraphicsLineItem(QLineF(m_start, m_end));
        item->setPen(QPen(Qt::white, 0)); // Cosmetic Pen
        //item->setFlag(QGraphicsItem::ItemIsSelectable);

        // Rückverweis auf dieses Datenobjekt speichern
        item->setData(Qt::UserRole, QVariant::fromValue(static_cast<void*>(this)));

        m_graphicsItem = item;
        return item;
    }

    void updateGraphicsItem() override {
        if (auto item = dynamic_cast<QGraphicsLineItem*>(m_graphicsItem)) {
            item->setLine(QLineF(m_start, m_end));
        }
    }

    QPointF start() const { return m_start; }
    QPointF end() const { return m_end; }
    void setStart(const QPointF& start) { m_start = start; updateGraphicsItem(); }
    void setEnd(const QPointF& end) { m_end = end; updateGraphicsItem(); }

private:
    QPointF m_start;
    QPointF m_end;
};

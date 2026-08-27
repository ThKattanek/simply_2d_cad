#pragma once

#include <QGraphicsItem>
#include <qpen.h>

class CrosshairItem : public QGraphicsItem
{
public:
    explicit CrosshairItem(QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setSize(qreal size) { m_size = size; }
    void setPosition(const QPointF &position);
    void setColor(const QColor &color) { m_pen.setColor(color); update(); }

private:
    qreal m_size = 20.0; // Default size of the crosshair arms
    QPointF m_position;
    QPen m_pen;
};
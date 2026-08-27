#pragma once

#include <QGraphicsItem>
#include <QPen>

class CadPointItem : public QGraphicsItem
{
public:
    explicit CadPointItem(QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setSize(qreal radius) { m_radius = radius; update(); }

private:
    QPen m_pen;
    qreal m_radius = 8.0; // Radius of the point in pixels
};

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
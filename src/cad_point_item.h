#pragma once

#include <QGraphicsItem>
#include <QPen>

class CadPointItem : public QGraphicsItem
{
public:
    explicit CadPointItem(QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    static const QBitmap& getPatternBitmap();

    QPen m_pen;
    bool m_isHovered = false;
};

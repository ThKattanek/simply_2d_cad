#include "crosshair_item.h"
#include <QPainter>

CrosshairItem::CrosshairItem(QGraphicsItem *parent) : QGraphicsItem(parent)
{
    // set the crosshair to be always on top of other items
    setZValue(1000); // Ensure the crosshair is on top of other items

    setAcceptedMouseButtons(Qt::NoButton); // Crosshair should not accept mouse events
    setFlag(QGraphicsItem::ItemIsSelectable, false); // Crosshair should not be selectable

    m_pen = QPen(Qt::green, 0); // Set the pen color to green and width to 0 (cosmetic pen)
}

QRectF CrosshairItem::boundingRect() const {
    // Sehr großes Bounding-Rechteck um das Fadenkreuz herum
    return QRectF(m_position.x() - 50000, m_position.y() - 50000, 100000, 100000);
}

void CrosshairItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    painter->setPen(m_pen);

    // Horizontale & Vertikale Linie
    painter->drawLine(QPointF(m_position.x() - m_size, m_position.y()), QPointF(m_position.x() + m_size, m_position.y()));
    painter->drawLine(QPointF(m_position.x(), m_position.y() - m_size), QPointF(m_position.x(), m_position.y() + m_size));
}

void CrosshairItem::setPosition(const QPointF &pos) {
    prepareGeometryChange();
    m_position = pos;
    update();
}
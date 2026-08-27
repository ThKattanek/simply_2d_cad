#include "cadpointitem.h"
#include <QPainter>

CadPointItem::CadPointItem(QGraphicsItem *parent) : QGraphicsItem(parent)
{
    // Set the point to be always not affected by transformations (like scaling or rotation)
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

    // Set the point to be selectable
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    // Set the point to be movable
    m_pen = QPen(Qt::white, 0); // Set the pen color to red and width to 0 (cosmetic pen)
}

QRectF CadPointItem::boundingRect() const
{
    return QRectF(-m_radius, -m_radius, 2 * m_radius, 2 * m_radius);
}

void CadPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(m_pen);

    qreal r = m_radius;   // Länge der Hauptachsen (0°, 90°, 180°, 270°)
    qreal d = m_radius / 1.414; // Länge der Diagonalen (45°, 135°, 225°, 315°)

    // 8 Linien zeichnen (Stern aus 4 durchgehenden Achsen)
    painter->drawLine(QPointF(-r,  0), QPointF( r,  0)); // Horizontal
    painter->drawLine(QPointF( 0, -r), QPointF( 0,  r)); // Vertikal
    painter->drawLine(QPointF(-d, -d), QPointF( d,  d)); // Diagonal 1
    painter->drawLine(QPointF(-d,  d), QPointF( d, -d)); // Diagonal 2
}


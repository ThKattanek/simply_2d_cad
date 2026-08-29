#include "cad_point_item.h"
#include <QPainter>
#include <qbitmap.h>

CadPointItem::CadPointItem(QGraphicsItem *parent) : QGraphicsItem(parent)
{
    // Set the point to be always not affected by transformations (like scaling or rotation)
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

    // Set the point to be selectable
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    // Set the point to be movable
    setAcceptHoverEvents(true);

    // Set the point to be movable
    m_pen = QPen(Qt::white, 0); // Set the pen color to red and width to 0 (cosmetic pen)
}

QRectF CadPointItem::boundingRect() const
{
    return QRectF(-5, -5, 11, 11);
}

void CadPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing, false);

    QColor color = Qt::white;
    if(isSelected())
    {
        color = Qt::green; // Change color when selected
    }
    else if(m_isHovered)
    {
        color = Qt::red; // Change color when hovered
    }

    painter->setPen(color);
    painter->drawPixmap(-5,-5, getPatternBitmap());
}

void CadPointItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    m_isHovered = true;
    update();
    QGraphicsItem::hoverEnterEvent(event);
}

void CadPointItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_isHovered = false;
    update();
    QGraphicsItem::hoverLeaveEvent(event);
}

const QBitmap &CadPointItem::getPatternBitmap()
{
    static std::unique_ptr<QBitmap> s_bitmap = nullptr;

    if (!s_bitmap) {

        // Create a 11x11 bitmap with transparent background
        auto bitmap = std::make_unique<QBitmap>(11, 11);
        bitmap->fill(Qt::color0); // Fill with transparent color

        // Draw a simple pattern (cross) on the image
        QPainter p(bitmap.get());
        p.setPen(QPen(Qt::color1, 1));

        p.drawLine(1, 1, 9, 9);
        p.drawLine(1, 9, 9, 1);
        p.drawLine(0, 5, 10, 5);
        p.drawLine(5, 0, 5, 10);

        // Create the pixmap from the image and store it in the static unique_ptr
        s_bitmap = std::move(bitmap);
    }

    return *s_bitmap;
}


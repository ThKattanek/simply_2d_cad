#include "cadview.h"
#include <QWheelEvent>
#include <qgraphicsitem.h>

CADView::CADView(QGraphicsScene *scene, QWidget *parent) : QGraphicsView(scene, parent)
{
    // Set the scene passed in the constructor
    setScene(scene);

    // Invert the Y-axis to match the typical CAD coordinate system (Y upwards)
    scale(1, -1);

    // Set cursor to a cross to emphasize the CAD nature
    this->setCursor(Qt::CrossCursor);

    // Antialiasing aktivieren, damit Linien und Texte glatter aussehen
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);

    // Set the background color to black for a classic CAD look
    setBackgroundBrush(QColor(0,0,0));

    // Set the transformation and resize anchors to the mouse position for zooming and resizing around the mouse
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    // deactivate scrollbars
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void CADView::showEvent(QShowEvent *event)
{
    QGraphicsView::showEvent(event);
    this->fitInView(QRectF(-100, -100, 200, 200), Qt::KeepAspectRatio);
}
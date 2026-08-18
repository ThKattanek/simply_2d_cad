#include "cadview.h"
#include <QWheelEvent>
#include <qgraphicsitem.h>

CADView::CADView(QGraphicsScene *scene, QWidget *parent) : QGraphicsView(scene, parent)
{
    // Die Szene setzen, die im Konstruktor übergeben wurde
    setScene(scene);

    // Die Y-Achse invertieren, damit die Koordinaten wie in CAD üblich sind (Y nach oben)
    scale(1, -1);

    // Cursor auf Kreuz setzen, um den CAD-Charakter zu unterstreichen
    this->setCursor(Qt::CrossCursor);

    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);

    setBackgroundBrush(QColor(0,0,0));

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void CADView::showEvent(QShowEvent *event)
{
    QGraphicsView::showEvent(event);
    this->fitInView(QRectF(-100, -100, 200, 200), Qt::KeepAspectRatio);
}
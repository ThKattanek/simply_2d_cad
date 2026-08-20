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

    // Antialiasing aktivieren, damit Linien und Texte glatter aussehen
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);

    // Hintergrundfarbe auf Schwarz setzen, um den klassischen CAD-Look zu erreichen
    setBackgroundBrush(QColor(0,0,0));

    // Die Transformation und Resize-Ankerpunkte auf die Mausposition setzen, damit Zoom und Resize um die Maus herum erfolgen
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    // Scrollbars deaktivieren, da wir in CAD normalerweise keine benötigen
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void CADView::showEvent(QShowEvent *event)
{
    QGraphicsView::showEvent(event);
    this->fitInView(QRectF(-100, -100, 200, 200), Qt::KeepAspectRatio);
}
#include "cad_view.h"
#include "cad_scene.h"

#include <QWheelEvent>
#include <QGraphicsItem>
#include <QApplication>
#include <QScrollBar>

CadView::CadView(QGraphicsScene *scene, QWidget *parent) : QGraphicsView(scene, parent)
{
    // Set the scene passed in the constructor
    m_cadScene = dynamic_cast<CadScene*>(scene);
    setScene(scene);

    // Invert the Y-axis to match the typical Cad coordinate system (Y upwards)
    scale(1, -1);

    // Enable mouse tracking to capture mouse movements even when no buttons are pressed
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setMouseTracking(true);
    viewport()->setMouseTracking(true);

    // Hide the default cursor, as we will use a custom crosshair cursor in the Cad application
    this->setCursor(Qt::BlankCursor);

    // Antialiasing aktivieren, damit Linien und Texte glatter aussehen
    // setRenderHint(QPainter::Antialiasing);
    // setRenderHint(QPainter::TextAntialiasing);

    // Set the background color to black for a classic Cad look
    setBackgroundBrush(QColor(0,0,0));

    // Set the transformation and resize anchors to the mouse position for zooming and resizing around the mouse
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    // deactivate scrollbars
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void CadView::showEvent(QShowEvent *event)
{
    QGraphicsView::showEvent(event);
    this->fitInView(QRectF(-100, -100, 200, 200), Qt::KeepAspectRatio);
}

void CadView::mousePressEvent(QMouseEvent *event)
{
    // 1. MITTLERE MAUSTASTE: Pan starten
    if (event->button() == Qt::MiddleButton)
    {
        m_isMiddleMousePanning = true;
        m_panStartMousePos = event->pos();

        QApplication::setOverrideCursor(Qt::BlankCursor);
        return; // Kein QGraphicsView::mousePressEvent aufrufen -> Keinerlei Fake-Events, keine Punkte!
    }

    // 2. RECHTE MAUSTASTE: Zoom vorbereiten
    if (event->button() == Qt::RightButton)
    {
        QApplication::setOverrideCursor(Qt::BlankCursor);

        m_isRightMouseZooming = true;
        m_lastMousePos = event->pos();
        m_zoomAnchorScenePos = mapToScene(event->pos());
        return;
    }

    // 3. LINKE MAUSTASTE: Durchreichen an Werkzeuge / Szene
    QGraphicsView::mousePressEvent(event);
}

void CadView::mouseMoveEvent(QMouseEvent *event) {
    // MITTLERE MAUSTASTE: Verschieben (Pan) ausführen
    if (m_isMiddleMousePanning)
    {
        QPoint delta = event->pos() - m_panStartMousePos;
        m_panStartMousePos = event->pos();

        // Scrollbars direkt gemäß der Mausbewegung verschieben
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());

        // HIER DER ZUSATZ: Fadenkreuz-Position auf die NEUE Szenen-Koordinate anpassen
        if (m_cadScene->getCrosshairItem()) {
            m_cadScene->getCrosshairItem()->setPosition(mapToScene(event->pos()));
        }

        return; // Verhindert, dass das Zeichenwerkzeug das Move-Event erhält
    }

    // RECHTE MAUSTASTE: Zoom ausführen
    if (m_isRightMouseZooming)
    {
        int deltaY = m_lastMousePos.y() - event->pos().y();
        m_lastMousePos = event->pos();

        if (deltaY != 0) {
            double factor = (deltaY > 0) ? 1.03 : 0.97;
            double currentScale = transform().m11();

            if ((factor > 1.0 && currentScale < 12000.0) || (factor < 1.0 && currentScale > 0.001)) {
                QPointF anchorViewportPos = mapFromScene(m_zoomAnchorScenePos);
                scale(factor, factor);
                QPointF newCenterScene = m_zoomAnchorScenePos - (mapToScene(anchorViewportPos.toPoint()) - mapToScene(viewport()->rect().center()));
                centerOn(newCenterScene);
            }
        }
        return;
    }

    QGraphicsView::mouseMoveEvent(event);
}

void CadView::mouseReleaseEvent(QMouseEvent *event)
{
    // 1. MITTLERE MAUSTASTE: Pan beenden
    if (event->button() == Qt::MiddleButton)
    {
        m_isMiddleMousePanning = false;
        QApplication::restoreOverrideCursor();
        return;
    }

    // 2. RECHTE MAUSTASTE: Zoom beenden
    if (event->button() == Qt::RightButton)
    {
        m_isRightMouseZooming = false;
        QApplication::restoreOverrideCursor();
        return;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void CadView::wheelEvent(QWheelEvent *event)
{
    // Zooming with the mouse wheel
    event->accept();
}
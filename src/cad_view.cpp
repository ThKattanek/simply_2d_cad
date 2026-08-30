#include "cad_view.h"

#include <QWheelEvent>
#include <QGraphicsItem>
#include <QApplication>

CadView::CadView(QGraphicsScene *scene, QWidget *parent) : QGraphicsView(scene, parent)
{
    // Set the scene passed in the constructor
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
    // 1. MITTLERE MAUSTASTE: Pan (Bildausschnitt verschieben)
    if (event->button() == Qt::MiddleButton)
    {
        QApplication::setOverrideCursor(Qt::BlankCursor);

        setDragMode(QGraphicsView::ScrollHandDrag);

        // Event simulieren, damit QGraphicsView das Verschieben sofort startet
        QMouseEvent fakeEvent(QEvent::MouseButtonPress, event->position(), event->scenePosition(), event->globalPosition(),
                              Qt::LeftButton, event->buttons() | Qt::LeftButton, event->modifiers());
        QGraphicsView::mousePressEvent(&fakeEvent);
        return;
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

    // Für die linke Maustaste (Werkzeuge) normales Verhalten durchreichen
    QGraphicsView::mousePressEvent(event);
}

void CadView::mouseMoveEvent(QMouseEvent *event) {

    if (event->buttons() & Qt::MiddleButton) {
        // Stellt sicher, dass Qt den Hand-Cursor während des Ziehens nicht erzwingt
        viewport()->setCursor(Qt::BlankCursor);
    }

    if (m_isRightMouseZooming) {
        int deltaY = m_lastMousePos.y() - event->pos().y();
        m_lastMousePos = event->pos();

        if (deltaY != 0) {
            double factor = (deltaY > 0) ? 1.03 : 0.97;

            double currentScale = transform().m11();
            if ((factor > 1.0 && currentScale < 12000.0) || (factor < 1.0 && currentScale > 0.001)) {

                // 1. Bildschirm-Position des Klickpunkts VOR dem Skalieren speichern
                QPointF anchorViewportPos = mapFromScene(m_zoomAnchorScenePos);

                // 2. Skalieren durchführen
                scale(factor, factor);

                // 3. Berechnen, wo das neue Zentrum der Szenenansicht liegen muss,
                // damit der geklickte Punkt starr an 'anchorViewportPos' auf dem Bildschirm bleibt
                QPointF newCenterScene = m_zoomAnchorScenePos - (mapToScene(anchorViewportPos.toPoint()) - mapToScene(viewport()->rect().center()));

                // 4. Die View exakt auf dieses neue Szenenzentrum ausrichten
                centerOn(newCenterScene);
            }
        }
        return; // Verhindert, dass das Zeichen-Werkzeug verarbeitet wird
    }

    QGraphicsView::mouseMoveEvent(event);
}

void CadView::mouseReleaseEvent(QMouseEvent *event)
{
    // 1. MITTLERE MAUSTASTE: Pan beenden
    if (event->button() == Qt::MiddleButton)
    {
        QMouseEvent fakeEvent(QEvent::MouseButtonRelease, event->position(), event->scenePosition(), event->globalPosition(),
                              Qt::LeftButton, event->buttons() & ~Qt::LeftButton, event->modifiers());
        QGraphicsView::mouseReleaseEvent(&fakeEvent);
        setDragMode(QGraphicsView::NoDrag);
        QApplication::restoreOverrideCursor();
        return;
    }

    // 2. RECHTE MAUSTASTE: Zoom beenden
    if (event->button() == Qt::RightButton) {
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
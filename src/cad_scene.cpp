/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "./cad_scene.h"
#include "./cad_tool_manager.h"
#include "./snap_types.h"

#include <QGraphicsItem>
#include <QGraphicsSceneEvent>
#include <qgraphicsview.h>
#include <qwidget.h>

#define SCENE_MIN_X -100000
#define SCENE_MAX_X 100000
#define SCENE_MIN_Y -100000
#define SCENE_MAX_Y 100000

CadScene::CadScene(CadToolManager* toolManager, QObject* parent)
    : QGraphicsScene(parent), m_toolManager(toolManager)
{
    // Set the scene rectangle to a large area to accommodate Cad drawings
    setSceneRect(SCENE_MIN_X, SCENE_MIN_Y, SCENE_MAX_X - SCENE_MIN_X, SCENE_MAX_Y - SCENE_MIN_Y);

    // Create and add the crosshair item to the scene
    m_crosshair = new CrosshairItem();
    m_crosshair->setData(Qt::UserRole + 1, "SystemItem");
    m_crosshair->setZValue(10000); // Ensure the crosshair is on top of other items
    m_crosshair->setColor(Qt::white); // Set the color of the crosshair to white
    addItem(m_crosshair);

    // Create a dashed line pattern for the center lines
    QList<qreal> pattern1;
    pattern1 << 9.0   // Strich
            << 3.0   // Lücke
            << 3.0   // Punkt 1
            << 3.0   // Lücke
            << 3.0   // Punkt 2
            << 3.0;  // Lücke vor dem nächsten Strich
    m_dashDotDotPenRed = new QPen(Qt::red, 0);
    m_dashDotDotPenRed->setDashPattern(pattern1);

    // Create a dotted line pattern for the helper lines
    QList<qreal> pattern2;
    pattern2 << 3.0  // Strich
             << 3.0; // Lücke
    m_dotPenRed = new QPen(Qt::red, 0);
    m_dotPenRed->setDashPattern(pattern2);

    // Add the center horizontal and vertical lines to the scene
    m_centerHLine = addLine(SCENE_MIN_X, 0, SCENE_MAX_X, 0, *m_dashDotDotPenRed);
    m_centerHLine->setData(Qt::UserRole + 1, "SystemItem");
    m_centerHLine->setZValue(100);

    m_centerVLine = addLine(0, SCENE_MIN_Y, 0, SCENE_MAX_Y, *m_dashDotDotPenRed);
    m_centerVLine->setData(Qt::UserRole + 1, "SystemItem");
    m_centerVLine->setZValue(100);

    // Add the snap marker to the scene
    m_snapMarker0 = new QGraphicsRectItem();
    m_snapMarker0->setData(Qt::UserRole + 1, "SystemItem");
    m_snapMarker0->setZValue(1000); // over the crosshair
    m_snapMarker0->setPen(QPen(Qt::green, 0));
    m_snapMarker0->setBrush(Qt::NoBrush);
    m_snapMarker0->setVisible(false); // Initially hidden
    addItem(m_snapMarker0);
}

CadScene::~CadScene()
{
    if(m_centerHLine != nullptr)
        delete m_centerHLine;

    if(m_centerVLine != nullptr)
        delete m_centerVLine;

    if(m_crosshair != nullptr)
        delete m_crosshair;

    if(m_dashDotDotPenRed != nullptr)
        delete m_dashDotDotPenRed;

    if(m_dotPenRed != nullptr)
        delete m_dotPenRed;
}

void CadScene::clearDocumentItems()
{
    for (QGraphicsItem* item : items()) {
        if (item->data(Qt::UserRole + 1).toString() == "SystemItem") {
            continue; // System-Item -> Stehen lassen!
        }
        removeItem(item);
        delete item;
    }
}

void CadScene::setDocument(CadDocument *document)
{
    if(m_document == document)
        return;

    m_document = document;

    // Wenn ein Objekt zum Modell hinzugefügt wird -> Item für Grafik-Scene bauen
    connect(m_document, &CadDocument::entityAdded, this, [this](CadEntity* entity) {
        QGraphicsItem* item = entity->createGraphicsItem();
        if (item) {
            addItem(item);
        }
    });

    // Wenn ein Objekt gelöscht wird -> Item aus der Grafik-Scene entfernen
    connect(m_document, &CadDocument::entityRemoved, this, [this](CadEntity* entity) {
        if (QGraphicsItem* item = entity->getGraphicsItem()) {
            removeItem(item);
            delete item; // Löscht die visuelle Darstellung aus der Szene
        }
    });

    // Beim Leeren des Dokuments
    connect(m_document, &CadDocument::documentCleared, this, [this]() {
        clearDocumentItems(); // Leert die QGraphicsScene ohne die SystemElemente wie Crosshair und CenterLines zu löschen
    });
}

void CadScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (auto tool = m_toolManager->activeTool()) {
        tool->mousePressEvent(this, event);
    }
    QGraphicsScene::mousePressEvent(event);
}

void CadScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF rawMousePos = event->scenePos();
    SnapResult snap;

    // Update the position of the crosshair item to follow the mouse cursor
    if(m_crosshair != nullptr) {
        m_crosshair->setPosition(rawMousePos);
    }

    if (m_document) {
        double zoomFactor = getZoomFactorFromEvent(event);
        snap = m_snapManager.findSnapPoint(rawMousePos, *m_document, zoomFactor);
    }

    // 3. Snap-Zustand & Marker aktualisieren
    m_hasActiveSnapPoint = snap.snapped;

    if (snap.snapped) {
        m_activeSnapPoint = snap.point;

        // Marker-Größe maßstabsunabhängig auf dem Bildschirm halten (z. B. 10x10 Pixel)
        const double zoomFactor = getZoomFactorFromEvent(event);
        const double markerSizeWorld = 14.0 / zoomFactor;
        const double halfSize = markerSizeWorld / 2.0;

        // Das Rechteck zentriert auf den Fangpunkt setzen
        m_snapMarker0->setRect(snap.point.x() - halfSize,
                              snap.point.y() - halfSize,
                              markerSizeWorld,
                              markerSizeWorld);

        // Optional: Farbe je nach SnapType anpassen
        if (snap.type == SnapType::Endpoint) {
            m_snapMarker0->setPen(QPen(Qt::red, 0));
        } else if (snap.type == SnapType::Midpoint) {
            m_snapMarker0->setPen(QPen(Qt::cyan, 0));
        }

        m_snapMarker0->setVisible(true);
    } else {
        m_snapMarker0->setVisible(false);
    }

    // 4. Signal für die Statusleiste senden (Zeigt gefangene Koordinate ODER freie Position)
    QPointF displayPos = snap.snapped ? snap.point : rawMousePos;
    emit cursorPositionChanged(displayPos, snap.snapped, snap.type);

    // 5. Werkzeuge aufrufen
    if (auto tool = m_toolManager->activeTool()) {
        tool->mouseMoveEvent(this, event);
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void CadScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (auto tool = m_toolManager->activeTool()) {
        tool->mouseReleaseEvent(this, event);
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void CadScene::keyPressEvent(QKeyEvent *event)
{
    if (auto tool = m_toolManager->activeTool()) {
        tool->keyPressEvent(this, event);
    }
    QGraphicsScene::keyPressEvent(event);
}

double CadScene::getZoomFactorFromEvent(QGraphicsSceneMouseEvent* event) const
{
    if (event && event->widget()) {
        // Der Parent des Viewports ist die QGraphicsView selbst
        if (auto* view = qobject_cast<QGraphicsView*>(event->widget()->parentWidget())) {
            return view->transform().m11(); // Skalierungsfaktor der X-Achse
        }
    }
    return 1.0; // Fallback, falls kein View ermittelt werden kann
}

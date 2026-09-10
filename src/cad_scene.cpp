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
#include "./cad_tools/cad_tool_manager.h"
#include "./snap_types.h"

#include <QGraphicsItem>
#include <QGraphicsSceneEvent>
#include <QGraphicsView>
#include <QWidget>
#include <QSettings>
#include <QEvent>

#define SCENE_MIN_X -100000
#define SCENE_MAX_X 100000
#define SCENE_MIN_Y -100000
#define SCENE_MAX_Y 100000

CadScene::CadScene(CadToolManager* toolManager, QObject* parent)
    : QGraphicsScene(parent), m_toolManager(toolManager)
{
    // Set the scene rectangle to a large area to accommodate Cad drawings
    setSceneRect(SCENE_MIN_X, SCENE_MIN_Y, SCENE_MAX_X - SCENE_MIN_X, SCENE_MAX_Y - SCENE_MIN_Y);
    clearDocument();
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

void CadScene::loadSettings()
{
    QSettings settings;
    m_snapMakerSize = settings.value("Snap/MarkerSize", 10).toInt();
    m_snapManager.setSnapTolerancePixels(settings.value("Snap/TolerancePixels", 10.0).toDouble());

    m_snapManager.setGridSnapEnabled(settings.value("Snap/GridSnapEnabled", true).toBool());
    m_snapManager.setPointSnapEnabled(settings.value("Snap/PointSnapEnabled", true).toBool());
    m_snapManager.setEndpointSnapEnabled(settings.value("Snap/EndpointSnapEnabled", true).toBool());
    m_snapManager.setMidpointSnapEnabled(settings.value("Snap/MidpointSnapEnabled", true).toBool());
    m_snapManager.setIntersectionSnapEnabled(settings.value("Snap/IntersectionSnapEnabled", true).toBool());
}

void CadScene::clearDocument()
{
    cancelCurrentTool();

    clear();

    m_crosshair = nullptr;
    m_centerHLine = nullptr;
    m_centerVLine = nullptr;
    m_snapMarkerPoint = nullptr;
    m_snapMarkerEndpoint = nullptr;
    m_snapMarkerIntersection = nullptr;
    m_snapMarkerMidpoint = nullptr;

    if (m_document) {
        m_document->clear();
    }

    setupSystemItems();

    m_lastPoint = QPointF(0, 0);
    update();
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

void CadScene::handleCommandInputPoint(const QPointF &parsedPoint)
{
    // 1. Letzten Punkt in der Szene aktualisieren
    m_lastPoint = parsedPoint;

    auto activeTool = m_toolManager->activeTool();

    // 2. Falls ein aktives Tool vorhanden ist, den Punkt übergeben
    if (activeTool) {
        // Option A: Wenn dein Tool eine eigene Methode für Tastaturpunkte hat:
        activeTool->handlePointInput(this, parsedPoint);

        // Option B: Falls du das Tool über sein m_lastPoint informieren willst:
        activeTool->setLastPoint(parsedPoint);

        // Szene neu zeichnen/aktualisieren (z. B. für Vorschau-Linien)
        update();
    }
}

void CadScene::handleCommandInputValue(const double parsedValue)
{
    if (auto tool = m_toolManager->activeTool()) {
        tool->handleValueInput(this, parsedValue);
    }
}

void CadScene::cancelCurrentTool()
{
    if(m_toolManager != nullptr)
        if(m_toolManager->activeTool() != nullptr)
            m_toolManager->activeTool()->cancel(this);
}

void CadScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if(event->button() == Qt::LeftButton) {
        m_lastPoint = getSnapOrPosition(event->scenePos());

        if (auto tool = m_toolManager->activeTool()) {
            tool->mousePressEvent(this, event);
        }
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

    // Perform snapping if a document is set
    if (m_document) {
        double zoomFactor = getZoomFactorFromEvent(event);
        snap = m_snapManager.findSnapPoint(rawMousePos, *this, zoomFactor);
        updateSnapMarkers(snap, zoomFactor);
    }

    // Emit the cursor position signal with the snapped position if available
    QPointF displayPos = snap.snapped ? snap.point : rawMousePos;
    emit cursorPositionChanged(displayPos, snap.snapped, snap.type);

    // Call the active tool's mouseMoveEvent if it exists
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

void CadScene::setupSystemItems()
{
    // Create and add the crosshair item to the scene
    m_crosshair = new CrosshairItem();
    m_crosshair->setData(Qt::UserRole + 1, "SystemItem");
    m_crosshair->setZValue(10000); // Ensure the crosshair is on top of other items
    m_crosshair->setColor(Qt::white); // Set the color of the crosshair to white
    addItem(m_crosshair);

    // Create a dashed line pattern for the center lines
    if(m_dashDotDotPenRed == nullptr)
    {
        QList<qreal> pattern1;
        pattern1 << 9.0   // Strich
                 << 3.0   // Lücke
                 << 3.0   // Punkt 1
                 << 3.0   // Lücke
                 << 3.0   // Punkt 2
                 << 3.0;  // Lücke vor dem nächsten Strich
        m_dashDotDotPenRed = new QPen(Qt::red, 0);
        m_dashDotDotPenRed->setDashPattern(pattern1);
    }

    // Create a dotted line pattern for the helper lines
    if(m_dotPenRed == nullptr)
    {
        QList<qreal> pattern2;
        pattern2 << 3.0  // Strich
                 << 3.0; // Lücke
        m_dotPenRed = new QPen(Qt::red, 0);
        m_dotPenRed->setDashPattern(pattern2);
    }

    // Add the center horizontal and vertical lines to the scene
    m_centerHLine = addLine(SCENE_MIN_X, 0, SCENE_MAX_X, 0, *m_dashDotDotPenRed);
    m_centerHLine->setData(Qt::UserRole + 1, "SystemItem");
    m_centerHLine->setZValue(100);

    m_centerVLine = addLine(0, SCENE_MIN_Y, 0, SCENE_MAX_Y, *m_dashDotDotPenRed);
    m_centerVLine->setData(Qt::UserRole + 1, "SystemItem");
    m_centerVLine->setZValue(100);

    // Add the point snap marker to the scene
    m_snapMarkerPoint = new SnapMarkerPointItem();
    m_snapMarkerPoint->setData(Qt::UserRole + 1, "SystemItem");
    m_snapMarkerPoint->setZValue(1000); // over the crosshair
    m_snapMarkerPoint->setColor(Qt::yellow);
    m_snapMarkerPoint->setVisible(false); // Initially hidden
    addItem(m_snapMarkerPoint);

    // Add the endpoint snap marker to the scene
    m_snapMarkerEndpoint = new SnapMarkerEndpointItem();
    m_snapMarkerEndpoint->setData(Qt::UserRole + 1, "SystemItem");
    m_snapMarkerEndpoint->setZValue(1000); // over the crosshair
    m_snapMarkerEndpoint->setColor(Qt::red);
    m_snapMarkerEndpoint->setVisible(false);
    addItem(m_snapMarkerEndpoint);

    // Add the intersection snap marker to the scene
    m_snapMarkerIntersection = new SnapMarkerIntersectionItem();
    m_snapMarkerIntersection->setData(Qt::UserRole + 1, "SystemItem");
    m_snapMarkerIntersection->setZValue(1000); // over the crosshair
    m_snapMarkerIntersection->setColor(Qt::magenta);
    m_snapMarkerIntersection->setVisible(false);
    addItem(m_snapMarkerIntersection);

    // Add the midpoint snap marker to the scene
    m_snapMarkerMidpoint = new SnapMarkerMidpointItem();
    m_snapMarkerMidpoint->setData(Qt::UserRole + 1, "SystemItem");
    m_snapMarkerMidpoint->setZValue(1000); // over the crosshair
    m_snapMarkerMidpoint->setColor(Qt::cyan);
    m_snapMarkerMidpoint->setVisible(false);
    addItem(m_snapMarkerMidpoint);
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

void CadScene::updateSnapMarkers(const SnapResult &snap, double zoomFactor)
{
    m_hasActiveSnapPoint = snap.snapped;

    // Set the active snap point if snapping occurred
    if (snap.snapped) {
        m_activeSnapPoint = snap.point;

        setVisibleAllSnapMarker(false);

        // Marker-Größe maßstabsunabhängig auf dem Bildschirm halten (z. B. 10x10 Pixel)
        const double markerSizeWorld = m_snapMakerSize / zoomFactor;

        switch (snap.type)
        {
        case SnapType::Endpoint:
            m_snapMarkerEndpoint->setPos(snap.point);
            m_snapMarkerEndpoint->setSize(markerSizeWorld);
            m_snapMarkerEndpoint->setVisible(true);
            break;
        case SnapType::Point:
            m_snapMarkerPoint->setPos(snap.point);
            m_snapMarkerPoint->setSize(markerSizeWorld);
            m_snapMarkerPoint->setVisible(true);
            break;
        case SnapType::Midpoint:
            m_snapMarkerMidpoint->setPos(snap.point);
            m_snapMarkerMidpoint->setSize(markerSizeWorld);
            m_snapMarkerMidpoint->setVisible(true);
            break;
        case SnapType::Intersection:
            m_snapMarkerIntersection->setPos(snap.point);
            m_snapMarkerIntersection->setSize(markerSizeWorld);
            m_snapMarkerIntersection->setVisible(true);
            break;
        default:
            break;
        }
    }
    else
        setVisibleAllSnapMarker(false);
}

void CadScene::setVisibleAllSnapMarker(bool visible)
{
    m_snapMarkerPoint->setVisible(visible);
    m_snapMarkerEndpoint->setVisible(visible);
    m_snapMarkerIntersection->setVisible(visible);
    m_snapMarkerMidpoint->setVisible(visible);
}

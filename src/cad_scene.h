/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

#include "./crosshair_item.h"
#include "./cad_document/cad_document.h"
#include "./snap_manager.h"
#include "./snap_marker_point_item.h"
#include "./snap_marker_endpoint_item.h"
#include "./snap_marker_intersection_item.h"
#include "./snap_marker_midpoint_item.h"

#include <QGraphicsScene>
#include <QGraphicsRectItem>

class CadToolManager;
class QGraphicsSceneEvent;
class QGraphicsItem;

class CadScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit CadScene(CadToolManager* toolManager, QObject* parent = nullptr);
    ~CadScene();

    void loadSettings();

    void clearDocument();

    void clearDocumentItems();
    void setDocument(CadDocument* document);
    CadDocument* getDocument() const { return m_document; }
    CrosshairItem* getCrosshairItem() { return m_crosshair; }

    QPointF getSnapOrPosition(const QPointF& rawPosition) const {return m_hasActiveSnapPoint ? m_activeSnapPoint : rawPosition;}
    bool hasActiveSnapPoint() const {return m_hasActiveSnapPoint;}

    QPointF getLastPoint() const { return m_lastPoint; }
    void setLastPoint(const QPointF& pt) { m_lastPoint = pt; }
    void handleCommandInputPoint(const QPointF& parsedPoint);
    void handleCommandInputValue(const double parsedValue);

signals:
    void cursorPositionChanged(const QPointF& position, bool isSnapped, SnapType snapType);

public slots:
    void cancelCurrentTool();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void setupSystemItems();
    double getZoomFactorFromEvent(QGraphicsSceneMouseEvent* event) const;
    void updateSnapMarkers(const SnapResult& snap, double zoomFactor);
    void setVisibleAllSnapMarker(bool visible);

    CrosshairItem* m_crosshair = nullptr;
    QGraphicsLineItem* m_centerHLine = nullptr;
    QGraphicsLineItem* m_centerVLine = nullptr;

    QPen* m_dashDotDotPenRed = nullptr;
    QPen* m_dotPenRed = nullptr;

    CadToolManager* m_toolManager = nullptr;
    CadDocument* m_document = nullptr;

    // Last point when left mouse button was clicked (used for relative coordinates)
    QPointF m_lastPoint;

    // Snapping
    SnapManager m_snapManager;
    int m_snapMakerSize = 10;
    SnapMarkerPointItem* m_snapMarkerPoint = nullptr;
    SnapMarkerEndpointItem* m_snapMarkerEndpoint = nullptr;
    SnapMarkerIntersectionItem* m_snapMarkerIntersection = nullptr;
    SnapMarkerMidpointItem* m_snapMarkerMidpoint = nullptr;
    QPointF m_activeSnapPoint;
    bool m_hasActiveSnapPoint = false;
};

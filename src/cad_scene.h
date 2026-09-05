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
#include "snap_manager.h"

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

    void clearDocumentItems();
    void setDocument(CadDocument* document);
    CadDocument* getDocument() const { return m_document; }
    CrosshairItem* getCrosshairItem() { return m_crosshair; }

    QPointF getSnapOrPosition(const QPointF& rawPosition) const {return m_hasActiveSnapPoint ? m_activeSnapPoint : rawPosition;}
    bool hasActiveSnapPoint() const {return m_hasActiveSnapPoint;}

signals:
    void cursorPositionChanged(const QPointF& position, bool isSnapped, SnapType snapType);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    double getZoomFactorFromEvent(QGraphicsSceneMouseEvent* event) const;

    CrosshairItem* m_crosshair = nullptr;
    CadToolManager* m_toolManager = nullptr;
    QGraphicsLineItem* m_centerHLine = nullptr;
    QGraphicsLineItem* m_centerVLine = nullptr;

    QPen* m_dashDotDotPenRed = nullptr;
    QPen* m_dotPenRed = nullptr;

    CadDocument* m_document = nullptr;

    // Snapping
    SnapManager m_snapManager;
    int m_snapMakerSize = 10;
    QGraphicsRectItem* m_snapMarker0 = nullptr;
    QPointF m_activeSnapPoint;
    bool m_hasActiveSnapPoint = false;
};

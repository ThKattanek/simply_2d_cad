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

#include "cad_tool.h"
#include <QPointF>

class QGraphicsLineItem;
class CadEntity;

class PerpendicularConstructionLineTool : public CadTool
{
    Q_OBJECT
public:
    void retranslate() override;
    void mousePressEvent(CadScene* scene, QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(CadScene* scene, QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(CadScene* scene, QKeyEvent* event) override;
    void handlePointInput(CadScene* scene, const QPointF& point) override;

    void activate(CadScene* scene) override;
    void deactivate(CadScene* scene) override;
    void cancel(CadScene* scene) override;

private:
    enum class State {
        SelectBaseEntity,
        PositionLine
    };

    void resetForNextEntity(CadScene* scene);
    void cancelDrawing(CadScene* scene);
    void updateHoverEntity(CadScene* scene, const QPointF& mousePos);
    void clearHover();
    void updatePreview(CadScene* scene, const QPointF& mousePos);

    bool getEntityLineParams(const CadEntity* entity, QPointF& basePoint, QPointF& dirVector, QPointF& normalVector) const;

    State m_state = State::SelectBaseEntity;
    QPointF m_currentMousePos;

    // Hover-Status
    CadEntity* m_hoveredEntity = nullptr;
    QGraphicsLineItem* m_hoveredSystemLine = nullptr;

    // Ausgewähltes Basiselement
    const CadEntity* m_selectedEntity = nullptr;
    QGraphicsLineItem* m_selectedSystemLine = nullptr;

    QPointF m_basePoint;
    QPointF m_dirVector;
    QPointF m_normalVector; // Steht senkrecht auf der ausgewählten Linie

    // Live-Vorschau
    QGraphicsLineItem* m_previewItem = nullptr;

    QString m_promptMsg01, m_promptMsg02;
};
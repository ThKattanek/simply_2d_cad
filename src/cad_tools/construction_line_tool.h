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

class ConstructionLineTool : public CadTool
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
    void cancelDrawing(CadScene* scene);
    void stateMachine(CadScene* scene, const QPointF& point);
    QLineF calculateInfiniteLine(const QPointF& p1, const QPointF& p2) const;

    QGraphicsLineItem* m_tempLine = nullptr;
    ToolState m_state = ToolState::Idle;
    QPointF m_p1;
    QPointF m_currentMousePos;

    QString m_promptMsg01, m_promptMsg02;
};

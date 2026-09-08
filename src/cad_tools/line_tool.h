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

enum LineState
{
    Idle,
    Drawing
};

class LineTool : public CadTool
{
    Q_OBJECT
public:
    void retranslate() override;
    void mousePressEvent(CadScene* scene, QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(CadScene* scene, QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(CadScene* scene, QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(CadScene* scene, QKeyEvent* event) override;
    void handlePointInput(CadScene* scene, const QPointF& point) override;
    void activate(CadScene* scene) override;
    void deactivate(CadScene* scene) override;
    void cancel(CadScene* scene) override;

private:
    void cancelDrawing(CadScene* scene);
    void lineStateMachine(CadScene* scene, const QPointF& point);

    QGraphicsLineItem* m_tempLine = nullptr;
    LineState m_lineState = Idle;
    QPointF m_currentMousePos;
    QPointF m_startPoint;
    QPointF m_endPoint;

    QString promtMsg01, promtMsg02;
};

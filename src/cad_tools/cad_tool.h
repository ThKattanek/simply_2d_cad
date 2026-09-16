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

#include "../cad_scene.h"

#include <QPointF>
#include <QObject>

// Forward declarations

class QGraphicsSceneMouseEvent;
class QKeyEvent;
class CadScene;

enum ToolMode
{
    Normal = 0
};

enum ToolState
{
    Idle,
    Drawing,
    Copy
};

// Base class for CAD tools
class CadTool : public QObject
{
    Q_OBJECT
public:
    virtual ~CadTool() = default;

    virtual void retranslate() {};
    virtual void mousePressEvent(CadScene* , QGraphicsSceneMouseEvent* ) {}
    virtual void mouseMoveEvent(CadScene* , QGraphicsSceneMouseEvent* ) {}
    virtual void mouseReleaseEvent(CadScene* , QGraphicsSceneMouseEvent* ) {}
    virtual void keyPressEvent(CadScene* , QKeyEvent* ) {}

    virtual void activate(CadScene* ) {}
    virtual void deactivate(CadScene* ) {}

    virtual void cancel(CadScene* ) {}

    uint32_t getToolMode() const { return m_toolMode; }
    void setToolMode(uint32_t mode) { m_toolMode = mode; }

    QPointF getLastPoint() const { return m_lastPoint; }
    void setLastPoint(const QPointF& pt) { m_lastPoint = pt; }

    virtual void handlePointInput(CadScene* , const QPointF& ) {}
    virtual void handleValueInput(CadScene* scene, double value) { Q_UNUSED(scene); Q_UNUSED(value); }

signals:
    void promptTextChanged(const QString& text);

private:
    uint32_t m_toolMode = 0;    // Tool mode for different behaviors
    QPointF m_lastPoint;        // Stores the last point for relative coordinate input
};

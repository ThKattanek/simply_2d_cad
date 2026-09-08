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

#include <QPointF>
#include <QObject>

// Forward declarations

class QGraphicsSceneMouseEvent;
class QKeyEvent;
class CadScene;

// Base class for CAD tools

class CadTool : public QObject
{
    Q_OBJECT
public:
    virtual ~CadTool() = default;

    virtual void mousePressEvent(CadScene* , QGraphicsSceneMouseEvent* ) {}
    virtual void mouseMoveEvent(CadScene* , QGraphicsSceneMouseEvent* ) {}
    virtual void mouseReleaseEvent(CadScene* , QGraphicsSceneMouseEvent* ) {}
    virtual void keyPressEvent(CadScene* , QKeyEvent* ) {}

    virtual void activate(CadScene* ) {}
    virtual void deactivate(CadScene* ) {}

    virtual void cancel(CadScene* ) {}

    QPointF getLastPoint() const { return m_lastPoint; }
    void setLastPoint(const QPointF& pt) { m_lastPoint = pt; }

    virtual void handlePointInput(CadScene* , const QPointF& ) {}

signals:
    void promptTextChanged(const QString& text);

private:
    QPointF m_lastPoint; // Stores the last point for relative coordinate input
};

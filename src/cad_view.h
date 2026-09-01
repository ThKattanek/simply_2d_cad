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

#include <QGraphicsView>

class CadScene;

class CadView : public QGraphicsView
{
    Q_OBJECT
public:
    CadView(QGraphicsScene *scene, QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    bool m_isRightMouseZooming = false;
    QPoint m_lastMousePos;
    QPointF m_zoomAnchorScenePos;
    bool m_isMiddleMousePanning = false;
    QPoint m_panStartMousePos;
    CadScene* m_cadScene = nullptr;
};
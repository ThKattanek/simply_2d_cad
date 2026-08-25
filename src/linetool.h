#pragma once

#include "cadtool.h"
#include <QPointF>

class QGraphicsLineItem;

enum LineState
{
    Idle,
    Drawing
};

class LineTool : public CADTool
{
public:
    void mousePressEvent(CADScene* scene, QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(CADScene* scene, QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(CADScene* scene, QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(CADScene* scene, QKeyEvent* event) override;
    void deactivate(CADScene* scene) override;

private:
    void cancelDrawing(CADScene* scene);

    QGraphicsLineItem* m_tempLine = nullptr;
    LineState m_lineState = Idle;
    QPointF m_startPoint;
};

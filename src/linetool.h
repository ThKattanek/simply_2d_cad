#pragma once

#include "cadtool.h"
#include <QPointF>

class QGraphicsLineItem;

enum LineState
{
    Idle,
    Drawing
};

class LineTool : public CadTool
{
public:
    void mousePressEvent(CadScene* scene, QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(CadScene* scene, QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(CadScene* scene, QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(CadScene* scene, QKeyEvent* event) override;
    void deactivate(CadScene* scene) override;

private:
    void cancelDrawing(CadScene* scene);

    QGraphicsLineItem* m_tempLine = nullptr;
    LineState m_lineState = Idle;
    QPointF m_startPoint;
};

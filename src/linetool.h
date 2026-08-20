#pragma once

#include "cadtool.h"
#include <QPointF>

class QGraphicsLineItem;

class LineTool : public CADTool {
public:
    void mousePressEvent(CADScene* scene, QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(CADScene* scene, QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(CADScene* scene, QGraphicsSceneMouseEvent* event) override;
    void deactivate(CADScene* scene) override;

private:
    QGraphicsLineItem* m_tempLine = nullptr;
    QPointF m_startPoint;
};

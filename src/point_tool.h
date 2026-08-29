#ifndef POINT_TOOL_H
#define POINT_TOOL_H

#include "cad_tool.h"

class QGraphicsLineItem;

class PointTool : public CadTool
{
public:
    void mousePressEvent(CadScene* scene, QGraphicsSceneMouseEvent* event) override;
    void deactivate(CadScene* scene) override;
};

#endif // POINT_TOOL_H

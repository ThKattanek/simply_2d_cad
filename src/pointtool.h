#ifndef POINTTOOL_H
#define POINTTOOL_H

#include "cadtool.h"

class QGraphicsLineItem;

class PointTool : public CadTool
{
public:
    void mousePressEvent(CadScene* scene, QGraphicsSceneMouseEvent* event) override;
    void deactivate(CadScene* scene) override;
};

#endif // POINTTOOL_H

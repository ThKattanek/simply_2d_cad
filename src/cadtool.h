#pragma once

// Forward declarations
class QGraphicsSceneMouseEvent;
class CADScene;

class CADTool
{
public:
    virtual ~CADTool() = default;

    virtual void mousePressEvent(CADScene* scene, QGraphicsSceneMouseEvent* event) {}
    virtual void mouseMoveEvent(CADScene* scene, QGraphicsSceneMouseEvent* event) {}
    virtual void mouseReleaseEvent(CADScene* scene, QGraphicsSceneMouseEvent* event) {}

    virtual void activate(CADScene* scene) {}
    virtual void deactivate(CADScene* scene) {}
};

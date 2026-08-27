#pragma once

// Forward declarations
class QGraphicsSceneMouseEvent;
class QKeyEvent;
class CADScene;

class CADTool
{
public:
    virtual ~CADTool() = default;

    virtual void mousePressEvent(CADScene* , QGraphicsSceneMouseEvent* ) {}
    virtual void mouseMoveEvent(CADScene* , QGraphicsSceneMouseEvent* ) {}
    virtual void mouseReleaseEvent(CADScene* , QGraphicsSceneMouseEvent* ) {}
    virtual void keyPressEvent(CADScene* , QKeyEvent* ) {}

    virtual void activate(CADScene* ) {}
    virtual void deactivate(CADScene* ) {}
};

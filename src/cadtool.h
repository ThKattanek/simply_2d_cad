#pragma once

// Forward declarations
class QGraphicsSceneMouseEvent;
class QKeyEvent;
class CadScene;

class CadTool
{
public:
    virtual ~CadTool() = default;

    virtual void mousePressEvent(CadScene* , QGraphicsSceneMouseEvent* ) {}
    virtual void mouseMoveEvent(CadScene* , QGraphicsSceneMouseEvent* ) {}
    virtual void mouseReleaseEvent(CadScene* , QGraphicsSceneMouseEvent* ) {}
    virtual void keyPressEvent(CadScene* , QKeyEvent* ) {}

    virtual void activate(CadScene* ) {}
    virtual void deactivate(CadScene* ) {}
};

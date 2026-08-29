#include "./point_tool.h"
#include "./cad_scene.h"
#include "./cad_document/cad_point.h"

#include <QGraphicsSceneMouseEvent>

void PointTool::mousePressEvent(CadScene *scene, QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        // Add a point at the clicked position        
        auto newLine = std::make_unique<CadPoint>(event->scenePos());
        scene->getDocument()->addEntity(std::move(newLine));
    }
}

void PointTool::deactivate(CadScene *)
{
}

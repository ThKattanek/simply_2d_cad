#include "./pointtool.h"
#include "./cadscene.h"
#include "./cadpointitem.h"

#include <QGraphicsSceneMouseEvent>

#define POINT_RADIUS 5.0

void PointTool::mousePressEvent(CadScene *scene, QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        // Add a point at the clicked position
        CadPointItem* pointItem = new CadPointItem();
        pointItem->setPos(event->scenePos());
        pointItem->setSize(POINT_RADIUS);
        scene->addItem(pointItem);
    }
}

void PointTool::deactivate(CadScene *scene)
{
}

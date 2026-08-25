// CadScene.cpp (Auszug)
#include "cadscene.h"
#include "cadtoolmanager.h"
#include <qgraphicssceneevent.h>

CADScene::CADScene(CADToolManager* toolManager, QObject* parent)
    : QGraphicsScene(parent), m_toolManager(toolManager)
{

    setSceneRect(-50000, -50000, 100000, 100000); // Set a large scene rect for CAD drawing
}

void CADScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (auto tool = m_toolManager->activeTool()) {
        tool->mousePressEvent(this, event);
    }
    QGraphicsScene::mousePressEvent(event);
}

void CADScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{

    emit cursorPositionChanged(event->scenePos());

    if (auto tool = m_toolManager->activeTool()) {
        tool->mouseMoveEvent(this, event);
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void CADScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (auto tool = m_toolManager->activeTool()) {
        tool->mouseReleaseEvent(this, event);
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void CADScene::keyPressEvent(QKeyEvent *event)
{
    if (auto tool = m_toolManager->activeTool()) {
        tool->keyPressEvent(this, event);
    }
    QGraphicsScene::keyPressEvent(event);
}

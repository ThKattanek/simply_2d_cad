// CadScene.cpp (Auszug)
#include "cadscene.h"
#include "cadtoolmanager.h"

CADScene::CADScene(CADToolManager* toolManager, QObject* parent)
    : QGraphicsScene(parent), m_toolManager(toolManager) {}

void CADScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (auto tool = m_toolManager->activeTool()) {
        tool->mousePressEvent(this, event);
    }
    QGraphicsScene::mousePressEvent(event);
}

void CADScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (auto tool = m_toolManager->activeTool()) {
        tool->mouseMoveEvent(this, event);
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void CADScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (auto tool = m_toolManager->activeTool()) {
        tool->mouseReleaseEvent(this, event);
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

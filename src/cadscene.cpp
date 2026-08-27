// CadScene.cpp (Auszug)
#include "cadscene.h"
#include "cadtoolmanager.h"
#include <qgraphicsitem.h>
#include <qgraphicssceneevent.h>

#define SCENE_MIN_X -100000
#define SCENE_MAX_X 100000
#define SCENE_MIN_Y -100000
#define SCENE_MAX_Y 100000

CADScene::CADScene(CADToolManager* toolManager, QObject* parent)
    : QGraphicsScene(parent), m_toolManager(toolManager)
{
    // Set the scene rectangle to a large area to accommodate CAD drawings
    setSceneRect(SCENE_MIN_X, SCENE_MIN_Y, SCENE_MAX_X - SCENE_MIN_X, SCENE_MAX_Y - SCENE_MIN_Y);

    // Create and add the crosshair item to the scene
    m_crosshair = new CrosshairItem();
    m_crosshair->setColor(Qt::white); // Set the color of the crosshair to white
    m_crosshair->setSize(4); // Set the size of the crosshair arms to 5 units
    addItem(m_crosshair);

    // Create a dashed line pattern for the center lines
    QList<qreal> pattern1;
    pattern1 << 9.0   // Strich
            << 3.0   // Lücke
            << 3.0   // Punkt 1
            << 3.0   // Lücke
            << 3.0   // Punkt 2
            << 3.0;  // Lücke vor dem nächsten Strich
    m_dashDotDotPenRed = new QPen(Qt::red, 0);
    m_dashDotDotPenRed->setDashPattern(pattern1);

    // Create a dotted line pattern for the helper lines
    QList<qreal> pattern2;
    pattern2 << 3.0  // Strich
             << 3.0; // Lücke
    m_dotPenRed = new QPen(Qt::red, 0);
    m_dotPenRed->setDashPattern(pattern2);

    // Add the center horizontal and vertical lines to the scene
    m_centerHLine = addLine(SCENE_MIN_X, 0, SCENE_MAX_X, 0, *m_dashDotDotPenRed);
    m_centerVLine = addLine(0, SCENE_MIN_Y, 0, SCENE_MAX_Y, *m_dashDotDotPenRed);
}

CADScene::~CADScene()
{
    if(m_centerHLine != nullptr)
        delete m_centerHLine;

    if(m_centerVLine != nullptr)
        delete m_centerVLine;

    if(m_dashDotDotPenRed != nullptr)
        delete m_dashDotDotPenRed;

    if(m_dotPenRed != nullptr)
        delete m_dotPenRed;
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
    // Update the position of the crosshair item to follow the mouse cursor
    if(m_crosshair != nullptr) {
        m_crosshair->setPosition(event->scenePos());
    }

    // Update the crosshair position based on the mouse movement
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

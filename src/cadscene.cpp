// CadScene.cpp (Auszug)
#include "cadscene.h"
#include "cadtoolmanager.h"
#include <qgraphicsitem.h>
#include <qgraphicssceneevent.h>

CADScene::CADScene(CADToolManager* toolManager, QObject* parent)
    : QGraphicsScene(parent), m_toolManager(toolManager)
{

    setSceneRect(-50000, -50000, 100000, 100000); // Set a large scene rect for CAD drawing

    m_dashDotDotPenRed = new QPen(Qt::red, 0);
    m_dotPenRed = new QPen(Qt::red, 0);

    QList<qreal> pattern1;
    pattern1 << 9.0   // Strich
            << 3.0   // Lücke
            << 3.0   // Punkt 1
            << 3.0   // Lücke
            << 3.0   // Punkt 2
            << 3.0;  // Lücke vor dem nächsten Strich
    m_dashDotDotPenRed->setDashPattern(pattern1);

    QList<qreal> pattern2;
    pattern2 << 3.0  // Strich
             << 3.0; // Lücke
    m_dotPenRed->setDashPattern(pattern2);

    m_centerHLine = addLine(-5000, 0, 5000, 0, *m_dashDotDotPenRed);
    m_centerVLine = addLine(0, -5000, 0, 5000, *m_dashDotDotPenRed);
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

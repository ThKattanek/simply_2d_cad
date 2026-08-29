#include "./cad_scene.h"
#include "./cad_tool_manager.h"

#include <QGraphicsItem>
#include <QGraphicsSceneEvent>

#define SCENE_MIN_X -100000
#define SCENE_MAX_X 100000
#define SCENE_MIN_Y -100000
#define SCENE_MAX_Y 100000

CadScene::CadScene(CadToolManager* toolManager, QObject* parent)
    : QGraphicsScene(parent), m_toolManager(toolManager)
{
    // Set the scene rectangle to a large area to accommodate Cad drawings
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
    m_centerHLine->setZValue(100);

    m_centerVLine = addLine(0, SCENE_MIN_Y, 0, SCENE_MAX_Y, *m_dashDotDotPenRed);
    m_centerVLine->setZValue(100);
}

CadScene::~CadScene()
{
    if(m_centerHLine != nullptr)
        delete m_centerHLine;

    if(m_centerVLine != nullptr)
        delete m_centerVLine;

    if(m_crosshair != nullptr)
        delete m_crosshair;

    if(m_dashDotDotPenRed != nullptr)
        delete m_dashDotDotPenRed;

    if(m_dotPenRed != nullptr)
        delete m_dotPenRed;
}

void CadScene::setDocument(CadDocument *document)
{
    if(m_document == document)
        return;

    m_document = document;

    // Wenn ein Objekt zum Modell hinzugefügt wird -> Item für Grafik-Scene bauen
    connect(m_document, &CadDocument::entityAdded, this, [this](CadEntity* entity) {
        QGraphicsItem* item = entity->createGraphicsItem();
        if (item) {
            addItem(item);
        }
    });

    // Wenn ein Objekt gelöscht wird -> Item aus der Grafik-Scene entfernen
    connect(m_document, &CadDocument::entityRemoved, this, [this](CadEntity* entity) {
        if (QGraphicsItem* item = entity->getGraphicsItem()) {
            removeItem(item);
            delete item; // Löscht die visuelle Darstellung aus der Szene
        }
    });

    // Beim Leeren des Dokuments
    connect(m_document, &CadDocument::documentCleared, this, [this]() {
        clear(); // Leert die QGraphicsScene (Crosshair etc. danach neu erstellen)
    });
}

void CadScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (auto tool = m_toolManager->activeTool()) {
        tool->mousePressEvent(this, event);
    }
    QGraphicsScene::mousePressEvent(event);
}

void CadScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
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

void CadScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (auto tool = m_toolManager->activeTool()) {
        tool->mouseReleaseEvent(this, event);
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void CadScene::keyPressEvent(QKeyEvent *event)
{
    if (auto tool = m_toolManager->activeTool()) {
        tool->keyPressEvent(this, event);
    }
    QGraphicsScene::keyPressEvent(event);
}

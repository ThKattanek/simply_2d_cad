#include "cadscene.h"
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <qgraphicsview.h>

CADScene::CADScene(QObject *parent, int start_x, int start_y, int width, int height) : QGraphicsScene(parent)
{
    // Größe der Virtuelle Arbeitsfläche
    setSceneRect(start_x, start_y, width - start_y, height - start_y);

    m_penPrewiew.setCosmetic(true);
    m_penFinal.setCosmetic(true);
}

void CADScene::setMode(DrawMode mode)
{
    m_currentMode  = mode;
    m_isDrawing = false;
}

void CADScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_currentMode == DrawMode::DrawLine) {
        if (!m_isDrawing) {
            // Erster Klick: Startpunkt setzen
            m_startPoint = event->scenePos();
            m_isDrawing = true;
            m_previewLine = new QGraphicsLineItem(QLineF(m_startPoint, m_startPoint));
            m_previewLine->setPen(m_penPrewiew); // Vorschau gestrichelt
            addItem(m_previewLine);
        } else {
            // Zweiter Klick: Linie finalisieren
            QGraphicsLineItem* finalLine = new QGraphicsLineItem(QLineF(m_startPoint, event->scenePos()));
            finalLine->setPen(m_penFinal); // TruTops-Style: Weiß auf Schwarz?
            addItem(finalLine);

            // Vorschau aufräumen
            removeItem(m_previewLine);
            delete m_previewLine;
            m_previewLine = nullptr;

            // Für TruTops-Verhalten (Linienelemente verketten):
            // m_startPoint = event->scenePos(); // Neuer Startpunkt ist alter Endpunkt
            // Oder Modus beenden:
            m_isDrawing = false;
        }
    } else {
        QGraphicsScene::mousePressEvent(event);
    }
}

void CADScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_currentMode == DrawMode::DrawLine && m_isDrawing && m_previewLine) {
        m_previewLine->setLine(QLineF(m_startPoint, event->scenePos()));
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void CADScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

}

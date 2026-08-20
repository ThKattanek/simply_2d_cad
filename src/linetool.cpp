#include "linetool.h"
#include "cadscene.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QPen>

void LineTool::mousePressEvent(CADScene* scene, QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_startPoint = event->scenePos();
        m_tempLine = scene->addLine(QLineF(m_startPoint, m_startPoint), QPen(Qt::yellow, 1));
    }
}

void LineTool::mouseMoveEvent(CADScene* scene, QGraphicsSceneMouseEvent* event) {
    if (m_tempLine) {
        m_tempLine->setLine(QLineF(m_startPoint, event->scenePos()));
    }
}

void LineTool::mouseReleaseEvent(CADScene* scene, QGraphicsSceneMouseEvent* event) {
    if (m_tempLine && event->button() == Qt::LeftButton) {
        m_tempLine->setPen(QPen(Qt::white, 1));
        m_tempLine->setFlag(QGraphicsItem::ItemIsSelectable);
        m_tempLine = nullptr;
    }
}

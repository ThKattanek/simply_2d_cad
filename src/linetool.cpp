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
    qDebug() << "MousePressEvent LINE";
}

void LineTool::mouseMoveEvent(CADScene* scene, QGraphicsSceneMouseEvent* event) {
    if (m_tempLine) {
        m_tempLine->setLine(QLineF(m_startPoint, event->scenePos()));
    }
    qDebug() << "MouseMoveEvent LINE";
}

void LineTool::mouseReleaseEvent(CADScene* scene, QGraphicsSceneMouseEvent* event) {
    if (m_tempLine && event->button() == Qt::LeftButton) {
        m_tempLine->setPen(QPen(Qt::white, 1));
        m_tempLine->setFlag(QGraphicsItem::ItemIsSelectable);
        m_tempLine = nullptr;
    }
    qDebug() << "MouseReleaseEvent LINE";
}

void LineTool::deactivate(CADScene* scene) {
    if (m_tempLine) {
        scene->removeItem(m_tempLine);
        delete m_tempLine;
        m_tempLine = nullptr;
    }
    qDebug() << "Deactivate LINE";
}
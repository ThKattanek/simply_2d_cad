#include "linetool.h"
#include "cadscene.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QPen>
#include <QKeyEvent>

void LineTool::mousePressEvent(CadScene* scene, QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if(m_lineState == LineState::Idle)
        {
            m_lineState = LineState::Drawing;

            m_startPoint = event->scenePos();
            m_tempLine = scene->addLine(QLineF(m_startPoint, m_startPoint), QPen(Qt::gray, 0));

        } else if(m_lineState == LineState::Drawing)
        {
            m_lineState = LineState::Idle;
            if (m_tempLine)
            {
                m_tempLine->setPen(QPen(Qt::white, 0));
                m_tempLine = nullptr;
            }
        }
    } else if (event->button() == Qt::RightButton && m_lineState == LineState::Drawing)
    {
        // Cancel drawing
        cancelDrawing(scene);
    }
}

void LineTool::mouseMoveEvent(CadScene* , QGraphicsSceneMouseEvent* event)
{
    if (m_tempLine)
    {
        m_tempLine->setLine(QLineF(m_startPoint, event->scenePos()));
    }
}

void LineTool::mouseReleaseEvent(CadScene* , QGraphicsSceneMouseEvent* )
{
    // No action needed on mouse release for this tool
}

void LineTool::keyPressEvent(CadScene *scene, QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && m_lineState == LineState::Drawing)
        cancelDrawing(scene);
}

void LineTool::deactivate(CadScene* scene)
{
    cancelDrawing(scene);
}

void LineTool::cancelDrawing(CadScene *scene)
{
    m_lineState = LineState::Idle;
    if (m_tempLine)
    {
        scene->removeItem(m_tempLine);
        delete m_tempLine;
        m_tempLine = nullptr;
    }
}
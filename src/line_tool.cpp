/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "./line_tool.h"
#include "./cad_scene.h"
#include "./cad_document/cad_line.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QPen>
#include <QKeyEvent>

void LineTool::mousePressEvent(CadScene* scene, QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        QPointF currentPos = scene->getSnapOrPosition(event->scenePos());

        if(m_lineState == LineState::Idle)
        {
            m_lineState = LineState::Drawing;

            m_startPoint = currentPos;
            m_tempLine = scene->addLine(QLineF(m_startPoint, m_startPoint), QPen(Qt::gray, 0));

        } else if(m_lineState == LineState::Drawing)
        {
            m_lineState = LineState::Idle;
            if (m_tempLine)
            {
                m_endPoint = currentPos;

                scene->removeItem(m_tempLine);
                delete m_tempLine;
                m_tempLine = nullptr;

                auto newLine = std::make_unique<CadLine>(m_startPoint, m_endPoint);
                scene->getDocument()->addEntity(std::move(newLine));
            }
        }
    } else if (event->button() == Qt::RightButton && m_lineState == LineState::Drawing)
    {
        // Cancel drawing
        cancelDrawing(scene);
    }
}

void LineTool::mouseMoveEvent(CadScene* scene, QGraphicsSceneMouseEvent* event)
{
    if (m_tempLine)
    {
        QPointF currentPos = scene->getSnapOrPosition(event->scenePos());
        m_tempLine->setLine(QLineF(m_startPoint, currentPos));
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
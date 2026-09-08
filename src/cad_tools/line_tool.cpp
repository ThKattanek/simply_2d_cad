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
#include "../cad_document/cad_line.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QPen>
#include <QKeyEvent>

void LineTool::mousePressEvent(CadScene* scene, QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        QPointF currentPosition = scene->getSnapOrPosition(event->scenePos());
        lineStateMachine(scene, currentPosition);
    }
    else if (event->button() == Qt::RightButton && m_lineState == LineState::Drawing)
        cancelDrawing(scene);
}

void LineTool::mouseMoveEvent(CadScene* scene, QGraphicsSceneMouseEvent* event)
{
    m_currentMousePos = scene->getSnapOrPosition(event->scenePos());

    if (m_tempLine)
    {
        m_tempLine->setLine(QLineF(m_startPoint, m_currentMousePos));
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

void LineTool::handlePointInput(CadScene *scene, const QPointF &point)
{
    lineStateMachine(scene, point);
}

void LineTool::activate(CadScene *scene)
{
    Q_UNUSED(scene);
    emit promptTextChanged(tr("1. Punkt der Line: Klicken Sie auf die Startposition oder geben Sie die Koordinaten ein."));
}

void LineTool::deactivate(CadScene* scene)
{
    cancelDrawing(scene);
}

void LineTool::cancel(CadScene *scene)
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

    emit promptTextChanged(tr("1. Punkt der Line: Klicken Sie auf die Startposition oder geben Sie die Koordinaten ein."));
}

void LineTool::lineStateMachine(CadScene *scene, const QPointF &point)
{
    QPointF currentPos = scene->getSnapOrPosition(point);

    if(m_lineState == LineState::Idle)
    {
        m_lineState = LineState::Drawing;

        m_startPoint = currentPos;
        m_tempLine = scene->addLine(QLineF(m_startPoint, m_currentMousePos), QPen(Qt::gray, 0));

        emit promptTextChanged(tr("2. Punkt der Line: Klicken Sie auf die Startposition oder geben Sie die Koordinaten ein."));

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

        emit promptTextChanged(tr("1. Punkt der Line: Klicken Sie auf die Startposition oder geben Sie die Koordinaten ein."));
    }
}
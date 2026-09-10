/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "./circle_tool.h"
#include "../cad_document/cad_circle.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QPen>
#include <QKeyEvent>

void CircleTool::retranslate()
{
    promtMsg01 = tr("Circle: Click the center of the circle or enter center point (x, y).");
    promtMsg02 = tr("Circle: Click the second point or enter radius (x, y / R).");
}

void CircleTool::mousePressEvent(CadScene *scene, QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QPointF currentPosition = scene->getSnapOrPosition(event->scenePos());
        circleStateMachine(scene, currentPosition);
    }
    else if (event->button() == Qt::RightButton && m_circleState == ToolState::Drawing)
        cancelDrawing(scene);
}

void CircleTool::mouseMoveEvent(CadScene *scene, QGraphicsSceneMouseEvent *event)
{
    m_currentMousePos = scene->getSnapOrPosition(event->scenePos());

    if (m_tempCircle)
    {
        m_radius = QLineF(m_centerPoint, m_currentMousePos).length();
        m_tempCircle->setRect(m_centerPoint.x() - m_radius, m_centerPoint.y() - m_radius, 2 * m_radius, 2 * m_radius);
    }
}

void CircleTool::mouseReleaseEvent(CadScene *scene, QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(scene);
    Q_UNUSED(event);

    // No action needed on mouse release for this tool
}

void CircleTool::keyPressEvent(CadScene *scene, QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && m_circleState == ToolState::Drawing)
        cancelDrawing(scene);
}

void CircleTool::handlePointInput(CadScene *scene, const QPointF &point)
{
    circleStateMachine(scene, point);
}

void CircleTool::handleValueInput(CadScene *scene, double value)
{
    if (m_circleState == ToolState::Drawing && value > 0.0)
    {
        m_circleState = ToolState::Idle;

        if (m_tempCircle) {
            scene->removeItem(m_tempCircle);
            delete m_tempCircle;
            m_tempCircle = nullptr;
        }

        auto newCircle = std::make_unique<CadCircle>(m_centerPoint, value);
        scene->getDocument()->addEntity(std::move(newCircle));

        emit promptTextChanged(promtMsg01);
    }
}

void CircleTool::activate(CadScene *scene)
{
    Q_UNUSED(scene);
    emit promptTextChanged(promtMsg01);
}

void CircleTool::deactivate(CadScene *scene)
{
    cancelDrawing(scene);
}

void CircleTool::cancel(CadScene *scene)
{
    cancelDrawing(scene);
}

void CircleTool::cancelDrawing(CadScene *scene)
{
    m_circleState = ToolState::Idle;
    if (m_tempCircle)
    {
        scene->removeItem(m_tempCircle);
        delete m_tempCircle;
        m_tempCircle = nullptr;
    }

    emit promptTextChanged(promtMsg01);
}

void CircleTool::circleStateMachine(CadScene *scene, const QPointF &point)
{
    QPointF currentPos = scene->getSnapOrPosition(point);

    if(m_circleState == ToolState::Idle)
    {
        m_circleState = ToolState::Drawing;

        m_centerPoint = currentPos;
        m_radius = 0.0;
        m_tempCircle = scene->addEllipse(m_centerPoint.x() - m_radius, m_centerPoint.y() - m_radius, 2.0 * m_radius, 2.0 * m_radius, QPen(Qt::gray, 0));

        emit promptTextChanged(promtMsg02);

    } else if(m_circleState == ToolState::Drawing)
    {
        m_circleState = ToolState::Idle;

        if (m_tempCircle)
        {
            m_radius = QLineF(m_centerPoint, currentPos).length();

            scene->removeItem(m_tempCircle);
            delete m_tempCircle;
            m_tempCircle = nullptr;

            auto newLine = std::make_unique<CadCircle>(m_centerPoint, m_radius);
            scene->getDocument()->addEntity(std::move(newLine));
        }

        emit promptTextChanged(promtMsg01);
    }
}

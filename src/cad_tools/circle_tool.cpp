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
#include "../commands/add_entity_command.h"
#include "../undo_stack.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QPen>
#include <QKeyEvent>

void CircleTool::retranslate()
{
    promtMsg01 = tr("Circle: Click the center of the circle or enter center point (x, y).");
    promtMsg02 = tr("Circle: Click the second point or enter radius (x, y / R).");
    promtMsg03 = tr("Circle: Click on another center point or enter the center point (x, y).");
    promtMsg04 = tr("Circle: Click the second point or enter diameter (x, y / D).");
    promtMsg05 = tr("Circle: Click the first point or enter the first point (x, y).");
    promtMsg06 = tr("Circle: Click the second point or enter the second point (x, y).");
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
        if (getToolMode() == ToolMode::Normal || getToolMode() == CircleToolMode::CenterDiameter)
        {
            m_radius = QLineF(m_centerPoint, m_currentMousePos).length();
            m_tempCircle->setRect(m_centerPoint.x() - m_radius, m_centerPoint.y() - m_radius, 2 * m_radius, 2 * m_radius);
        }
        else if (getToolMode() == CircleToolMode::Diameter)
        {
            // Update the temporary circle based on the two points on the circumference
            m_radius = QLineF(m_firstPoint, m_currentMousePos).length() / 2.0;
            QPointF center = (m_firstPoint + m_currentMousePos) / 2.0;
            m_tempCircle->setRect(center.x() - m_radius, center.y() - m_radius, 2 * m_radius, 2 * m_radius);
        }
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
    switch (getToolMode())
    {
    case ToolMode::Normal:
        if (m_circleState == ToolState::Drawing && value > 0.0)
            circleStateMachine(scene, m_centerPoint + QPointF(value, 0));
        break;
    case CircleToolMode::CenterDiameter:
        if (m_circleState == ToolState::Drawing && value > 0.0)
            circleStateMachine(scene, m_centerPoint + QPointF(value / 2.0, 0));
        break;
    default:
        break;
    }
}

void CircleTool::activate(CadScene *scene)
{
    Q_UNUSED(scene);
    switch (getToolMode())
    {
    case ToolMode::Normal:
        emit promptTextChanged(promtMsg01);
        break;
    case CircleToolMode::CenterDiameter:
        emit promptTextChanged(promtMsg01);
        break;
    case CircleToolMode::Diameter:
        emit promptTextChanged(promtMsg05);
        break;
    default:
        break;
    }
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

    if(getToolMode() == ToolMode::Normal || getToolMode() == CircleToolMode::CenterDiameter)
    {
        switch(m_circleState)
        {
            case ToolState::Idle:
            m_circleState = ToolState::Drawing;

            m_centerPoint = currentPos;
            m_radius = 0.0;
            m_tempCircle = scene->addEllipse(m_centerPoint.x() - m_radius, m_centerPoint.y() - m_radius, 2.0 * m_radius, 2.0 * m_radius, QPen(Qt::gray, 0));

            if(getToolMode() == ToolMode::Normal)
                emit promptTextChanged(promtMsg02);
            else if(getToolMode() == CircleToolMode::CenterDiameter)
                emit promptTextChanged(promtMsg04);
                break;

            case Drawing:
            m_circleState = ToolState::Copy;

            if (m_tempCircle)
            {
                m_radius = QLineF(m_centerPoint, currentPos).length();

                scene->removeItem(m_tempCircle);
                delete m_tempCircle;
                m_tempCircle = nullptr;

                auto newCircle = std::make_unique<CadCircle>(m_centerPoint, m_radius);
                auto command = std::make_unique<AddEntityCommand>(scene->getDocument(), std::move(newCircle), tr("Add Circle"));

                if (scene->getUndoStack()) {
                    scene->getUndoStack()->push(std::move(command));
                } else {
                    command->execute();
                }
            }
            emit promptTextChanged(promtMsg03);
            break;

            case Copy:
            m_centerPoint = currentPos;

            auto newCircle = std::make_unique<CadCircle>(m_centerPoint, m_radius);
            auto command = std::make_unique<AddEntityCommand>(scene->getDocument(), std::move(newCircle), tr("Add Circle"));

            if (scene->getUndoStack()) {
                scene->getUndoStack()->push(std::move(command));
            } else {
                command->execute();
            }

            break;
        }
    }
    else if(getToolMode() == CircleToolMode::Diameter)
    {
        // Drawing the circle over 2 points on the circle none center point
        switch (m_circleState)
        {
        case ToolState::Idle:
            m_circleState = ToolState::Drawing;
            m_firstPoint = currentPos;
            m_tempCircle = scene->addEllipse(m_firstPoint.x(), m_firstPoint.y(), 0, 0, QPen(Qt::gray, 0));

            emit promptTextChanged(promtMsg06);
            break;

        case ToolState::Drawing:
            m_circleState = ToolState::Idle;

            if (m_tempCircle)
            {
                m_radius = QLineF(m_firstPoint, point).length() / 2.0;
                QPointF center = (m_firstPoint + point) / 2.0;

                scene->removeItem(m_tempCircle);
                delete m_tempCircle;
                m_tempCircle = nullptr;

                auto newCircle = std::make_unique<CadCircle>(center, m_radius);
                auto command = std::make_unique<AddEntityCommand>(scene->getDocument(), std::move(newCircle), tr("Add Circle"));

                if (scene->getUndoStack()) {
                    scene->getUndoStack()->push(std::move(command));
                } else {
                    command->execute();
                }
            }
            emit promptTextChanged(promtMsg05);
            break;

        default:
            break;
        }
    }
}

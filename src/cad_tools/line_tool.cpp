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
#include "../commands/add_entity_command.h"
#include "../undo_stack.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QPen>
#include <QKeyEvent>

void LineTool::retranslate()
{
    promtMsg01 = tr("Line: Click the first point on the line or enter the coordinates (x, y).");
    promtMsg02 = tr("Line: Click the second point on the line or enter the coordinates (x, y).");

    promtMsg03 = tr("HLine: Click the first point on the line or enter the coordinates (x, y).");
    promtMsg04 = tr("HLine: Click the second point on the line, or enter the coordinates (x, y) or the length.");

    promtMsg05 = tr("VLine: Click the first point on the line or enter the coordinates (x, y).");
    promtMsg06 = tr("VLine: Click the second point on the line, or enter the coordinates (x, y) or the length.");
}

void LineTool::mousePressEvent(CadScene* scene, QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        QPointF currentPosition = scene->getSnapOrPosition(event->scenePos());
        lineStateMachine(scene, currentPosition);
    }
    else if (event->button() == Qt::RightButton && m_lineState == ToolState::Drawing)
        cancelDrawing(scene);
}

void LineTool::mouseMoveEvent(CadScene* scene, QGraphicsSceneMouseEvent* event)
{
    m_currentMousePos = scene->getSnapOrPosition(event->scenePos());

    if (m_tempLine)
    {
        switch(getToolMode())
        {
        case LineToolMode::Horizontal:
            m_currentMousePos.setY(m_startPoint.y());
            break;
        case LineToolMode::Vertical:
            m_currentMousePos.setX(m_startPoint.x());
            break;
        default:
            break;
        }

        m_tempLine->setLine(QLineF(m_startPoint, m_currentMousePos));
    }
}

void LineTool::mouseReleaseEvent(CadScene* , QGraphicsSceneMouseEvent* )
{
    // No action needed on mouse release for this tool
}

void LineTool::keyPressEvent(CadScene *scene, QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && m_lineState == ToolState::Drawing)
        cancelDrawing(scene);
}

void LineTool::handlePointInput(CadScene *scene, const QPointF &point)
{
    lineStateMachine(scene, point);
}

void LineTool::handleValueInput(CadScene *scene, double value)
{
    QPointF endPoint;

    if (m_lineState == ToolState::Drawing)
    {
        if(getToolMode() == LineToolMode::Horizontal)
            endPoint = QPointF(m_startPoint.x() + value, m_startPoint.y());
        else if(getToolMode() == LineToolMode::Vertical)
            endPoint = QPointF(m_startPoint.x(), m_startPoint.y() + value);
    }

    lineStateMachine(scene, endPoint);
}

void LineTool::activate(CadScene *scene)
{
    Q_UNUSED(scene);

    switch(getToolMode())
    {
    case LineToolMode::Horizontal:
        emit promptTextChanged(promtMsg03);
        break;
    case LineToolMode::Vertical:
        emit promptTextChanged(promtMsg05);
        break;
    default:
        emit promptTextChanged(promtMsg01);
    }
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
    m_lineState = ToolState::Idle;
    if (m_tempLine)
    {
        scene->removeItem(m_tempLine);
        delete m_tempLine;
        m_tempLine = nullptr;
    }

    emit promptTextChanged(promtMsg01);
}

void LineTool::lineStateMachine(CadScene *scene, const QPointF &point)
{
    QPointF currentPos = scene->getSnapOrPosition(point);

    if(m_lineState == ToolState::Idle)
    {
        m_lineState = ToolState::Drawing;

        m_startPoint = currentPos;

        switch(getToolMode())
        {
            case LineToolMode::Horizontal:
                m_currentMousePos.setY(m_startPoint.y());
                break;
            case LineToolMode::Vertical:
                m_currentMousePos.setX(m_startPoint.x());
                break;
            default:
                m_currentMousePos = m_startPoint;
                break;
        }

        m_tempLine = scene->addLine(QLineF(m_startPoint, m_currentMousePos), QPen(Qt::gray, 0));

        switch(getToolMode())
        {
            case LineToolMode::Horizontal:
                emit promptTextChanged(promtMsg04);
                break;
            case LineToolMode::Vertical:
                emit promptTextChanged(promtMsg06);
                break;
            default:
                emit promptTextChanged(promtMsg02);
        }

    } else if(m_lineState == ToolState::Drawing)
    {
        m_lineState = ToolState::Idle;

        if (m_tempLine)
        {
            m_endPoint = currentPos;

            switch(getToolMode())
            {
                case LineToolMode::Horizontal:
                    m_endPoint.setY(m_startPoint.y());
                    break;
                case LineToolMode::Vertical:
                    m_endPoint.setX(m_startPoint.x());
                    break;
                default:
                    break;
            }

            scene->removeItem(m_tempLine);
            delete m_tempLine;
            m_tempLine = nullptr;

            auto newLine = std::make_unique<CadLine>(m_startPoint, m_endPoint);
            auto command = std::make_unique<AddEntityCommand>(scene->getDocument(), std::move(newLine), tr("Add Line"));

            if (scene->getUndoStack()) {
                scene->getUndoStack()->push(std::move(command));
            } else {
                command->execute();
            }
        }

        switch(getToolMode())
        {
        case LineToolMode::Horizontal:
            emit promptTextChanged(promtMsg03);
            break;
        case LineToolMode::Vertical:
            emit promptTextChanged(promtMsg05);
            break;
        default:
            emit promptTextChanged(promtMsg01);
        }
    }
}
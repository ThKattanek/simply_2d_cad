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

    // NEU: Prompt-Meldungen für den 3-Punkte-Kreis
    promtMsg07 = tr("Circle 3 Points: Click the first point on the circle (x, y).");
    promtMsg08 = tr("Circle 3 Points: Click the second point on the circle (x, y).");
    promtMsg09 = tr("Circle 3 Points: Click the third point on the circle (x, y).");
}

void CircleTool::mousePressEvent(CadScene *scene, QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QPointF currentPosition = scene->getSnapOrPosition(event->scenePos());
        circleStateMachine(scene, currentPosition);
    }
}

void CircleTool::mouseMoveEvent(CadScene *scene, QGraphicsSceneMouseEvent *event)
{
    m_currentMousePos = scene->getSnapOrPosition(event->scenePos());

    if (m_tempCircle)
    {
        QPointF center;
        qreal radius;
        QPointF tempCenter;

        switch(getToolMode()) {
        case ToolMode::Normal:
        case CircleToolMode::CenterDiameter:
            m_radius = QLineF(m_centerPoint, m_currentMousePos).length();
            m_tempCircle->setRect(m_centerPoint.x() - m_radius, m_centerPoint.y() - m_radius, 2 * m_radius, 2 * m_radius);
            break;

        case CircleToolMode::Diameter:
            m_radius = QLineF(m_firstPoint, m_currentMousePos).length() / 2.0;
            center = (m_firstPoint + m_currentMousePos) / 2.0;
            m_tempCircle->setRect(center.x() - m_radius, center.y() - m_radius, 2 * m_radius, 2 * m_radius);
            break;

        case CircleToolMode::ThreePoints:
            if (m_pointCount == 1) {
                // Punkt 1 liegt auf dem Umfang, die Maus bildet vorübergehend den gegenüberliegenden Punkt
                m_radius = QLineF(m_p1, m_currentMousePos).length() / 2.0;
                tempCenter = (m_p1 + m_currentMousePos) / 2.0;

                m_tempCircle->setRect(tempCenter.x() - m_radius, tempCenter.y() - m_radius, 2.0 * m_radius, 2.0 * m_radius);
                m_tempCircle->setVisible(true);
            }
            else if (m_pointCount == 2) {
                // Echter 3-Punkte-Kreis über P1, P2 und m_currentMousePos
                if (calculateCircle3P(m_p1, m_p2, m_currentMousePos, center, radius)) {
                    m_tempCircle->setRect(center.x() - radius, center.y() - radius, 2.0 * radius, 2.0 * radius);
                    m_tempCircle->setVisible(true);
                } else {
                    m_tempCircle->setVisible(false); // Kollinear
                }
            }
            break;
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
    switch (getToolMode()) {
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

    m_circleState = ToolState::Idle;
    m_pointCount = 0;

    if (m_tempCircle) {
        scene->removeItem(m_tempCircle);
        delete m_tempCircle;
        m_tempCircle = nullptr;
    }

    switch (getToolMode()) {
    case ToolMode::Normal:
        emit promptTextChanged(promtMsg01);
        break;
    case CircleToolMode::CenterDiameter:
        emit promptTextChanged(promtMsg01);
        break;
    case CircleToolMode::Diameter:
        emit promptTextChanged(promtMsg05);
        break;
    case CircleToolMode::ThreePoints:
        emit promptTextChanged(promtMsg07);
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
    activate(scene);
}

void CircleTool::circleStateMachine(CadScene *scene, const QPointF &point)
{
    QPointF currentPos = scene->getSnapOrPosition(point);

    switch(getToolMode()) {
    case ToolMode::Normal:
    case CircleToolMode::CenterDiameter:
        switch(m_circleState) {
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
        break;

    case CircleToolMode::Diameter:
        // Drawing the circle over 2 points on the circle none center point
        switch (m_circleState) {
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
        break;

    case CircleToolMode::ThreePoints:
        switch (m_pointCount)
        {
        case 0: // Ersten Punkt gesetzt
            m_p1 = currentPos;
            m_pointCount = 1;
            m_circleState = ToolState::Drawing;
            scene->setLastPoint(m_p1);

            m_tempCircle = scene->addEllipse(m_p1.x(), m_p1.y(), 0, 0, QPen(Qt::gray, 0));
            emit promptTextChanged(promtMsg08);
            break;

        case 1: // Zweiten Punkt gesetzt
            m_p2 = currentPos;
            m_pointCount = 2;
            scene->setLastPoint(m_p2);

            emit promptTextChanged(promtMsg09);
            break;

        case 2: // Dritten Punkt gesetzt & Kreis erzeugen
            QPointF center;
            qreal radius;

            if (calculateCircle3P(m_p1, m_p2, currentPos, center, radius)) {
                auto newCircle = std::make_unique<CadCircle>(center, radius);
                auto command = std::make_unique<AddEntityCommand>(
                    scene->getDocument(), std::move(newCircle), tr("Add 3-Point Circle"));

                if (scene->getUndoStack()) {
                    scene->getUndoStack()->push(std::move(command));
                } else {
                    command->execute();
                }
            }

            // Aufräumen & Neustart des Tools
            cancelDrawing(scene);
            break;
        }
        break;
    }
}

bool CircleTool::calculateCircle3P(const QPointF &p1, const QPointF &p2, const QPointF &p3, QPointF &center, qreal &radius)
{
    double D = 2.0 * (p1.x() * (p2.y() - p3.y()) + p2.x() * (p3.y() - p1.y()) + p3.x() * (p1.y() - p2.y()));

    if (std::abs(D) < 1e-9) {
        return false; // Kollinear, kein eindeutiger Kreis
    }

    double sq1 = p1.x() * p1.x() + p1.y() * p1.y();
    double sq2 = p2.x() * p2.x() + p2.y() * p2.y();
    double sq3 = p3.x() * p3.x() + p3.y() * p3.y();

    double cx = (sq1 * (p2.y() - p3.y()) + sq2 * (p3.y() - p1.y()) + sq3 * (p1.y() - p2.y())) / D;
    double cy = (sq1 * (p3.x() - p2.x()) + sq2 * (p1.x() - p3.x()) + sq3 * (p2.x() - p1.x())) / D;

    center = QPointF(cx, cy);
    radius = std::hypot(p1.x() - cx, p1.y() - cy);

    return true;
}

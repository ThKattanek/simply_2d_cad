#include "rectangle_tool.h"
#include "../cad_document/cad_line.h"
#include "../commands/add_entity_command.h"
#include "../undo_stack.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include <QPen>
#include <QKeyEvent>

void RectangleTool::retranslate()
{
    promptMsg01 = tr("Rectangle: Click first corner point or enter coordinates (x, y).");
    promptMsg02 = tr("Rectangle: Click opposite corner point or enter coordinates (x, y).");
}

void RectangleTool::mousePressEvent(CadScene* scene, QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        QPointF currentPosition = scene->getSnapOrPosition(event->scenePos());
        rectangleStateMachine(scene, currentPosition);
    }
    else if (event->button() == Qt::RightButton && m_rectState == ToolState::Drawing)
    {
        cancelDrawing(scene);
    }
}

void RectangleTool::mouseMoveEvent(CadScene* scene, QGraphicsSceneMouseEvent* event)
{
    m_currentMousePos = scene->getSnapOrPosition(event->scenePos());

    if (m_tempRect)
    {
        QRectF rect = QRectF(m_firstCorner, m_currentMousePos).normalized();
        m_tempRect->setRect(rect);
    }
}

void RectangleTool::keyPressEvent(CadScene* scene, QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape && m_rectState == ToolState::Drawing)
        cancelDrawing(scene);
}

void RectangleTool::handlePointInput(CadScene* scene, const QPointF& point)
{
    rectangleStateMachine(scene, point);
}

void RectangleTool::activate(CadScene* scene)
{
    Q_UNUSED(scene);
    emit promptTextChanged(promptMsg01);
}

void RectangleTool::deactivate(CadScene* scene)
{
    cancelDrawing(scene);
}

void RectangleTool::cancel(CadScene* scene)
{
    cancelDrawing(scene);
}

void RectangleTool::cancelDrawing(CadScene* scene)
{
    m_rectState = ToolState::Idle;
    if (m_tempRect)
    {
        scene->removeItem(m_tempRect);
        delete m_tempRect;
        m_tempRect = nullptr;
    }

    emit promptTextChanged(promptMsg01);
}

void RectangleTool::rectangleStateMachine(CadScene* scene, const QPointF& point)
{
    QPointF currentPos = scene->getSnapOrPosition(point);

    if (m_rectState == ToolState::Idle)
    {
        m_rectState = ToolState::Drawing;
        m_firstCorner = currentPos;
        scene->setLastPoint(m_firstCorner);

        // Vorschau-Rechteck anlegen
        m_tempRect = scene->addRect(QRectF(m_firstCorner, m_firstCorner), QPen(Qt::gray, 0));

        emit promptTextChanged(promptMsg02);
    }
    else if (m_rectState == ToolState::Drawing)
    {
        m_rectState = ToolState::Idle;

        if (m_tempRect)
        {
            scene->removeItem(m_tempRect);
            delete m_tempRect;
            m_tempRect = nullptr;

            QPointF p1 = m_firstCorner;
            QPointF p2 = currentPos;

            // Die 4 Eckpunkte des Rechtecks
            QPointF corner1(p1.x(), p1.y());
            QPointF corner2(p2.x(), p1.y());
            QPointF corner3(p2.x(), p2.y());
            QPointF corner4(p1.x(), p2.y());

            // 4 einzelne CadLines zum Dokument hinzufügen
            auto addLine = [&](const QPointF& start, const QPointF& end) {
                auto line = std::make_unique<CadLine>(start, end);
                auto cmd = std::make_unique<AddEntityCommand>(scene->getDocument(), std::move(line), tr("Add Rectangle Line"));
                if (scene->getUndoStack()) {
                    scene->getUndoStack()->push(std::move(cmd));
                } else {
                    cmd->execute();
                }
            };

            addLine(corner1, corner2);
            addLine(corner2, corner3);
            addLine(corner3, corner4);
            addLine(corner4, corner1);
        }

        emit promptTextChanged(promptMsg01);
    }
}
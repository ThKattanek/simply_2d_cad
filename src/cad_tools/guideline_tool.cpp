#include "guideline_tool.h"
#include "../cad_document/cad_line.h"
#include "../commands/add_entity_command.h"
#include "../undo_stack.h"

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QRectF>

void GuidelineTool::retranslate()
{
    promtMsgH = tr("Guideline H: Enter Y coordinate or click to place a horizontal guideline.");
    promtMsgV = tr("Guideline V: Enter X coordinate or click to place a vertical guideline.");
}

void GuidelineTool::mousePressEvent(CadScene* scene, QGraphicsSceneMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return;

    QPointF pos = scene->getSnapOrPosition(event->scenePos());

    QRectF rect = scene->sceneRect();
    QPointF startPt, endPt;

    if (getToolMode() == GuidelineHorizontal) {
        double y = pos.y();
        startPt = QPointF(rect.left(), y);
        endPt   = QPointF(rect.right(), y);
    } else {
        double x = pos.x();
        startPt = QPointF(x, rect.top());
        endPt   = QPointF(x, rect.bottom());
    }

    auto newLine = std::make_unique<CadLine>(startPt, endPt);
    auto command = std::make_unique<AddEntityCommand>(scene->getDocument(), std::move(newLine), tr("Add Guideline"));

    if (scene->getUndoStack()) {
        scene->getUndoStack()->push(std::move(command));
    } else {
        command->execute();
    }

    // keep tool active and update prompt
    if (getToolMode() == GuidelineHorizontal)
        emit promptTextChanged(promtMsgH);
    else
        emit promptTextChanged(promtMsgV);
}

void GuidelineTool::keyPressEvent(CadScene* scene, QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        cancel(scene);
    }
}

void GuidelineTool::handleValueInput(CadScene* scene, double value)
{
    QRectF rect = scene->sceneRect();
    QPointF startPt, endPt;

    if (getToolMode() == GuidelineHorizontal) {
        double y = value;
        startPt = QPointF(rect.left(), y);
        endPt   = QPointF(rect.right(), y);
    } else {
        double x = value;
        startPt = QPointF(x, rect.top());
        endPt   = QPointF(x, rect.bottom());
    }

    auto newLine = std::make_unique<CadLine>(startPt, endPt);
    auto command = std::make_unique<AddEntityCommand>(scene->getDocument(), std::move(newLine), tr("Add Guideline"));

    if (scene->getUndoStack()) {
        scene->getUndoStack()->push(std::move(command));
    } else {
        command->execute();
    }

    // update prompt text
    if (getToolMode() == GuidelineHorizontal)
        emit promptTextChanged(promtMsgH);
    else
        emit promptTextChanged(promtMsgV);
}

void GuidelineTool::activate(CadScene* /*scene*/)
{
    retranslate();
    if (getToolMode() == GuidelineHorizontal)
        emit promptTextChanged(promtMsgH);
    else
        emit promptTextChanged(promtMsgV);
}

void GuidelineTool::deactivate(CadScene* /*scene*/)
{
    // nothing to clean up
}

void GuidelineTool::cancel(CadScene* /*scene*/)
{
    // nothing to cancel for this one-shot tool
}

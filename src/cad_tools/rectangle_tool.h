#pragma once

#include "cad_tool.h"
#include <QPointF>

class QGraphicsRectItem;

class RectangleTool : public CadTool
{
    Q_OBJECT
public:
    void retranslate() override;
    void mousePressEvent(CadScene* scene, QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(CadScene* scene, QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(CadScene* scene, QKeyEvent* event) override;
    void handlePointInput(CadScene* scene, const QPointF& point) override;
    void activate(CadScene* scene) override;
    void deactivate(CadScene* scene) override;
    void cancel(CadScene* scene) override;

private:
    void cancelDrawing(CadScene* scene);
    void rectangleStateMachine(CadScene* scene, const QPointF& point);

    QGraphicsRectItem* m_tempRect = nullptr;
    ToolState m_rectState = ToolState::Idle;
    QPointF m_firstCorner;
    QPointF m_currentMousePos;

    QString promptMsg01, promptMsg02;
};

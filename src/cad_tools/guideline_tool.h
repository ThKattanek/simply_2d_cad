#pragma once

#include "cad_tool.h"

#include <QPointF>

enum GuidelineToolMode {
    GuidelineHorizontal,
    GuidelineVertical
};

class GuidelineTool : public CadTool
{
    Q_OBJECT
public:
    GuidelineTool(GuidelineToolMode mode = GuidelineHorizontal) { setToolMode(mode); }

    void retranslate() override;
    void mousePressEvent(CadScene* scene, QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(CadScene* scene, QKeyEvent* event) override;
    void handleValueInput(CadScene* scene, double value) override;
    void activate(CadScene* scene) override;
    void deactivate(CadScene* scene) override;
    void cancel(CadScene* scene) override;

private:
    GuidelineToolMode m_mode = GuidelineHorizontal;
    QString promtMsgH, promtMsgV;
};

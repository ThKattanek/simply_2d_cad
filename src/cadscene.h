#pragma once

#include "crosshairitem.h"
#include <QGraphicsScene>

class CADToolManager;
class QGraphicsSceneEvent;
class QGraphicsItem;

class CADScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit CADScene(CADToolManager* toolManager, QObject* parent = nullptr);
    ~CADScene();

signals:
    void cursorPositionChanged(const QPointF& position);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    CrosshairItem* m_crosshair = nullptr;
    CADToolManager* m_toolManager = nullptr;
    QGraphicsLineItem* m_centerHLine = nullptr;
    QGraphicsLineItem* m_centerVLine = nullptr;

    QPen* m_dashDotDotPenRed = nullptr;
    QPen* m_dotPenRed = nullptr;
};

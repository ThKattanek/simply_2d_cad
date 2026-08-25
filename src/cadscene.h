#pragma once

#include <QGraphicsScene>

class CADToolManager;
class QGraphicsSceneEvent;

class CADScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit CADScene(CADToolManager* toolManager, QObject* parent = nullptr);

signals:
    void cursorPositionChanged(const QPointF& position);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    CADToolManager* m_toolManager;
};

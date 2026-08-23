#pragma once

#include <QGraphicsScene>

class CADToolManager;

class CADScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit CADScene(CADToolManager* toolManager, QObject* parent = nullptr);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    CADToolManager* m_toolManager;
};

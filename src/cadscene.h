#ifndef CADSCENE_H
#define CADSCENE_H

#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <qevent.h>

enum class DrawMode {
    Select,
    DrawLine
};

class CADScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit CADScene(QObject *parent = nullptr, int start_x = 0, int start_y = 0, int width = 1000, int height = 1000);
    void setMode(DrawMode mode);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
private:

    QPen m_penPrewiew = QPen(Qt::gray, 1);
    QPen m_penFinal = QPen(Qt::white, 1);

    DrawMode m_currentMode = DrawMode::Select;
    bool m_isDrawing = false;
    QPointF m_startPoint;
    QGraphicsLineItem* m_previewLine = nullptr;
};

#endif // CADSCENE_H

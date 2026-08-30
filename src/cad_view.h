#pragma once

#include <QGraphicsView>

class CadView : public QGraphicsView
{
    Q_OBJECT
public:
    CadView(QGraphicsScene *scene, QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    bool m_isRightMouseZooming = false;
    QPoint m_lastMousePos;
    QPointF m_zoomAnchorScenePos;
};
#pragma once

#include <QGraphicsView>

class CADView : public QGraphicsView
{
    Q_OBJECT
public:
    CADView(QGraphicsScene *scene, QWidget *parent = nullptr);
protected:
    void showEvent(QShowEvent *event) override;
};
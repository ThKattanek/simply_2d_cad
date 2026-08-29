#pragma once

#include <QGraphicsView>

class CadView : public QGraphicsView
{
    Q_OBJECT
public:
    CadView(QGraphicsScene *scene, QWidget *parent = nullptr);
protected:
    void showEvent(QShowEvent *event) override;
};
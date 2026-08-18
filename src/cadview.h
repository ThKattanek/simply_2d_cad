#ifndef CADVIEW_H
#define CADVIEW_H

#include <QGraphicsView>

class CADView : public QGraphicsView
{
    Q_OBJECT
public:
    CADView(QGraphicsScene *scene, QWidget *parent = nullptr);
protected:
    void showEvent(QShowEvent *event) override;
};

#endif // CADVIEW_H

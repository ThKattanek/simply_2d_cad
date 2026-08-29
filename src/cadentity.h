#pragma once

#include <QGraphicsItem>
#include <QString>

// Base class for CAD entities

class CadEntity
{
public:
    virtual ~CadEntity() = default;

    virtual QGraphicsItem* createGraphicsItem() = 0;
    virtual void updateGraphicsItem(QGraphicsItem* item) = 0;

    QGraphicsItem* getGraphicsItem() const { return m_graphicsItem; }
    void setGraphicsItem(QGraphicsItem* item) { m_graphicsItem = item; }

    QString getLayer() const { return m_layer; }
    void setLayer(const QString& layer) { m_layer = layer; }

protected:
    QGraphicsItem* m_graphicsItem = nullptr;
    QString m_layer = "0";
};

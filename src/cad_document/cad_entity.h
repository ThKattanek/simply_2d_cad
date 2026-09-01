/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

#include <QGraphicsItem>
#include <QString>
#include <QDataStream>

// Base class for CAD entities

enum EntityType {
    Line,
    Point
};

class CadEntity
{
public:
    virtual ~CadEntity() = default;
    virtual EntityType type() const = 0;

    virtual void serialize(QDataStream& out_stream) const = 0;
    virtual void deserialize(QDataStream& in_stream) = 0;

    virtual QGraphicsItem* createGraphicsItem() = 0;
    virtual void updateGraphicsItem() = 0;

    QGraphicsItem* getGraphicsItem() const { return m_graphicsItem; }
    void setGraphicsItem(QGraphicsItem* item) { m_graphicsItem = item; }

    QString getLayer() const { return m_layer; }
    void setLayer(const QString& layer) { m_layer = layer; }

protected:
    QGraphicsItem* m_graphicsItem = nullptr;
    QString m_layer = "0";
};

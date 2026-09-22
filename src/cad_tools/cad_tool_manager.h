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

#include <QObject>
#include <QMap>
#include <memory>

class QAction;
class QActionGroup;
class CadScene;

#include "cad_tool.h"

class CadToolManager : public QObject
{
    Q_OBJECT
public:
    explicit CadToolManager(QObject* parent = nullptr);

    //
    void retranslateAllTools();

    // Set the scene for tools to interact with
    void setScene(CadScene* scene) { m_scene = scene; }

    // Registers a new tool with the manager
    // tool_mode can be used to specify different modes for the same tool, if needed
    void registerTool(const QString& actionName, std::shared_ptr<CadTool> tool, uint32_t tool_mode = 0);

    // Link a QAction directly from the UI
    void bindAction(QAction* action);

    // Set the active tool by name
    CadTool* activeTool() const {return m_activeTool.get();}

signals:
    void toolChanged(CadTool* newTool);
    void promptTextChanged(const QString& text);

private:
    QMap<QString, std::shared_ptr<CadTool>> m_tools;
    CadScene* m_scene = nullptr;
    std::shared_ptr<CadTool> m_activeTool;
    QActionGroup* m_actionGroup = nullptr;
};
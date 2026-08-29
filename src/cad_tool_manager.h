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

    // Set the scene for tools to interact with
    void setScene(CadScene* scene) { m_scene = scene; }

    // Registers a new tool with the manager
    void registerTool(const QString& actionName, std::shared_ptr<CadTool> tool);

    // Link a QAction directly from the UI
    void bindAction(QAction* action);

    // Set the active tool by name
    CadTool* activeTool() const {return m_activeTool.get();}

signals:
    void toolChanged(CadTool* newTool);

private:
    QMap<QString, std::shared_ptr<CadTool>> m_tools;
    CadScene* m_scene = nullptr;
    std::shared_ptr<CadTool> m_activeTool;
    QActionGroup* m_actionGroup = nullptr;
};
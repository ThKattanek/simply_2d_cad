#pragma once

#include <QObject>
#include <QMap>
#include <memory>

class QAction;
class QActionGroup;

#include "cadtool.h"

class CADToolManager : public QObject {
    Q_OBJECT
public:
    explicit CADToolManager(QObject* parent = nullptr);

    // Registers a new tool with the manager
    void registerTool(const QString& actionName, std::shared_ptr<CADTool> tool);

    // Link a QAction directly from the UI
    void bindAction(QAction* action);

    // Set the active tool by name
    CADTool* activeTool() const {return m_activeTool.get();}

signals:
    void toolChanged(CADTool* newTool);

private:
    QMap<QString, std::shared_ptr<CADTool>> m_tools;
    std::shared_ptr<CADTool> m_activeTool;
    QActionGroup* m_actionGroup = nullptr;
};
#include "./cad_tool_manager.h"
#include "./cad_scene.h"
#include <QAction>
#include <QActionGroup>

CadToolManager::CadToolManager(QObject* parent) : QObject(parent)
{
    m_actionGroup = new QActionGroup(this);
    m_actionGroup->setExclusive(true);
}

void CadToolManager::registerTool(const QString& actionName, std::shared_ptr<CadTool> tool)
{
    m_tools[actionName] = tool;
}

void CadToolManager::bindAction(QAction* action)
{
    action->setCheckable(true);
    m_actionGroup->addAction(action);

    connect(action, &QAction::triggered, this, [this, action]() {
        QString name = action->objectName();
        if (m_tools.contains(name)) {
            auto newTool = m_tools[name];

            if(m_activeTool != newTool) {
                if (m_activeTool) {
                    m_activeTool->deactivate(m_scene);
                }
            }

            m_activeTool = newTool;

            if(m_activeTool) {
                m_activeTool->activate(m_scene);
            }

            emit toolChanged(m_activeTool.get());
        }
    });
}
#include "./cadtoolmanager.h"
#include "./cadscene.h"
#include <QAction>
#include <QActionGroup>

CADToolManager::CADToolManager(QObject* parent) : QObject(parent)
{
    m_actionGroup = new QActionGroup(this);
    m_actionGroup->setExclusive(true);
}

void CADToolManager::registerTool(const QString& actionName, std::shared_ptr<CADTool> tool)
{
    m_tools[actionName] = tool;
}

void CADToolManager::bindAction(QAction* action)
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
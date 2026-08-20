#include "cadtoolmanager.h"
#include <QAction>
#include <QActionGroup>

CADToolManager::CADToolManager(QObject* parent) : QObject(parent) {
    m_actionGroup = new QActionGroup(this);
    m_actionGroup->setExclusive(true);
}

void CADToolManager::registerTool(const QString& actionName, std::shared_ptr<CADTool> tool) {
    m_tools[actionName] = tool;
}

void CADToolManager::bindAction(QAction* action) {
    action->setCheckable(true);
    m_actionGroup->addAction(action);

    connect(action, &QAction::triggered, this, [this, action]() {
        QString name = action->objectName();
        if (m_tools.contains(name)) {
            m_activeTool = m_tools[name];
            emit toolChanged(m_activeTool.get());
        }
    });
}
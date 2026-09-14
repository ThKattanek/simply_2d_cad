#pragma once

#include "./command.h"
#include <vector>
#include <memory>

class MacroCommand : public Command
{
public:
    explicit MacroCommand(QString description)
        : m_description(std::move(description)) {}

    void addCommand(CommandPtr command) {
        if (command) {
            m_commands.push_back(std::move(command));
        }
    }

    void execute() override {
        for (auto& cmd : m_commands) {
            cmd->execute();
        }
    }

    void undo() override {
        // Rückwärts durchgehen, damit die Reihenfolge beim Undo stimmt
        for (auto it = m_commands.rbegin(); it != m_commands.rend(); ++it) {
            (*it)->undo();
        }
    }

    void redo() override {
        for (auto& cmd : m_commands) {
            cmd->redo();
        }
    }

    QString description() const override {
        return m_description;
    }

private:
    std::vector<CommandPtr> m_commands;
    QString m_description;
};

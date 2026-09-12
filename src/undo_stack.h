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

#include "./commands/command.h"

#include <QObject>
#include <vector>

class UndoStack : public QObject
{
    Q_OBJECT
public:
    explicit UndoStack(QObject* parent = nullptr);

    void push(CommandPtr command);
    bool canUndo() const;
    bool canRedo() const;
    QString undoText() const;
    QString redoText() const;
    void clear();

public slots:
    void undo();
    void redo();

signals:
    void stackChanged();
    void canUndoChanged(bool canUndo);
    void canRedoChanged(bool canRedo);

private:
    void emitStackSignals();

    std::vector<CommandPtr> m_undoStack;
    std::vector<CommandPtr> m_redoStack;
};

/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "./undo_stack.h"

UndoStack::UndoStack(QObject* parent)
    : QObject(parent)
{
}

void UndoStack::push(CommandPtr command)
{
    if (!command) {
        return;
    }

    command->execute();
    m_undoStack.push_back(std::move(command));
    m_redoStack.clear();
    emitStackSignals();
}

void UndoStack::undo()
{
    if (!canUndo()) {
        return;
    }

    CommandPtr command = std::move(m_undoStack.back());
    m_undoStack.pop_back();
    command->undo();
    m_redoStack.push_back(std::move(command));
    emitStackSignals();
}

void UndoStack::redo()
{
    if (!canRedo()) {
        return;
    }

    CommandPtr command = std::move(m_redoStack.back());
    m_redoStack.pop_back();
    command->redo();
    m_undoStack.push_back(std::move(command));
    emitStackSignals();
}

bool UndoStack::canUndo() const
{
    return !m_undoStack.empty();
}

bool UndoStack::canRedo() const
{
    return !m_redoStack.empty();
}

QString UndoStack::undoText() const
{
    if (!canUndo()) {
        return QString();
    }
    return m_undoStack.back()->description();
}

QString UndoStack::redoText() const
{
    if (!canRedo()) {
        return QString();
    }
    return m_redoStack.back()->description();
}

void UndoStack::clear()
{
    m_undoStack.clear();
    m_redoStack.clear();
    emitStackSignals();
}

void UndoStack::emitStackSignals()
{
    emit stackChanged();
    emit canUndoChanged(canUndo());
    emit canRedoChanged(canRedo());
}

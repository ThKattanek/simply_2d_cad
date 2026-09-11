/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "./remove_entity_command.h"
#include "../cad_document/cad_document.h"
#include "../cad_document/cad_entity.h"
#include <utility>

RemoveEntityCommand::RemoveEntityCommand(CadDocument* document, CadEntity* targetEntity, QString description)
    : m_document(document)
    , m_targetEntity(targetEntity)
    , m_description(std::move(description))
{
}

void RemoveEntityCommand::execute()
{
    if (!m_document) {
        return;
    }

    if (m_targetEntity) {
        m_removedEntity = m_document->takeEntity(m_targetEntity);
    } else {
        m_removedEntity = m_document->removeLastEntity();
    }

    if (m_removedEntity) {
        m_targetEntity = m_removedEntity.get();
    }
}

void RemoveEntityCommand::undo()
{
    if (!m_document || !m_removedEntity) {
        return;
    }
    m_targetEntity = m_document->addEntity(std::move(m_removedEntity));
}

void RemoveEntityCommand::redo()
{
    execute();
}

QString RemoveEntityCommand::description() const
{
    return m_description;
}

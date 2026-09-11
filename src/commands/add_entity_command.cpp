/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "./add_entity_command.h"
#include "../cad_document/cad_document.h"
#include "../cad_document/cad_entity.h"
#include <utility>

AddEntityCommand::AddEntityCommand(CadDocument* document, std::unique_ptr<CadEntity> entity, QString description)
    : m_document(document)
    , m_entity(std::move(entity))
    , m_description(std::move(description))
{
}

void AddEntityCommand::execute()
{
    if (!m_document || !m_entity) {
        return;
    }
    m_addedEntity = m_document->addEntity(std::move(m_entity));
}

void AddEntityCommand::undo()
{
    if (!m_document || !m_addedEntity) {
        return;
    }
    m_entity = m_document->takeEntity(m_addedEntity);
    m_addedEntity = nullptr;
}

void AddEntityCommand::redo()
{
    execute();
}

QString AddEntityCommand::description() const
{
    return m_description;
}

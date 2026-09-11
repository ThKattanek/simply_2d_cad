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

#include "./command.h"

class CadDocument;
class CadEntity;

class AddEntityCommand : public Command
{
public:
    AddEntityCommand(CadDocument* document, std::unique_ptr<CadEntity> entity, QString description);

    void execute() override;
    void undo() override;
    void redo() override;
    QString description() const override;

private:
    CadDocument* m_document = nullptr;
    std::unique_ptr<CadEntity> m_entity;
    QString m_description;
};

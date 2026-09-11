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

#include <QString>
#include <memory>

class Command
{
public:
    virtual ~Command() = default;

    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual void redo() = 0;
    virtual QString description() const = 0;
};

using CommandPtr = std::unique_ptr<Command>;

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

#include <QDialog>

namespace Ui {
class AppSettingsDialog;
}

class AppSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AppSettingsDialog(QWidget *parent = nullptr);
    ~AppSettingsDialog();

protected:
    void changeEvent(QEvent *event) override;

private slots:
    void onApply();
    void onAbort();
    void onDefault();

private:
    void retranslateDynamicTexts();
    void loadSettingsToUi();
    void saveSettingsFromUi();

    Ui::AppSettingsDialog *ui;
};

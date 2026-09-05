/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "app_settings_dialog.h"
#include "ui_app_settings_dialog.h"

#include <QSettings>
#include <QPushButton>

AppSettingsDialog::AppSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AppSettingsDialog)
{
    ui->setupUi(this);
    loadSettingsToUi();

    ui->listSettingGroups->setCurrentRow(1); // Select the first group by default

    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &AppSettingsDialog::onApply);
    connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this, &AppSettingsDialog::onDefault);
    connect(ui->buttonBox->button(QDialogButtonBox::Abort), &QPushButton::clicked, this, &AppSettingsDialog::onAbort);
}

AppSettingsDialog::~AppSettingsDialog()
{
    delete ui;
}

void AppSettingsDialog::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        retranslateDynamicTexts();
    }
}

void AppSettingsDialog::onApply()
{
    saveSettingsFromUi();
    accept(); // Close the dialog with QDialog::Accepted result
}

void AppSettingsDialog::onAbort()
{
}

void AppSettingsDialog::onDefault()
{
    // Reset settings to default values
}

void AppSettingsDialog::retranslateDynamicTexts()
{
    // Update any dynamic texts in the dialog that need to be retranslated when the language changes
}

void AppSettingsDialog::loadSettingsToUi()
{
    // Load settings from the application settings to the UI elements
    QSettings settings;

    ui->spinSnapMarkerSize->setValue(settings.value("Snap/MarkerSize", 10).toInt());
}

void AppSettingsDialog::saveSettingsFromUi()
{
    // Save settings from the UI elements to the application settings
    QSettings settings;

    settings.setValue("Snap/MarkerSize", ui->spinSnapMarkerSize->value());

    emit settingsChanged();
}

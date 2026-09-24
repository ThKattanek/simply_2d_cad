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
#include <QFileDialog>
#include <QStandardPaths>

AppSettingsDialog::AppSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AppSettingsDialog)
{
    ui->setupUi(this);
    loadSettingsToUi();

    ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)->setDefault(false);
    ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)->setAutoDefault(false);
    ui->buttonBox->button(QDialogButtonBox::Apply)->setDefault(false);
    ui->buttonBox->button(QDialogButtonBox::Apply)->setAutoDefault(false);
    ui->buttonBox->button(QDialogButtonBox::Abort)->setDefault(false);
    ui->buttonBox->button(QDialogButtonBox::Abort)->setAutoDefault(false);

    ui->listSettingGroups->setCurrentRow(0); // Select the first group by default

    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &AppSettingsDialog::onApply);
    connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this, &AppSettingsDialog::onDefault);
    connect(ui->buttonBox->button(QDialogButtonBox::Abort), &QPushButton::clicked, this, &AppSettingsDialog::onAbort);
    connect(ui->btnBrowseSavePath, &QPushButton::clicked, this, &AppSettingsDialog::onBrowseButtonClicked);
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
    reject(); // Close the dialog with QDialog::Rejected result
}

void AppSettingsDialog::onDefault()
{
    // Reset settings to default values
    switch (ui->listSettingGroups->currentRow()) {
    case 0: // General Settings
        ui->txtDefaultSavePath->setText(QDir::homePath() + "/Simply2dCad");
        ui->checkBoxEnableAntialiasing->setChecked(false);
        break;

    case 1: // Snap Settings
        ui->spinSnapMarkerSize->setValue(12);
        ui->spinSnapTolerance->setValue(10);
        break;
    default:
        break;
    }
}

void AppSettingsDialog::onBrowseButtonClicked()
{
    QString current = ui->txtDefaultSavePath->text();
    if (current.isEmpty()) {
        current = QDir::homePath() + "/Simply2dCad";
    }

    QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Select Default Save Directory"),
        current
        );

    if (!dir.isEmpty()) {
        ui->txtDefaultSavePath->setText(dir);
    }
}

void AppSettingsDialog::retranslateDynamicTexts()
{
    // Update any dynamic texts in the dialog that need to be retranslated when the language changes
}

void AppSettingsDialog::loadSettingsToUi()
{
    // Load settings from the application settings to the UI elements
    QSettings settings;
    QString defaultPath = QDir::homePath() + "/Simply2dCad";

    ui->txtDefaultSavePath->setText(settings.value("General/DefaultSavePath", defaultPath).toString());
    ui->spinSnapMarkerSize->setValue(settings.value("Snap/MarkerSize", 12).toInt());
    ui->spinSnapTolerance->setValue(settings.value("Snap/TolerancePixels", 10.0).toDouble());
    ui->checkBoxEnableAntialiasing->setChecked(settings.value("General/EnableAntialiasing", false).toBool());
}

void AppSettingsDialog::saveSettingsFromUi()
{
    ui->spinSnapMarkerSize->interpretText();
    ui->spinSnapTolerance->interpretText();

    // Save settings from the UI elements to the application settings
    QSettings settings;

    settings.setValue("General/DefaultSavePath", ui->txtDefaultSavePath->text().trimmed());
    settings.setValue("Snap/MarkerSize", ui->spinSnapMarkerSize->value());
    settings.setValue("Snap/TolerancePixels", ui->spinSnapTolerance->value());
    settings.setValue("General/EnableAntialiasing", ui->checkBoxEnableAntialiasing->isChecked());

    emit settingsChanged();
}

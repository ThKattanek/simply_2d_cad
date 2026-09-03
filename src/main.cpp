/*
 * Simply 2D CAD
 * Copyright (C) 2026 Thorsten Kattanek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "mainwindow.h"

#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Globale Metadaten für QSettings
    QCoreApplication::setOrganizationName("simply_2d_cad");
    QCoreApplication::setApplicationName("simply_2d_cad");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    // 1. Gespeicherte Sprache laden (Fallback: System-Sprache)
    QSettings settings;
    QString langName = settings.value("ui/language", QLocale::system().name()).toString();
    QLocale currentLocale(langName);

    // 2. Übersetzer laden (falls du .qm Dateien nutzt)
    QTranslator translator;
    if (translator.load(currentLocale, "simply_2d_cad", "_", ":/i18n")) {
        app.installTranslator(&translator);
    }

    MainWindow w;
    w.show();
    return QApplication::exec();
}
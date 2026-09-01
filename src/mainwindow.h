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

#include <QMainWindow>
#include <QTranslator>
#include <QToolBar>
#include <QLabel>

#include "./cad_view.h"
#include "./cad_scene.h"
#include "./cad_document/cad_document.h"
#include "./cad_tool_manager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void changeEvent(QEvent *event) override;

private slots:
    void updateCursorPosition(const QPointF& position);
    void on_action_Close_triggered();
    void on_actionSave_triggered();
    void on_actionLoad_triggered();
    void on_actionExportAsDxf_triggered();


    void on_actionImport_triggered();

private:
    void createLanguageMenu();
    void switchLanguage(const QString &qmFileName);
    void createToolBar();
    void zoomToFitGeometry();
    bool exportDxf(const QString &fileName);
    bool importDxf(const QString &fileName);

    Ui::MainWindow *ui;
    QTranslator m_translator;
    QMenu *m_langMenu = nullptr;
    QToolBar *toolBar = nullptr;
    QLabel *m_coordLabel = nullptr;

    CadView *m_cadView = nullptr;
    CadScene *m_cadScene = nullptr;
    CadToolManager *m_toolManager = nullptr;

    CadDocument *m_cadDocument = nullptr;
};

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
#include "ui_mainwindow.h"

#include "app_settings_dialog.h"

#include <QDir>
#include <QLocale>
#include <QMenu>
#include <QMenuBar>
#include <QActionGroup>
#include <QApplication>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QDirIterator>
#include <QLibraryInfo>

#include "./cad_tool_manager.h"
#include "./dxf_manager.h"

#include "./select_tool.h"
#include "./line_tool.h"
#include "./point_tool.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. Gespeicherte Sprache laden (Fallback: System-Sprache)
    QString langName = m_settings.value("Ui/Language", QLocale::system().name()).toString();
    QLocale currentLocale(langName);

    // 2. Übersetzer laden (falls du .qm Dateien nutzt)
    qApp->removeTranslator(&m_translator);
    if (m_translator.load(currentLocale, "simply_2d_cad", "_", ":/i18n")) {
        qApp->installTranslator(&m_translator);
    }

    createLanguageMenu();
    ui->retranslateUi(this);

    initializeCommandToolbar();

    connectSnapSettingsToUi();

    m_cadDocument = new CadDocument(this);

    m_toolManager = new CadToolManager(this);
    m_cadScene = new CadScene(m_toolManager, this);
    m_cadScene->setDocument(m_cadDocument);
    m_toolManager->setScene(m_cadScene);
    m_cadView = new CadView(m_cadScene, this);

    connect(m_cadView, &CadView::cancelCurrentTool, m_cadScene, &CadScene::cancelCurrentTool);

    m_cadView->setCommandInput(m_commandInput);

    setCentralWidget(m_cadView);

    m_coordLabel = new QLabel("X: 0.00 | Y: 0.00", this);
    m_coordLabel->setMinimumWidth(150);
    m_coordLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // Ganz rechts in die Statusleiste einbetten (wird NIE von StatusTips verdeckt)

    // "addPermanentWidget" means that the widget is permanently added to the status bar and is not affected by temporary messages (like status tips).
    // It will always be visible in the status bar.
    ui->statusbar->addPermanentWidget(m_coordLabel);

    // Connect the cursorPositionChanged signal to the updateCursorPosition slot
    connect(m_cadScene, &CadScene::cursorPositionChanged,
            this, &MainWindow::updateCursorPosition);

    // Register tools under the objectNames from the UI (MainWindow.ui)
    m_toolManager->registerTool("actionToolSelect", std::make_shared<SelectTool>());
    m_toolManager->registerTool("actionToolPoint", std::make_shared<PointTool>());
    m_toolManager->registerTool("actionToolLine", std::make_shared<LineTool>());

    // Automatically bind UI actions
    m_toolManager->bindAction(ui->actionToolSelect);
    m_toolManager->bindAction(ui->actionToolLine);
    m_toolManager->bindAction(ui->actionToolPoint);

    // Set the default tool to SelectTool
    ui->actionToolSelect->trigger();

    // Load all Settings from QSettings
    m_cadScene->loadSettings();

    // Load the layout settings (window size, position, toolbar positions) from QSettings
    loadLayoutSettings();
}

MainWindow::~MainWindow()
{
    delete ui;

    if(m_cadView != nullptr)
        delete m_cadView;

    if(m_cadScene != nullptr)
        delete m_cadScene;

    if(m_toolManager != nullptr)
        delete m_toolManager;

    if(m_cadDocument != nullptr)
        delete m_cadDocument;
}

void MainWindow::createLanguageMenu()
{
    m_langMenu = menuBar()->addMenu(tr("&Language"));

    // Create an exclusive action group for the language menu
    QActionGroup *langGroup = new QActionGroup(this);
    langGroup->setExclusive(true);

    // Scan the ":/i18n/" resource directory for .qm files
    QDir i18nDir(":/i18n/");
    QStringList qmFiles = i18nDir.entryList(QStringList() << "*.qm", QDir::Files);

    for (const QString &fileName : std::as_const(qmFiles)) {
        // Der Dateiname sieht meist so aus: "simply_2d_cad_de_DE.qm"
        // Wir wollen nur das "de_DE" extrahieren:
        QString localeCode = fileName;
        localeCode.remove("simply_2d_cad_"); // Präfix deines Targets entfernen
        localeCode.remove(".qm");            // Dateiendung entfernen

        // Mache aus "de_DE" einen echten Namen ("Deutsch")
        QLocale locale(localeCode);
        QString languageName = locale.nativeLanguageName();

        // Falls Qt die Sprache nicht kennt, Fallback auf den Code
        if (languageName.isEmpty()) {
            languageName = localeCode;
        }

        // Aktion erstellen
        QAction *action = new QAction(languageName, this);
        action->setCheckable(true);
        action->setData(fileName); // Speichere den Dateinamen versteckt in der Aktion

        // Ist das unsere aktuell aktive Sprache des Systems?

        if (locale.language() == (QLocale)m_translator.language()) {
            action->setChecked(true);
            switchLanguage(fileName);
        }

        langGroup->addAction(action);
        m_langMenu->addAction(action);

        // Klick-Event verbinden
        connect(action, &QAction::triggered, this, [this, action]() {
            switchLanguage(action->data().toString());
        });
    }
}

void MainWindow::switchLanguage(const QString &qmFileName)
{
    qApp->removeTranslator(&m_translator);
    qApp->removeTranslator(&m_translatorQtBase);

    if (m_translator.load(":/i18n/" + qmFileName)) {
        qApp->installTranslator(&m_translator);
        m_settings.setValue("Ui/Language", m_translator.language());
    }

    QString langCode = m_translator.language();
    QString qtTranslationsPath = QLibraryInfo::path(QLibraryInfo::TranslationsPath);

    if (m_translatorQtBase.load("qtbase_" + langCode, qtTranslationsPath)) {
        qApp->installTranslator(&m_translatorQtBase);
    }
}

void MainWindow::initializeCommandToolbar()
{
    QWidget* container = new QWidget(this);
    QVBoxLayout* vLayout = new QVBoxLayout(container);

    // Ränder und Abstände eng halten, damit die Toolbar schön schmal bleibt
    vLayout->setContentsMargins(0,2,2, 2);
    vLayout->setSpacing(2);

    m_commandPromt = new QLabel(tr("TEST TSET"), this);
    QFont font = m_commandPromt->font();
    font.setPointSize(8); // Etwas kleiner für kompakte Optik
    m_commandPromt->setFont(font);

    m_commandInput = new QLineEdit(this);
    m_commandInput->setClearButtonEnabled(true);
    m_commandInput->setMinimumWidth(250);
    m_commandInput->setFocusPolicy(Qt::NoFocus); // Damit die Toolbar nicht automatisch den Fokus bekommt

    m_commandInput->setStyleSheet(
    "QLineEdit { border: 1px solid #bcbcbc; border-radius: 3px; background-color: #ffffff; }"
    "QLineEdit:hover { border: 1px solid #bcbcbc; }"
    );

    vLayout->addWidget(m_commandPromt);
    vLayout->addWidget(m_commandInput);

    ui->tb_commandLine->addWidget(container);

    connect(m_commandInput, &QLineEdit::returnPressed, this, &MainWindow::on_commandSubmitted);
}

void MainWindow::zoomToFitGeometry()
{
    QRectF bounds;

    // Alle Items der Szene durchgehen
    for (QGraphicsItem* item : m_cadScene->items()) {

        // System-Items (Mittellinien, Fadenkreuz) überspringen
        QString itemType = item->data(Qt::UserRole + 1).toString();
        if (itemType == "SystemItem") {
            continue; // Mittellinien und Fadenkreuz ignorieren!
        }

        // Nur sichtbare Geometrie-Items einrechnen
        if (item->isVisible()) {
            bounds = bounds.united(item->sceneBoundingRect());
        }
    }

    // Wenn echte Geometrie gefunden wurde:
    if (!bounds.isEmpty() && bounds.isValid()) {

        // 5 % Rand (Margin) um die Geometrie herum hinzufügen
        double margin = qMax(bounds.width(), bounds.height()) * 0.05;

        // Mindest-Margin festlegen, falls die Geometrie z. B. nur ein einziger Punkt ist
        if (margin < 1.0) margin = 5.0;

        bounds.adjust(-margin, -margin, margin, margin);

        // Ansicht perfekt einpassen
        m_cadView->fitInView(bounds, Qt::KeepAspectRatio);

    } else {
        // Fallback: Falls die Datei keine Zeichnungselemente enthält
        m_cadView->centerOn(0, 0);
    }
}

bool MainWindow::exportDxf(const QString &fileName)
{
    if (!m_cadDocument)
        return false;

    return DxfManager::exportEntities(fileName, m_cadDocument->getEntities());
}

bool MainWindow::importDxf(const QString &fileName)
{
    if (!m_cadDocument) return false;

    std::vector<std::unique_ptr<CadEntity>> newEntities;
    if (!DxfManager::importEntities(fileName, newEntities)) {
        return false;
    }

    // Neue Entities im CadDocument registrieren
    for (auto& entity : newEntities) {
        m_cadDocument->addEntity(std::move(entity));
    }

    return true;
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        // When using Qt Designer Forms (*.ui files):
        ui->retranslateUi(this);

        // Texte, die du in C++ gesetzt hast, müssen hier neu aufgerufen werden:
        if (m_langMenu) {
            m_langMenu->setTitle(tr("&Language"));
        }
    }

    // call the base class event handler to ensure proper event processing
    QMainWindow::changeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    saveLayoutSettings();
}

void MainWindow::updateCursorPosition(const QPointF &position)
{
    m_coordLabel->setText(QString("X: %1 | Y: %2 mm")
                              .arg(position.x(), 0, 'f', 3)
                              .arg(position.y(), 0, 'f', 3));
}

void MainWindow::on_action_Close_triggered()
{
    close();
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Simply 2D CAD File"), "", tr("Simply 2D CAD File (*.s2dcad)"));
    if (!fileName.isEmpty()) {
        if (!m_cadDocument->saveToFile(fileName)) {
            QMessageBox::warning(this, tr("Error"), tr("The file could not be saved."));
        }
    }
}


void MainWindow::on_actionLoad_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Simply 2D CAD File"), "", tr("Simply 2D CAD File (*.s2dcad)"));
    if (!fileName.isEmpty()) {
        if (!m_cadDocument->loadFromFile(fileName))
        {
            m_cadView->centerOn(0, 0);
            QMessageBox::warning(this, tr("Error"), tr("The file could not be loaded."));
        } else
        {
            zoomToFitGeometry();
        }
    }
}

void MainWindow::on_actionExportAsDxf_triggered()
{
    if(!exportDxf(QFileDialog::getSaveFileName(this, tr("Export as DXF"), "", tr("DXF File (*.dxf)"))))
    {
        QMessageBox::warning(this, tr("Error"), tr("The DXF file could not be exported."));
    }
}

void MainWindow::on_actionImport_triggered()
{
    if(!importDxf(QFileDialog::getOpenFileName(this, tr("Import DXF"), "", tr("DXF File (*.dxf)"))))
    {
        QMessageBox::warning(this, tr("Error"), tr("The DXF file could not be imported."));
    } else
    {
        zoomToFitGeometry();
    }
}

void MainWindow::on_actionOptions_triggered()
{
    AppSettingsDialog settingsDialog(this);
    if (settingsDialog.exec() == QDialog::Accepted) {
        m_cadScene->loadSettings(); // Reload settings after changes
    }
}

void MainWindow::on_commandSubmitted()
{
    QString input = m_commandInput->text().trimmed();
    if (input.isEmpty()) return;

    bool isRelative = false;
    if (input.startsWith('@')) {
        isRelative = true;
        input.remove(0, 1); // '@' entfernen
    }

    QPointF parsedPoint;
    bool validParse = false;

    // 1. Polarkoordinaten prüfen (Format: Länge<Winkel)
    if (input.contains('<')) {
        QStringList parts = input.split('<');
        if (parts.size() == 2) {
            bool okLen = false, okAngle = false;
            double length = parts[0].toDouble(&okLen);
            double angleDeg = parts[1].toDouble(&okAngle);

            if (okLen && okAngle) {
                // Grad in Radian umrechnen (Standard CAD: 0° = Rechts, 90° = Oben)
                double angleRad = qDegreesToRadians(angleDeg);
                parsedPoint = QPointF(length * std::cos(angleRad), -length * std::sin(angleRad));
                validParse = true;
            }
        }
    }
    // 2. Kartesische Koordinaten prüfen (Format: X,Y)
    else if (input.contains(',')) {
        QStringList parts = input.split(',');
        if (parts.size() == 2) {
            bool okX = false, okY = false;
            double x = parts[0].toDouble(&okX);
            double y = parts[1].toDouble(&okY);

            if (okX && okY) {
                parsedPoint = QPointF(x, y);
                validParse = true;
            }
        }
    }

    if (validParse) {
        // Bei Relativkoordinaten den Punkt zum letzten geklickten Punkt aufaddieren
        if (isRelative) {
            QPointF lastPt = m_cadScene->getLastPoint(); // Letzter Basispunkt aus der Szene
            parsedPoint += lastPt;
        }

        // An das aktive Werkzeug übergeben
        m_cadScene->handleCommandInputPoint(parsedPoint);
        m_commandInput->clear();
    }
}

void MainWindow::saveLayoutSettings()
{
    m_settings.setValue("MainWindow/Geometry", saveGeometry());
    m_settings.setValue("MainWindow/State", saveState());
}

void MainWindow::loadLayoutSettings()
{
    QSettings settings;
    if (settings.contains("MainWindow/Geometry")) {
        restoreGeometry(settings.value("MainWindow/Geometry").toByteArray());
    }
    if (settings.contains("MainWindow/State")) {
        // Stellt exakt wieder her, wo die SnapToolBar zuletzt lag!
        restoreState(settings.value("MainWindow/State").toByteArray());
    }
}

void MainWindow::loadSnapSettingsToUi()
{
    QSettings settings;

    // QSignalBlocker verhindert unbeabsichtigte Signal-Kaskaden beim Initialisieren
    {
        const QSignalBlocker b0(ui->actionSnapPoint);
        ui->actionSnapPoint->setChecked(settings.value("Snap/PointSnapEnabled", true).toBool());

        const QSignalBlocker b1(ui->actionSnapEndpoint);
        ui->actionSnapEndpoint->setChecked(settings.value("Snap/EndpointSnapEnabled", true).toBool());
    }
    {
        const QSignalBlocker b2(ui->actionSnapMidpoint);
        ui->actionSnapMidpoint->setChecked(settings.value("Snap/MidpointSnapEnabled", true).toBool());
    }
    {
        const QSignalBlocker b3(ui->actionSnapIntersection);
        ui->actionSnapIntersection->setChecked(settings.value("Snap/IntersectionSnapEnabled", true).toBool());
    }
}

void MainWindow::connectSnapSettingsToUi()
{
    // Initialen Status der Actions aus QSettings laden
    loadSnapSettingsToUi();

    // Signale der Designer-Actions verbinden

    connect(ui->actionSnapPoint, &QAction::toggled, this, [this](bool checked) {
        QSettings settings;
        settings.setValue("Snap/PointSnapEnabled", checked);
        m_cadScene->loadSettings();
    });

    connect(ui->actionSnapEndpoint, &QAction::toggled, this, [this](bool checked) {
        QSettings settings;
        settings.setValue("Snap/EndpointSnapEnabled", checked);
        m_cadScene->loadSettings();
    });

    connect(ui->actionSnapMidpoint, &QAction::toggled, this, [this](bool checked) {
        QSettings settings;
        settings.setValue("Snap/MidpointSnapEnabled", checked);
        m_cadScene->loadSettings();
    });

    connect(ui->actionSnapIntersection, &QAction::toggled, this, [this](bool checked) {
        QSettings settings;
        settings.setValue("Snap/IntersectionSnapEnabled", checked);
        m_cadScene->loadSettings();
    });
}


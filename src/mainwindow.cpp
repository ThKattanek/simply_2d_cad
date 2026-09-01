#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    createLanguageMenu();
    ui->retranslateUi(this);

    m_cadDocument = new CadDocument(this);

    m_toolManager = new CadToolManager(this);
    m_cadScene = new CadScene(m_toolManager, this);
    m_cadScene->setDocument(m_cadDocument);
    m_toolManager->setScene(m_cadScene);
    m_cadView = new CadView(m_cadScene, this);

    setCentralWidget(m_cadView);

    m_coordLabel = new QLabel("X: 0.00 | Y: 0.00", this);
    m_coordLabel->setMinimumWidth(150);
    ui->statusbar->addWidget(m_coordLabel);

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





  //  ui->actionToolPoint->setIcon(QIcon(":/icons/test.svg"));

    showMaximized();
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
        if (locale.language() == QLocale::system().language()) {
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
    if (m_translator.load(":/i18n/" + qmFileName)) {
        qApp->installTranslator(&m_translator);
    }
    else {
    }
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


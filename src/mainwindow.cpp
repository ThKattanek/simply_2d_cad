#include "mainwindow.h"
#include "linetool.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QLocale>
#include <QMenu>
#include <QMenuBar>
#include <QActionGroup>
#include <QApplication>
#include <QAction>

#include "./cadtoolmanager.h"

#define SCENE_START_X -100000
#define SCENE_START_Y -100000
#define SCENE_WIDTH 200000
#define SCENE_HEIGHT 200000

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createLanguageMenu();
    ui->retranslateUi(this);

    m_toolManager = new CADToolManager(this);
    m_cadScene = new CADScene(m_toolManager, this);
    m_cadView = new CADView(m_cadScene, this);
    setCentralWidget(m_cadView);

    // 1. Werkzeuge unter den objectNames aus der UI (MainWindow.ui) registrieren​
    //m_toolManager->registerTool("actionToolSelect", std::make_shared<SelectTool>());
    m_toolManager->registerTool("actionToolLine", std::make_shared<LineTool>());

    // 2. UI-Actions automatisch verknüpfen​
    //m_toolManager->bindAction(ui->actionToolSelect);
    m_toolManager->bindAction(ui->actionToolLine);

    // Standard-Werkzeug aktivieren​
    ui->actionToolSelect->trigger();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createLanguageMenu()
{
    m_langMenu = menuBar()->addMenu(tr("&Language"));

    // ActionGroup sorgt dafür, dass immer nur ein Menüpunkt gleichzeitig abgehakt ist (Radio-Button-Verhalten)
    QActionGroup *langGroup = new QActionGroup(this);
    langGroup->setExclusive(true);

    // Durchsuche den internen Ressourcen-Ordner nach kompilierten Übersetzungen
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

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        // Wenn du Qt Designer Forms (*.ui Dateien) nutzt:
        ui->retranslateUi(this);

        // Texte, die du in C++ gesetzt hast, müssen hier neu aufgerufen werden:
        if (m_langMenu) {
            m_langMenu->setTitle(tr("&Language"));
        }
    }

    // Basisklasse aufrufen, damit das normale Event-Handling weiterläuft
    QMainWindow::changeEvent(event);
}

void MainWindow::on_action_Close_triggered()
{
    close();
}
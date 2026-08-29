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

private:
    void createLanguageMenu();
    void switchLanguage(const QString &qmFileName);
    void createToolBar();

    void zoomToFitGeometry();

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

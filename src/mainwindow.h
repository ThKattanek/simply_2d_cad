#pragma once

#include <QMainWindow>
#include <QTranslator>
#include <QToolBar>
#include <QLabel>

#include "./cadview.h"
#include "./cadscene.h"
#include "./cadtoolmanager.h"

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

private:
    void createLanguageMenu();
    void switchLanguage(const QString &qmFileName);
    void createToolBar();

    Ui::MainWindow *ui;
    QTranslator m_translator;
    QMenu *m_langMenu = nullptr;
    QToolBar *toolBar = nullptr;
    QLabel *m_coordLabel = nullptr;

    CADView *m_cadView = nullptr;
    CADScene *m_cadScene = nullptr;
    CADToolManager *m_toolManager = nullptr;
};

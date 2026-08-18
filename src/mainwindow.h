#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>
#include <QToolBar>

#include "./cadview.h"
#include "./cadscene.h"

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
    void on_action_Close_triggered();

private:
    void createLanguageMenu();
    void switchLanguage(const QString &qmFileName);
    void createToolBar();

    Ui::MainWindow *ui;
    QTranslator m_translator;
    QMenu *m_langMenu = nullptr;
    QToolBar *toolBar = nullptr;

    CADView *m_cadView;
    CADScene *m_cadScene;
};
#endif // MAINWINDOW_H

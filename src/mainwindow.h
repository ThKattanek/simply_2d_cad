#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>

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

    Ui::MainWindow *ui;
    QTranslator m_translator;
    QMenu *m_langMenu = nullptr;
};
#endif // MAINWINDOW_H

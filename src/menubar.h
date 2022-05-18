#ifndef MENUBAR_H
#define MENUBAR_H
#include <QMenu>
#include <QAction>
#include <QFileDialog>

class FileMenu : public QMenu
{
    Q_OBJECT

public:
    FileMenu(const QString& title, QWidget *parent);

private slots:
    void openFolder();

signals:
    void openFolderPushed(const QString& folderPath);
    void addFolderPushed();
    void saveFolderPushed();
    void updateFolderPushed();
    void reloadFolderPushed();
};


class WidgetMenu : public QMenu
{
    Q_OBJECT

public:
    WidgetMenu(const QString& title, QWidget *parent);

signals:
    void clearOutputWindowRequested();
    void clearConsoleWindowRequested();
    void openEditorSettingRequested();
    void openGnuplotSettingRequested();
    void openTemplateCustomRequested();
};


class HelpMenu : public QMenu
{
    Q_OBJECT

public:
    HelpMenu(const QString& title, QWidget *parent);

signals:
    void rebootRequested();
};


class ScriptMenu : public QMenu
{
    Q_OBJECT

public:
    ScriptMenu(const QString& title, QWidget *parent);

signals:
    void closeProcessRequested();
    void saveAsTemplateRequested();
};


class SheetMenu : public QMenu
{
    Q_OBJECT

public:
    SheetMenu(const QString& title, QWidget *parent);

    void setAutoUpdateMenuText(const bool isEnable);

private:
    QAction *autoUpdateAction;

signals:
    void openInNewWindowRequested();
    void autoTableUpdateRequested();
};

















#endif // MENUBAR_H

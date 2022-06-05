#ifndef MENUBAR_H
#define MENUBAR_H
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QPushButton>
#include "filetreewidget.h"
#include "layoutparts.h"


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
    void openFileTreeSettingRequested();
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









class MenuBarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MenuBarWidget(QWidget *parent = nullptr);

public:
    void setScript(TreeFileItem *item);
    void setSheet(TreeFileItem *item);
    void changeAutoUpdateSheetMenuText(const bool isAuto);


private slots:
    void initializeMenu();
    void setName(TreeFileItem *item);
    void removeScript();
    void removeSheet();

private:
    const QString activeButtonSheet = "QPushButton { background: transparent; text-align: left; } QPushButton:hover { background-color: rgb(231, 241, 255); }";
    const QString emptyButtonSheet = "QPushButton { background:transparent; }";

    const int barHeight = 20;

    QPushButton *scriptButton;
    QPushButton *sheetButton;
    mlayout::IconLabel *scriptIcon;
    mlayout::IconLabel *sheetIcon;
    QSpacerItem *spacer;
    QPushButton *runButton;

    QMenu *emptyMenu;
    QMenu *scriptMenu;
    QMenu *sheetMenu;
    QAction *autoUpdateAction;

    QMetaObject::Connection renameScriptConnection;
    QMetaObject::Connection removeScriptConnection;
    QMetaObject::Connection renameSheetConnection;
    QMetaObject::Connection removeSheetConnection;

signals:
    void closeProcessRequested();
    void saveAsTemplateRequested();
    void openInNewWindowRequested();
    void autoSheetUpdateRequested();
    void runRequested();
};










#endif // MENUBAR_H

/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

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
    void gnuplotHelpRequested();
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
    void changeScriptState(const bool isSaved);
    void changeSheetState(const bool isSaved);

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
    QMetaObject::Connection scriptSavedConnection;
    QMetaObject::Connection renameSheetConnection;
    QMetaObject::Connection removeSheetConnection;
    QMetaObject::Connection sheetSavedConnection;

signals:
    void closeProcessRequested();
    void saveAsTemplateRequested();
    void openInNewWindowRequested();
    void autoSheetUpdateRequested();
    void runRequested();
};





class TextEdit;

class EditorMenu : public QMenu
{
    Q_OBJECT
public:
    EditorMenu(const QString& title, QWidget *parent);

    void setEditor(TextEdit *editor) { this->editor = editor; }

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void commentOut();
    void enableAutoRun();
    void showGnuplotHelp();

private:
    TextEdit *editor;

    QAction *standardContextMenuAction;

    QAction *commentOutAction;
    QAction *showGnuplotHelpAction;
    QAction *autoRunAction;
    QAction *saveAsTemplateAction;
    QAction *settingWidgetAction;
    QAction *reloadScriptAction;

signals:
    void openEditorSettingRequested();
    void reloadScriptRequested();
    void saveAsTemplateRequested();
};










#endif // MENUBAR_H

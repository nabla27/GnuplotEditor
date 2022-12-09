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

class TreeFileItem;


class FileMenu : public QMenu
{
    Q_OBJECT
public:
    explicit FileMenu(const QString& title, QWidget *parent);

private:
    QAction *aOpenFolder;
    QAction *aAddFolder;
    QAction *aSaveFolder;
    QAction *aUpdateFolder;
    QAction *aReloadFolder;
    QAction *aOpenFile;
    QAction *aNewFile;
    QAction *aOpenTreeSetting;

signals:
    void openFolderRequested();
    void addFolderRequested();
    void saveFolderRequested();
    void updateFolderRequested();
    void reloadFolderRequested();
    void openFileRequested();
    void newFileRequested();
    void openTreeSettingRequested();
};







class EditorMenu : public QMenu
{
    Q_OBJECT
public:
    explicit EditorMenu(const QString& title, QWidget *parent);

public:
    void setCurrentItem(TreeFileItem *item);

private slots:
    void reloadFile();
    void saveFile();
    void saveFileAs();
    void setAutoRun();
    void openInNewWindow();
    void closeProcess();
    void commentOutScript();
    void emitSaveAsTemplate();

private:
    QAction *aReloadFile;
    QAction *aSaveFile;
    QAction *aSaveAllFiles;
    QAction *aSaveFileAs;
    QAction *aAutoRun;
    QAction *aFind;
    QAction *aOpenInNewWindow;
    QAction *aRun;
    QAction *aCloseProcess;
    QAction *aCommentOutScript;
    QAction *aShowCmdHelpScript;
    QAction *aSaveAsTemplate;

    QAction *standardContextAction = nullptr;
    QMenu *standardContextMenu = nullptr;
    TreeFileItem *currentItem = nullptr;

signals:
    void saveAllFileRequested();
    void findRequested();
    void runRequested(TreeFileItem*);
    void showCmdHelpRequested();
    void saveAsTemplateRequested(const QString&);
};








class GnuplotMenu : public QMenu
{
    Q_OBJECT
public:
    explicit GnuplotMenu(const QString& title, QWidget *parent);

private:
    QAction *aScriptTemplate;
    QAction *aGnuplotSetting;
    QAction *aHelpDocument;

signals:
    void showScriptTemplateRequested();
    void showGnuplotSettingRequested();
    void showGnuplotHelpRequested();
};









class ViewMenu : public QMenu
{
    Q_OBJECT
public:
    explicit ViewMenu(const QString& title, QWidget *parent);

private:
    QAction *aSplitVertically;
    QAction *aSplitHorizontally;
    QAction *aUnsplit;
    QAction *aClearOutputWidget;
    QAction *aClearConsoleWidget;
    QAction *aEditorLayoutSetting;
};








class HelpMenu : public QMenu
{
    Q_OBJECT
public:
    explicit HelpMenu(const QString& title, QWidget *parent);

private:
    QAction *aLogWindow;
    QAction *aReboot;
};












#endif // MENUBAR_H

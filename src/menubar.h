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
class LogBrowserWidget;
class PluginListWidget;
class TextEdit;


class FileMenu : public QMenu
{
    Q_OBJECT
public:
    explicit FileMenu(const QString& title, QWidget *parent);

public slots:
    void setCurrentItem(TreeFileItem *item);

private slots:
    void reloadFile();
    void saveFile();
    void saveFileAs();

private:
    void setActionEnable(bool enable);
    void resetCurrentItem();

private:
    QAction *aOpenFolder;
    QAction *aAddFolder;
    QAction *aSaveFolder;
    QAction *aUpdateFolder;
    QAction *aReloadFolder;

    QAction *aOpenFile;
    QAction *aNewFile;

    QAction *aReloadFile;
    QAction *aSaveFile;
    QAction *aSaveAllFiles;
    QAction *aSaveFileAs;

    QAction *aOpenTreeSetting;

    TreeFileItem *currentItem = nullptr;

signals:
    void openFolderRequested();
    void addFolderRequested();
    void saveFolderRequested();
    void updateFolderRequested();
    void reloadFolderRequested();
    void openFileRequested();
    void newFileRequested();
    void saveAllFilesRequested();
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
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void deleteText();
    void selectAll();
    void openInNewWindow();

private:
    void setActionDisable();
    void resetCurrentItem();

private:

    QAction *aUndo;
    QAction *aRedo;
    QAction *aCut;
    QAction *aCopy;
    QAction *aPaste;
    QAction *aDelete;
    QAction *aSelectAll;

    QAction *aFind;
    QAction *aOpenInNewWindow;

    TreeFileItem *currentItem = nullptr;

signals:
    void findRequested();
};








class GnuplotMenu : public QMenu
{
    Q_OBJECT
public:
    explicit GnuplotMenu(const QString& title, QWidget *parent);

public:
    void setCurrentItem(TreeFileItem *item);

private slots:
    void setAutoRun();
    void closeProcess();
    void reStart();
    void runDetached();
    void commentOut();
    void emitSaveAsTemplate();

private:
    void resetCurrentItem();
    void setupActionEnable(bool enable);

private:
    QAction *aRun;
    QAction *aCloseProcess;
    QAction *aReStart;
    QAction *aAutoRun;
    QAction *aRunDetached;

    QAction *aCommentOut;
    QAction *aShowCmdHelp;
    QAction *aHelpDocument;
    QAction *aSaveAsTemplate;
    QAction *aScriptTemplate;
    QAction *aGnuplotSetting;

    TreeFileItem *currentItem = nullptr;

signals:
    void runRequested(TreeFileItem *item);
    void showCmdHelpRequested();
    void showGnuplotHelpRequested();
    void saveAsTemplateRequested(const QString&);
    void showScriptTemplateRequested();
    void showGnuplotSettingRequested();
};









class ViewMenu : public QMenu
{
    Q_OBJECT
public:
    explicit ViewMenu(const QString& title, QWidget *parent);

private:
    QAction *aSplitHorizontally;
    QAction *aSplitVertically;
    QAction *aUnsplit;
    QAction *aRemoveAllStackedEditor;
    QAction *aClearOutputWidget;
    QAction *aClearConsoleWidget;
    QAction *aEditorLayoutSetting;

signals:
    void splitHorizontallyRequested();
    void splitVerticallyRequested();
    void unsplitRequested();
    void removeAllStackedEditorRequested();
    void clearOutputWidgetRequested();
    void clearConsoleWidgetRequested();
    void showEditorSettingRequested();
};








class HelpMenu : public QMenu
{
    Q_OBJECT
public:
    explicit HelpMenu(const QString& title, QWidget *parent);

private:
    QAction *aLogWindow;
    QAction *aPlugins;
    QAction *aReboot;

    LogBrowserWidget *logWindow;
    PluginListWidget *pluginSetting;

signals:
    void rebootRequested();
};












#endif // MENUBAR_H

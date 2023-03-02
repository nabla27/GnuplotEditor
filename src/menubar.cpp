/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "menubar.h"

#include <QAction>
#include <QFileDialog>
#include <QClipboard>
#include "filetreewidget.h"
#include "textedit.h"
#include "standardpixmap.h"
#include "logger.h"
#include "plugin.h"


FileMenu::FileMenu(const QString &title, QWidget *parent)
    : QMenu(title, parent)
    , aOpenFolder(new QAction("Open Folder", this))
    , aAddFolder(new QAction("Add Folder", this))
    , aSaveFolder(new QAction("Save As New Folder", this))
    , aUpdateFolder(new QAction("Update File Tree", this))
    , aReloadFolder(new QAction("Reload File Tree", this))

    , aOpenFile(new QAction("Open File", this))
    , aNewFile(new QAction("New File", this))

    , aReloadFile(new QAction("Reload File", this))
    , aSaveFile(new QAction("Save File", this))
    , aSaveAllFiles(new QAction("Save All Files", this))
    , aSaveFileAs(new QAction("Save File As", this))

    , aOpenTreeSetting(new QAction("File Tree Setting", this))
{
    addAction(aOpenFolder);
    addAction(aAddFolder);
    addAction(aSaveFolder);
    addAction(aUpdateFolder);
    addAction(aReloadFolder);
    addSeparator();
    addAction(aOpenFile);
    addAction(aNewFile);
    addSeparator();
    addAction(aReloadFile);
    addAction(aSaveFile);
    addAction(aSaveAllFiles);
    addAction(aSaveFileAs);
    addSeparator();
    addAction(aOpenTreeSetting);

    aOpenFolder->setIcon(QIcon(StandardPixmap::File::folderOpen()));
    aAddFolder->setIcon(QIcon(StandardPixmap::File::folderAdd()));
    aReloadFolder->setIcon(QIcon(StandardPixmap::Icon::reload()));
    aOpenFile->setIcon(QIcon(StandardPixmap::File::fileAdd()));
    aNewFile->setIcon(QIcon(StandardPixmap::File::normal()));
    aReloadFile->setIcon(QIcon(StandardPixmap::File::fileReload()));
    aSaveFile->setIcon(QIcon(StandardPixmap::File::fileSave()));
    aSaveFileAs->setIcon(QIcon(StandardPixmap::File::fileSaves()));

    aOpenFile->setShortcut(QKeySequence("Ctrl+O"));
    aNewFile->setShortcut(QKeySequence("Ctrl+N"));
    aSaveFile->setShortcut(QKeySequence("Ctrl+S"));
    aSaveAllFiles->setShortcut(QKeySequence("Ctrl+Shift+S"));

    connect(aOpenFolder, &QAction::triggered, this, &FileMenu::openFolderRequested);
    connect(aAddFolder, &QAction::triggered, this, &FileMenu::addFolderRequested);
    connect(aSaveFolder, &QAction::triggered, this, &FileMenu::saveFolderRequested);
    connect(aUpdateFolder, &QAction::triggered, this, &FileMenu::updateFolderRequested);
    connect(aReloadFolder, &QAction::triggered, this, &FileMenu::reloadFolderRequested);
    connect(aOpenFile, &QAction::triggered, this, &FileMenu::openFileRequested);
    connect(aNewFile, &QAction::triggered, this, &FileMenu::newFileRequested);
    connect(aReloadFile, &QAction::triggered, this, &FileMenu::reloadFile);
    connect(aSaveFile, &QAction::triggered, this, &FileMenu::saveFile);
    connect(aSaveAllFiles, &QAction::triggered, this, &FileMenu::saveAllFilesRequested);
    connect(aSaveFileAs, &QAction::triggered, this, &FileMenu::saveFileAs);
    connect(aOpenTreeSetting, &QAction::triggered, this, &FileMenu::openTreeSettingRequested);

    setActionEnable(false);
}

void FileMenu::setActionEnable(bool enable)
{
    aReloadFile->setEnabled(enable);
    aSaveFile->setEnabled(enable);
    aSaveFileAs->setEnabled(enable);
}

void FileMenu::resetCurrentItem()
{
    currentItem = nullptr;
}

void FileMenu::setCurrentItem(TreeFileItem *item)
{
    if(currentItem)
        disconnect(currentItem, &TreeFileItem::destroyed, this, &FileMenu::resetCurrentItem);

    currentItem = item;

    if(currentItem)
        connect(currentItem, &TreeFileItem::destroyed, this, &FileMenu::resetCurrentItem);

    if(item)
    {
        setActionEnable(true);
        const QString fileName = item->fileInfo().fileName();

        aReloadFile->setText("Reload File \"" + fileName + "\"");
        aSaveFile->setText("Save File \"" + fileName + "\"");
        aSaveFileAs->setText("Save File \"" + fileName + "\" As ...");
    }
    else
    {
        setActionEnable(false);

        aReloadFile->setText("Reload File");
        aSaveFile->setText("Save File");
        aSaveFileAs->setText("Save File As ...");
    }
}

void FileMenu::reloadFile()
{
    if(currentItem)
    {
        currentItem->load();
    }
}

void FileMenu::saveFile()
{
    if(currentItem)
    {
        currentItem->save();
    }
}

void FileMenu::saveFileAs()
{
    if(!currentItem) return;

    const QString pathForSave = QFileDialog::getSaveFileName(this);

    if(pathForSave.isEmpty()) return;

    if(!QFile::copy(currentItem->fileInfo().absoluteFilePath(), pathForSave))
    {
        __LOGOUT__("failed to copy the file \"" +
                   currentItem->fileInfo().absoluteFilePath() +
                   "\" to \"" + pathForSave, Logger::LogLevel::Error);
    }
}











EditorMenu::EditorMenu(const QString &title, QWidget *parent)
    : QMenu(title, parent)

    , aUndo(new QAction("Undo", this))
    , aRedo(new QAction("Redo", this))
    , aCut(new QAction("Cut", this))
    , aCopy(new QAction("Copy", this))
    , aPaste(new QAction("Paste", this))
    , aDelete(new QAction("Delete", this))
    , aSelectAll(new QAction("Select All", this))

    , aFind(new QAction("Find", this))
    , aOpenInNewWindow(new QAction("Open In New Window", this))
{
    addAction(aUndo);
    addAction(aRedo);

    addSeparator();

    addAction(aCut);
    addAction(aCopy);
    addAction(aPaste);
    addAction(aDelete);

    addSeparator();

    addAction(aSelectAll);

    addSeparator();

    addAction(aFind);
    addAction(aOpenInNewWindow);

    aFind->setIcon(QIcon(StandardPixmap::Icon::find()));
    aOpenInNewWindow->setIcon(QIcon(StandardPixmap::Icon::openInWindow()));

    aUndo->setShortcut(QKeySequence("Ctrl+Z"));
    aRedo->setShortcut(QKeySequence("Ctrl+Y"));
    aCut->setShortcut(QKeySequence("Ctrl+X"));
    aCopy->setShortcut(QKeySequence("Ctrl+C"));
    aPaste->setShortcut(QKeySequence("Ctrl+V"));
    aSelectAll->setShortcut(QKeySequence("Ctrl+A"));

    aFind->setShortcut(QKeySequence("Ctrl+F"));
    aOpenInNewWindow->setShortcut(QKeySequence("Ctrl+W"));

    setActionDisable();

    connect(aUndo, &QAction::triggered, this, &EditorMenu::undo);
    connect(aRedo, &QAction::triggered, this, &EditorMenu::redo);
    connect(aCut, &QAction::triggered, this, &EditorMenu::cut);
    connect(aCopy, &QAction::triggered, this, &EditorMenu::copy);
    connect(aPaste, &QAction::triggered, this, &EditorMenu::paste);
    connect(aDelete, &QAction::triggered, this, &EditorMenu::deleteText);
    connect(aSelectAll, &QAction::triggered, this, &EditorMenu::selectAll);
    connect(aFind, &QAction::triggered, this, &EditorMenu::findRequested);
    connect(aOpenInNewWindow, &QAction::triggered, this, &EditorMenu::openInNewWindow);
}

void EditorMenu::setActionDisable()
{
    aUndo->setEnabled(false);
    aRedo->setEnabled(false);
    aCut->setEnabled(false);
    aCopy->setEnabled(false);
    aPaste->setEnabled(false);
    aDelete->setEnabled(false);
    aSelectAll->setEnabled(false);
    aFind->setEnabled(false);
}

void EditorMenu::resetCurrentItem()
{
    currentItem = nullptr;
}

void EditorMenu::setCurrentItem(TreeFileItem *item)
{
    if(currentItem)
        currentItem->disconnect(this);

    currentItem = item;

    if(currentItem)
    {
        aOpenInNewWindow->setEnabled(true);
        connect(item, &TreeFileItem::destroyed, this, &EditorMenu::resetCurrentItem);
    }
    else
    {
        aOpenInNewWindow->setEnabled(false);
    }

    if(!item || item->type() != (int)FileTreeWidget::TreeItemType::Script)
    {
        setActionDisable();
        return;
    }

    TextEdit *editor = static_cast<TreeScriptItem*>(item)->editor;

    if(editor)
    {
        aFind->setEnabled(true);
        aUndo->setEnabled(editor->document()->availableUndoSteps() > 0);
        aRedo->setEnabled(editor->document()->availableRedoSteps() > 0);
        const bool hasSelection = editor->textCursor().hasSelection();
        aCut->setEnabled(hasSelection);
        aCopy->setEnabled(hasSelection);
        aPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
        aDelete->setEnabled(hasSelection);
        aSelectAll->setEnabled(true);
    }
    else
    {
        setActionDisable();
    }
}

void EditorMenu::undo()
{
    if(currentItem)
    {
        if(TextEdit *editor = static_cast<TreeScriptItem*>(currentItem)->editor)
        {
            editor->undo();
        }
    }
}

void EditorMenu::redo()
{
    if(currentItem)
    {
        if(TextEdit *editor = static_cast<TreeScriptItem*>(currentItem)->editor)
        {
            editor->redo();
        }
    }
}

void EditorMenu::cut()
{
    if(currentItem)
    {
        if(TextEdit *editor = static_cast<TreeScriptItem*>(currentItem)->editor)
        {
            editor->cut();
        }
    }
}

void EditorMenu::copy()
{
    if(currentItem)
    {
        if(TextEdit *editor = static_cast<TreeScriptItem*>(currentItem)->editor)
        {
            editor->copy();
        }
    }
}

void EditorMenu::paste()
{
    if(currentItem)
    {
        if(TextEdit *editor = static_cast<TreeScriptItem*>(currentItem)->editor)
        {
            editor->paste();
        }
    }
}

void EditorMenu::deleteText()
{
    if(currentItem)
    {
        if(TextEdit *editor = static_cast<TreeScriptItem*>(currentItem)->editor)
        {
            editor->textCursor().removeSelectedText();
        }
    }
}

void EditorMenu::selectAll()
{
    if(currentItem)
    {
        if(TextEdit *editor = static_cast<TreeScriptItem*>(currentItem)->editor)
        {
            editor->selectAll();
        }
    }
}

void EditorMenu::openInNewWindow()
{
    if(currentItem)
    {
        if(QWidget *w = currentItem->widget())
        {
            w->setParent(nullptr);
            w->show();
        }
    }
}









GnuplotMenu::GnuplotMenu(const QString &title, QWidget *parent)
    : QMenu(title, parent)
    , aRun(new QAction("Run", this))
    , aCloseProcess(new QAction("Close Process", this))
    , aReStart(new QAction("Restart", this))
    , aAutoRun(new QAction("Autorun", this))
    , aRunDetached(new QAction("Run Detached With Gnuplot", this))
    , aCommentOut(new QAction("Comment Out", this))
    , aShowCmdHelp(new QAction("Help For Cmd Under Cursor", this))
    , aHelpDocument(new QAction("Help Document", this))
    , aSaveAsTemplate(new QAction("Save As Template", this))
    , aScriptTemplate(new QAction("Script Template", this))
    , aGnuplotSetting(new QAction("Gnuplot Setting", this))
{
    addAction(aRun);
    addAction(aCloseProcess);
    addAction(aReStart);
    addAction(aAutoRun);
    addAction(aRunDetached);
    addSeparator();
    addAction(aCommentOut);
    addAction(aShowCmdHelp);
    addAction(aHelpDocument);
    addAction(aSaveAsTemplate);
    addAction(aScriptTemplate);
    addAction(aGnuplotSetting);

    aRun->setIcon(QIcon(StandardPixmap::Icon::execute()));

    aRun->setShortcut(QKeySequence("Ctrl+R"));
    aCommentOut->setShortcut(QKeySequence("Ctrl+Shift+D"));
    aShowCmdHelp->setShortcut(QKeySequence("F1"));
    aHelpDocument->setShortcut(QKeySequence("F2"));
    aSaveAsTemplate->setShortcut(QKeySequence("Ctrl+Shift+T"));
    aScriptTemplate->setShortcut(QKeySequence("Ctrl+T"));
    aGnuplotSetting->setShortcut(QKeySequence("Ctrl+G"));

    connect(aRun, &QAction::triggered, [this](){ emit runRequested(currentItem); });
    connect(aCloseProcess, &QAction::triggered, this, &GnuplotMenu::closeProcess);
    connect(aReStart, &QAction::triggered, this, &GnuplotMenu::reStart);
    connect(aAutoRun, &QAction::triggered, this, &GnuplotMenu::setAutoRun);
    connect(aRunDetached, &QAction::triggered, this, &GnuplotMenu::runDetached);
    connect(aCommentOut, &QAction::triggered, this, &GnuplotMenu::commentOut);
    connect(aShowCmdHelp, &QAction::triggered, this, &GnuplotMenu::showCmdHelpRequested);
    connect(aHelpDocument, &QAction::triggered, this, &GnuplotMenu::showGnuplotHelpRequested);
    connect(aSaveAsTemplate, &QAction::triggered, this, &GnuplotMenu::emitSaveAsTemplate);
    connect(aScriptTemplate, &QAction::triggered, this, &GnuplotMenu::showScriptTemplateRequested);
    connect(aGnuplotSetting, &QAction::triggered, this, &GnuplotMenu::showGnuplotSettingRequested);

    aAutoRun->setCheckable(true);

    setupActionEnable(false);
}

void GnuplotMenu::resetCurrentItem()
{
    currentItem = nullptr;
}

void GnuplotMenu::setupActionEnable(bool enable)
{
    aRun->setEnabled(enable);
    aCloseProcess->setEnabled(enable);
    aAutoRun->setEnabled(enable);
    aRunDetached->setEnabled(enable);
    aCommentOut->setEnabled(enable);
    aShowCmdHelp->setEnabled(enable);
    aSaveAsTemplate->setEnabled(enable);
}

void GnuplotMenu::setCurrentItem(TreeFileItem *item)
{
    if(currentItem)
        currentItem->disconnect(this);

    currentItem = item;

    if(currentItem)
        connect(currentItem, &TreeFileItem::destroyed, this, &GnuplotMenu::resetCurrentItem);


    const bool isScript = (item) && (item->type() == (int)FileTreeWidget::TreeItemType::Script);

    setupActionEnable(isScript);

    if(currentItem)
    {
        if(item->isEnableUpdateTimer())
            aAutoRun->setChecked(true);
        else
            aAutoRun->setChecked(false);
    }
}

void GnuplotMenu::setAutoRun()
{
    if(currentItem)
    {
        currentItem->setUpdateTimer(!currentItem->isEnableUpdateTimer());
    }
}

void GnuplotMenu::closeProcess()
{
    if(TreeScriptItem *item = static_cast<TreeScriptItem*>(currentItem))
    {
        item->requestCloseProcess();
    }
}

void GnuplotMenu::reStart()
{
    if(TreeScriptItem *item = static_cast<TreeScriptItem*>(currentItem))
    {
        item->requestCloseProcess();
        emit runRequested(item);
    }
}

void GnuplotMenu::runDetached()
{
    __LOGOUT__("no supported.", Logger::LogLevel::Debug);
}

void GnuplotMenu::commentOut()
{
    if(TreeScriptItem *item = static_cast<TreeScriptItem*>(currentItem))
    {
        if(!item->editor)
        {
            __LOGOUT__("the editor of the scirpt\"" + item->text(0) + "\" is nullptr.", Logger::LogLevel::Warn);
            return;
        }

        item->editor->reverseSelectedCommentState();
    }
}

void GnuplotMenu::emitSaveAsTemplate()
{
    if(TreeScriptItem *item = static_cast<TreeScriptItem*>(currentItem))
    {
        if(!item->editor)
        {
            __LOGOUT__("the editor of the scirpt\"" + item->text(0) + "\" is nullptr.", Logger::LogLevel::Warn);
            return;
        }

        emit saveAsTemplateRequested(item->editor->toPlainText());
    }
}












ViewMenu::ViewMenu(const QString &title, QWidget *parent)
    : QMenu(title, parent)
    , aSplitHorizontally(new QAction("Split Horizontally", this))
    , aSplitVertically(new QAction("Split Vertically", this))
    , aUnsplit(new QAction("Unsplit", this))
    , aRemoveAllStackedEditor(new QAction("Remove All Stacked Editor", this))
    , aClearOutputWidget(new QAction("Clear Output Window", this))
    , aClearConsoleWidget(new QAction("Clear Console Window", this))
    , aEditorLayoutSetting(new QAction("Editor Layout", this))
{
    addAction(aSplitHorizontally);
    addAction(aSplitVertically);
    addAction(aUnsplit);
    addAction(aRemoveAllStackedEditor);
    addSeparator();
    addAction(aClearOutputWidget);
    addAction(aClearConsoleWidget);
    addSeparator();
    addAction(aEditorLayoutSetting);

    aSplitHorizontally->setIcon(QIcon(StandardPixmap::Icon::separateH()));
    aSplitVertically->setIcon(QIcon(StandardPixmap::Icon::separateV()));
    aUnsplit->setIcon(QIcon(StandardPixmap::Icon::closeWidget()));

    aEditorLayoutSetting->setShortcut(QKeySequence("Ctrl+E"));

    connect(aSplitHorizontally, &QAction::triggered, this, &ViewMenu::splitHorizontallyRequested);
    connect(aSplitVertically, &QAction::triggered, this, &ViewMenu::splitVerticallyRequested);
    connect(aUnsplit, &QAction::triggered, this, &ViewMenu::unsplitRequested);
    connect(aRemoveAllStackedEditor, &QAction::triggered, this, &ViewMenu::removeAllStackedEditorRequested);
    connect(aClearOutputWidget, &QAction::triggered, this, &ViewMenu::clearOutputWidgetRequested);
    connect(aClearConsoleWidget, &QAction::triggered, this, &ViewMenu::clearConsoleWidgetRequested);
    connect(aEditorLayoutSetting, &QAction::triggered, this, &ViewMenu::showEditorSettingRequested);
}







HelpMenu::HelpMenu(const QString &title, QWidget *parent)
    : QMenu(title, parent)
    , aLogWidget(new QAction("Log", this))
    , aPlugins(new QAction("Plugins", this))
    , aReboot(new QAction("Reboot", this))

    , logWidget(new LogSettingWidget(this))
    , pluginSetting(new PluginListWidget(this))
{
    addAction(aLogWidget);
    addAction(aPlugins);
    addAction(aReboot);

    connect(aLogWidget, &QAction::triggered, logWidget, &LogSettingWidget::show);
    connect(aPlugins, &QAction::triggered, pluginSetting, &PluginListWidget::show);
    connect(aReboot, &QAction::triggered, this, &HelpMenu::rebootRequested);

    logWidget->setWindowFlag(Qt::Window);
}

















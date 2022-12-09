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
#include "filetreewidget.h"

FileMenu::FileMenu(const QString &title, QWidget *parent)
    : QMenu(title, parent)
    , aOpenFolder(new QAction("Open Folder", this))
    , aAddFolder(new QAction("Add Folder", this))
    , aSaveFolder(new QAction("Save As New Folder", this))
    , aUpdateFolder(new QAction("Update File Tree", this))
    , aReloadFolder(new QAction("Reload File Tree", this))
    , aOpenFile(new QAction("Open File", this))
    , aNewFile(new QAction("New File", this))
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
    addAction(aOpenTreeSetting);

    aOpenFolder->setIcon(QIcon(QPixmap(":/icon/file_folderopen")));
    aAddFolder->setIcon(QIcon(QPixmap(":/icon/file_folderadd")));
    aReloadFolder->setIcon(QIcon(QPixmap(":/icon/icon_reload")));
    aOpenFile->setIcon(QIcon(QPixmap(":/icon/file_fileadd")));
    aNewFile->setIcon(QIcon(QPixmap(":/icon/file_normal")));

    aOpenFile->setShortcut(QKeySequence("Ctrl+O"));
    aNewFile->setShortcut(QKeySequence("Ctrl+N"));

    connect(aOpenFolder, &QAction::triggered, this, &FileMenu::openFolderRequested);
    connect(aAddFolder, &QAction::triggered, this, &FileMenu::addFolderRequested);
    connect(aSaveFolder, &QAction::triggered, this, &FileMenu::saveFolderRequested);
    connect(aUpdateFolder, &QAction::triggered, this, &FileMenu::updateFolderRequested);
    connect(aReloadFolder, &QAction::triggered, this, &FileMenu::reloadFolderRequested);
    connect(aOpenFile, &QAction::triggered, this, &FileMenu::openFileRequested);
    connect(aNewFile, &QAction::triggered, this, &FileMenu::newFileRequested);
    connect(aOpenTreeSetting, &QAction::triggered, this, &FileMenu::openTreeSettingRequested);
}







EditorMenu::EditorMenu(const QString &title, QWidget *parent)
    : QMenu(title, parent)
    , aReloadFile(new QAction("Reload File", this))
    , aSaveFile(new QAction("Save File", this))
    , aSaveAllFiles(new QAction("Save All Files", this))
    , aSaveFileAs(new QAction("Save File as", this))
    , aAutoRun(new QAction("Auto Run", this))
    , aFind(new QAction("Find", this))
    , aOpenInNewWindow(new QAction("Open In New Window", this))
    , aRun(new QAction("Run", this))
    , aCloseProcess(new QAction("Close Process", this))
    , aCommentOutScript(new QAction("Comment Out", this))
    , aShowCmdHelpScript(new QAction("Help For Command Under Cursor", this))
    , aSaveAsTemplate(new QAction("Save As Template", this))
{
    addAction(aReloadFile);
    addAction(aSaveFile);
    addAction(aSaveAllFiles);
    addAction(aSaveFileAs);
    addAction(aAutoRun);
    addAction(aFind);
    addAction(aOpenInNewWindow);
    addSeparator();
    addAction(aRun);
    addAction(aCloseProcess);
    addAction(aCommentOutScript);
    addAction(aShowCmdHelpScript);
    addAction(aSaveAsTemplate);

    aReloadFile->setIcon(QIcon(QPixmap(":/icon/file_filereload")));
    aSaveFile->setIcon(QIcon(QPixmap(":/icon/file_filesave")));
    aSaveFileAs->setIcon(QIcon(QPixmap(":/icon/file_filesaveas")));
    aFind->setIcon(QIcon(QPixmap(":/icon/icon_find")));
    aOpenInNewWindow->setIcon(QIcon(QPixmap(":/icon/icon_openinwindow")));
    aRun->setIcon(QIcon(QPixmap(":/icon/icon_execute")));

    aSaveFile->setShortcut(QKeySequence("Ctrl+S"));
    aSaveAllFiles->setShortcut(QKeySequence("Ctrl+Shift+S"));
    aFind->setShortcut(QKeySequence("Ctrl+F"));
    aOpenInNewWindow->setShortcut(QKeySequence("Ctrl+W"));
    aRun->setShortcut(QKeySequence("Ctrl+R"));
    aCommentOutScript->setShortcut(QKeySequence("Ctrl+Shift+D"));
    aShowCmdHelpScript->setShortcut(QKeySequence("F1"));
    aSaveAsTemplate->setShortcut(QKeySequence("Ctrl+Shift+T"));

    aOpenInNewWindow->setEnabled(true);

    connect(aReloadFile, &QAction::triggered, this, &EditorMenu::reloadFile);
    connect(aSaveFile, &QAction::triggered, this, &EditorMenu::saveFile);
    connect(aSaveAllFiles, &QAction::triggered, this, &EditorMenu::saveAllFileRequested);
    connect(aSaveFileAs, &QAction::triggered, this, &EditorMenu::saveFileAs);
    connect(aAutoRun, &QAction::triggered, this, &EditorMenu::setAutoRun);
    connect(aFind, &QAction::triggered, this, &EditorMenu::findRequested);
    connect(aOpenInNewWindow, &QAction::triggered, this, &EditorMenu::openInNewWindow);
    connect(aRun, &QAction::triggered, [this](){ emit runRequested(currentItem); });
    connect(aCloseProcess, &QAction::triggered, this, &EditorMenu::closeProcess);
    connect(aCommentOutScript, &QAction::triggered, this, &EditorMenu::commentOutScript);
    connect(aShowCmdHelpScript, &QAction::triggered, this, &EditorMenu::showCmdHelpRequested);
    connect(aSaveAsTemplate, &QAction::triggered, this, &EditorMenu::emitSaveAsTemplate);
}

void EditorMenu::setCurrentItem(TreeFileItem *item)
{
    currentItem = item;

    if(standardContextMenu)
    {
        standardContextMenu->deleteLater();
        standardContextMenu = nullptr;
        standardContextAction->deleteLater();
        standardContextAction = nullptr;
    }

    bool itemExists = false;
    bool isScript = false;
    QString fileName = "";

    if(item)
    {
        itemExists = true;
        isScript = item->type() == (int)FileTreeWidget::TreeItemType::Script;
        fileName = item->fileInfo().fileName();

        if(isScript)
        {
            standardContextMenu = static_cast<TreeScriptItem*>(item)->editor->createStandardContextMenu();
            standardContextMenu->setTitle("Edit");
            standardContextAction = insertMenu(aRun, standardContextMenu);
        }
    }

    aReloadFile->setEnabled(itemExists);
    aSaveFile->setEnabled(itemExists);
    aSaveFileAs->setEnabled(itemExists);
    aAutoRun->setEnabled(itemExists);
    aFind->setEnabled(itemExists);
    aOpenInNewWindow->setEnabled(itemExists);

    aReloadFile->setText("Reload File \"" + fileName + "\"");
    aSaveFile->setText("Save File \"" + fileName + "\"");
    aSaveFileAs->setText("Save File \"" + fileName + "\" As ...");

    aRun->setEnabled(isScript);
    aCloseProcess->setEnabled(isScript);
    aCommentOutScript->setEnabled(isScript);
    aShowCmdHelpScript->setEnabled(isScript);
    aSaveAsTemplate->setEnabled(isScript);
}

void EditorMenu::reloadFile()
{
    if(currentItem)
    {
        currentItem->load();
    }
}

void EditorMenu::saveFile()
{
    if(currentItem)
    {
        currentItem->save();
    }
}

void EditorMenu::saveFileAs()
{
    if(!currentItem) return;

    const QString pathForSave = QFileDialog::getSaveFileName(this);

    if(pathForSave.isEmpty()) return;

    QFile::copy(currentItem->fileInfo().absoluteFilePath(), pathForSave);
}

void EditorMenu::setAutoRun()
{
    if(currentItem)
    {
        //
    }
}

void EditorMenu::openInNewWindow()
{
    if(!currentItem) return;
    if(QWidget *w = currentItem->widget())
    {
        w->setParent(nullptr);
        w->show();
    }
}

void EditorMenu::closeProcess()
{
    if(TreeScriptItem *item = static_cast<TreeScriptItem*>(currentItem))
    {
        item->process->close();
    }
}

void EditorMenu::commentOutScript()
{
    if(TreeScriptItem *item = static_cast<TreeScriptItem*>(currentItem))
    {
        if(!item->editor) return;
        item->editor->reverseSelectedCommentState();
    }
}

void EditorMenu::emitSaveAsTemplate()
{
    if(TreeScriptItem *item = static_cast<TreeScriptItem*>(currentItem))
    {
        if(!item->editor) return;
        emit saveAsTemplateRequested(item->editor->toPlainText());
    }
}








GnuplotMenu::GnuplotMenu(const QString &title, QWidget *parent)
    : QMenu(title, parent)
    , aScriptTemplate(new QAction("Script Template", this))
    , aGnuplotSetting(new QAction("Gnuplot Setting", this))
    , aHelpDocument(new QAction("Help Document", this))
{
    addAction(aScriptTemplate);
    addAction(aGnuplotSetting);
    addAction(aHelpDocument);

    aScriptTemplate->setShortcut(QKeySequence("Ctrl+T"));
    aGnuplotSetting->setShortcut(QKeySequence("Ctrl+G"));
    aHelpDocument->setShortcut(QKeySequence("F2"));

    connect(aScriptTemplate, &QAction::triggered, this, &GnuplotMenu::showScriptTemplateRequested);
    connect(aGnuplotSetting, &QAction::triggered, this, &GnuplotMenu::showGnuplotSettingRequested);
    connect(aHelpDocument, &QAction::triggered, this, &GnuplotMenu::showGnuplotHelpRequested);
}







ViewMenu::ViewMenu(const QString &title, QWidget *parent)
    : QMenu(title, parent)
    , aSplitVertically(new QAction("Split Vertically", this))
    , aSplitHorizontally(new QAction("Split Horizontally", this))
    , aUnsplit(new QAction("Unsplit", this))
    , aRemoveAllStackedEditor(new QAction("Remove All Stacked Editor", this))
    , aClearOutputWidget(new QAction("Clear Output Window", this))
    , aClearConsoleWidget(new QAction("Clear Console Window", this))
    , aEditorLayoutSetting(new QAction("Editor Layout", this))
{
    addAction(aSplitVertically);
    addAction(aSplitHorizontally);
    addAction(aUnsplit);
    addAction(aRemoveAllStackedEditor);
    addSeparator();
    addAction(aClearOutputWidget);
    addAction(aClearConsoleWidget);
    addSeparator();
    addAction(aEditorLayoutSetting);

    aSplitVertically->setIcon(QIcon(QPixmap(":/icon/icon_separatev")));
    aSplitHorizontally->setIcon(QIcon(QPixmap(":/icon/icon_separateh")));
    aUnsplit->setIcon(QIcon(QPixmap(":/icon/icon_closewidget")));

    aEditorLayoutSetting->setShortcut(QKeySequence("Ctrl+E"));

    connect(aSplitVertically, &QAction::triggered, this, &ViewMenu::splitVerticallyRequested);
    connect(aSplitHorizontally, &QAction::triggered, this, &ViewMenu::splitHorizontallyRequested);
    connect(aUnsplit, &QAction::triggered, this, &ViewMenu::unsplitRequested);
    connect(aRemoveAllStackedEditor, &QAction::triggered, this, &ViewMenu::removeAllStackedEditorRequested);
    connect(aClearOutputWidget, &QAction::triggered, this, &ViewMenu::clearOutputWidgetRequested);
    connect(aClearConsoleWidget, &QAction::triggered, this, &ViewMenu::clearConsoleWidgetRequested);
    connect(aEditorLayoutSetting, &QAction::triggered, this, &ViewMenu::showEditorSettingRequested);
}







HelpMenu::HelpMenu(const QString &title, QWidget *parent)
    : QMenu(title, parent)
    , aLogWindow(new QAction("Log", this))
    , aReboot(new QAction("Reboot", this))
{
    addAction(aLogWindow);
    addAction(aReboot);

    connect(aReboot, &QAction::triggered, this, &HelpMenu::rebootRequested);
}
















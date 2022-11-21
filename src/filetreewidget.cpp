/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "filetreewidget.h"
#include <QApplication>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include "imagedisplay.h"




QThread TreeFileItem::iothread = QThread();
QHash<QString, TreeScriptItem::ReadType> TreeScriptItem::suffix = QHash<QString, TreeScriptItem::ReadType>();
QHash<QString, TreeSheetItem::ReadType> TreeSheetItem::suffix = QHash<QString, TreeSheetItem::ReadType>();
QHash<QString, TreeFileItem*> TreeFileItem::list = QHash<QString, TreeFileItem*>();
QStringList FileTreeWidget::fileFilter = QStringList();





void TreeFileItem::setFileIcon()
{
    switch (FileTreeWidget::TreeItemType(type()))
    {
    case FileTreeWidget::TreeItemType::Script:
        setIcon(0, scriptIcon); break;
    case FileTreeWidget::TreeItemType::Sheet:
        setIcon(0, sheetIcon); break;
    case FileTreeWidget::TreeItemType::Image:
        setIcon(0, imageIcon); break;
    case FileTreeWidget::TreeItemType::Other:
        setIcon(0, otherIcon); break;
    case FileTreeWidget::TreeItemType::Dir:
    case FileTreeWidget::TreeItemType::Root:
    case FileTreeWidget::TreeItemType::ScriptFolder:
    case FileTreeWidget::TreeItemType::SheetFolder:
    case FileTreeWidget::TreeItemType::OtherFolder:
        setIcon(0, folderIcon);
        break;
    default:
        break;
    }
}

void TreeFileItem::setText(int column, const QString &text)
{
    QTreeWidgetItem::setText(column, text);
    if(column == 0)
        emit renamed(this);
}

void TreeFileItem::setSavedState(const bool isSaved)
{
    isSavedFlag = isSaved;
    emit editStateChanged(isSaved);
}














TreeScriptItem::TreeScriptItem(QTreeWidgetItem *parent, int type)
    : TreeFileItem(parent, type)
    , editor(nullptr)
    , process(nullptr)
{
}

TreeScriptItem::~TreeScriptItem()
{
    delete editor; editor = nullptr;
}

void TreeScriptItem::save()
{
    if(!editor) return; //まだ一度も選択されていない場合など

    switch(suffix.value(info.suffix()))
    {
    case ReadType::Text:
    {
        WriteTxtFile *writeTxt = new WriteTxtFile(nullptr);
        writeTxt->moveToThread(&iothread);
        connect(this, &TreeScriptItem::saveRequested, writeTxt, &WriteTxtFile::write);
        connect(writeTxt, &WriteTxtFile::finished, this, &TreeScriptItem::receiveSavedResult);
        connect(writeTxt, &WriteTxtFile::finished, writeTxt, &WriteTxtFile::deleteLater);
        break;
    }
    case ReadType::Html:
        /* htmlを読み込んで表示した後はただのtextになるため，htmlとしてセーブできない */
    default:
        emit errorCaused("Failed to save this file " + info.absoluteFilePath(),
                         BrowserWidget::MessageType::FileSystemErr);
        return;
    }

    emit saveRequested(info.absoluteFilePath(), editor->toPlainText());
}

void TreeScriptItem::load()
{
    switch(suffix.value(info.suffix()))
    {
    case ReadType::Text:
    case ReadType::Html:
    {
        ReadTxtFile *readTxt = new ReadTxtFile(nullptr);
        readTxt->moveToThread(&iothread);
        connect(this, &TreeScriptItem::loadRequested, readTxt, &ReadTxtFile::read);
        connect(readTxt, &ReadTxtFile::finished, this, &TreeScriptItem::receiveLoadedResult);
        connect(readTxt, &ReadTxtFile::finished, readTxt, &ReadTxtFile::deleteLater);
        break;
    }
    default:
        emit errorCaused("Failed to load this file " + info.absoluteFilePath(),
                         BrowserWidget::MessageType::FileSystemErr);
        return;
    }

    emit loadRequested(info.absoluteFilePath());
}

void TreeScriptItem::requestCloseProcess()
{
    emit closeProcessRequested();
}

void TreeScriptItem::receiveSavedResult(const bool& ok)
{
    setSavedState(ok);
    if(!ok)
    {
        emit errorCaused("Failed to save this file " + info.absoluteFilePath(),
                         BrowserWidget::MessageType::FileSystemErr);
    }
}

void TreeScriptItem::receiveLoadedResult(const QString& text, const bool& ok)
{
    if(ok)
    {
        switch(suffix.value(info.suffix()))
        {
        case ReadType::Text:
        {
            editor->setPlainText(text);
            break;
        }
        case ReadType::Html:
        {
            editor->clear();
            editor->appendHtml(text);
            break;
        }
        default:
            return;
        }

        setSavedState(true); //データをセットしてから

        return;
    }

    emit errorCaused("Failed to load this file " + info.absoluteFilePath(),
                     BrowserWidget::MessageType::FileSystemErr);
}












TreeSheetItem::~TreeSheetItem()
{
    delete table; table = nullptr;
}

void TreeSheetItem::save()
{
    if(!table) return;  //まだ一度も表示されていない場合

    switch(suffix.value(info.suffix()))
    {
    case ReadType::Csv:
    {
        WriteCsvFile *writeCsv = new WriteCsvFile(nullptr);
        writeCsv->moveToThread(&iothread);
        connect(this, &TreeSheetItem::saveRequested, writeCsv, &WriteCsvFile::write);
        connect(writeCsv, &WriteCsvFile::finished, this, &TreeSheetItem::receiveSavedResult);
        connect(writeCsv, &WriteCsvFile::finished, writeCsv, &WriteCsvFile::deleteLater);
        break;
    }
    case ReadType::Tsv:
    {
        WriteTsvFile *writeTsv = new WriteTsvFile(nullptr);
        writeTsv->moveToThread(&iothread);
        connect(this, &TreeSheetItem::saveRequested, writeTsv, &WriteTsvFile::write);
        connect(writeTsv, &WriteTsvFile::finished, this, &TreeSheetItem::receiveSavedResult);
        connect(writeTsv, &WriteTsvFile::finished, writeTsv, &WriteTsvFile::deleteLater);
        break;
    }
    default:
        emit errorCaused("Failed to save this file " + info.absoluteFilePath(),
                         BrowserWidget::MessageType::FileSystemErr);
        return;
    }

    emit saveRequested(info.absoluteFilePath(), table->getData<QString>());
}

void TreeSheetItem::load()
{
    switch(suffix.value(info.suffix()))
    {
    case ReadType::Csv:
    {
        ReadCsvFile *readCsv = new ReadCsvFile(nullptr);
        readCsv->moveToThread(&iothread);
        connect(this, &TreeSheetItem::loadRequested, readCsv, &ReadCsvFile::read);
        connect(readCsv, &ReadCsvFile::finished, this, &TreeSheetItem::receiveLoadResult);
        connect(readCsv, &ReadCsvFile::finished, readCsv, &ReadCsvFile::deleteLater);
        break;
    }
    case ReadType::Tsv:
    {
        ReadTsvFile *readTsv = new ReadTsvFile(nullptr);
        readTsv->moveToThread(&iothread);
        connect(this, &TreeSheetItem::loadRequested, readTsv, &ReadTsvFile::read);
        connect(readTsv, &ReadTsvFile::finished, this, &TreeSheetItem::receiveLoadResult);
        connect(readTsv, &ReadTsvFile::finished, readTsv, &ReadTsvFile::deleteLater);
        break;
    }
    default:
        emit errorCaused("Failed to load this file " + info.absoluteFilePath(),
                         BrowserWidget::MessageType::FileSystemErr);
        return;
    }

    emit loadRequested(info.absoluteFilePath());
}

void TreeSheetItem::receiveSavedResult(const bool& ok)
{
    setSavedState(ok);
    if(!ok)
    {
        emit errorCaused("Failed to save this file " + info.absoluteFilePath(),
                         BrowserWidget::MessageType::FileSystemErr);
    }
}

void TreeSheetItem::receiveLoadResult(const QList<QList<QString> >& data, const bool& ok)
{
    if(ok)
    {
        table->setData(data);
        setSavedState(true);  //データをセットしてから
    }
    else
    {
        emit errorCaused("Failed to load this file " + info.absoluteFilePath(),
                         BrowserWidget::MessageType::FileSystemErr);
    }
}






TreeImageItem::~TreeImageItem()
{
    if(imageDisplay)
    {
        imageDisplay->deleteLater();
        imageDisplay = nullptr;
    }
}















/* Qtのファイルシステム系の関数はすべて、パスの区切りに'/'を使用している。
 * しかし、QDir::separator()はWindowsでは'\'を返すため、QDir::separator()を使わずに'/'を使った方が良い
 *
 * FileTreeのアイテムはすべてTreeFileItemかその派生を使う。アイコンがコンストラクタで設定される。
 * QTreeWidgetItemを直接使わない。
 */

FileTreeWidget::FileTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
    , treeModel(FileTreeModel::FileSystem)
    , rootTreeItem(nullptr)
    , dirWatcher(new QFileSystemWatcher(this))
    , fileMenu(nullptr)
    , dirMenu(nullptr)
{
    TreeFileItem::iothread.start();

    setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(this, &FileTreeWidget::customContextMenuRequested, this, &FileTreeWidget::onCustomContextMenu);
    initializeContextMenu();

    connect(dirWatcher, &QFileSystemWatcher::directoryChanged, this, &FileTreeWidget::updateFileTree);
}

FileTreeWidget::~FileTreeWidget()
{
    TreeFileItem::iothread.quit();
    TreeFileItem::iothread.wait();
}

void FileTreeWidget::initializeContextMenu()
{
        if(!fileMenu) delete fileMenu;
        if(!dirMenu) delete dirMenu;
        if(!rootMenu) delete rootMenu;

        fileMenu = new QMenu(this);
        dirMenu = new QMenu(this);
        rootMenu = new QMenu(this);

        QAction *actRename = new QAction("Rename", fileMenu);
        QAction *actRemove = new QAction("Remove", fileMenu);
        QAction *actExport = new QAction("Export", fileMenu);
        QAction *actAdd = new QAction("Add", dirMenu);
        QAction *actNew = new QAction("New", dirMenu);

        connect(actRename, &QAction::triggered, this, &FileTreeWidget::renameFile);
        connect(actRemove, &QAction::triggered, this, &FileTreeWidget::removeFile);
        connect(actExport, &QAction::triggered, this, &FileTreeWidget::exportFile);
        connect(actAdd, &QAction::triggered, this, &FileTreeWidget::addFile);
        connect(actNew, &QAction::triggered, this, &FileTreeWidget::newFile);

        fileMenu->addAction(actRename);
        fileMenu->addAction(actRemove);
        fileMenu->addAction(actExport);

        dirMenu->addAction(actAdd);
        dirMenu->addAction(actNew);
        dirMenu->addAction(actRename);
        dirMenu->addAction(actRemove);

        rootMenu->addAction(actAdd);
        rootMenu->addAction(actNew);
}

void FileTreeWidget::onCustomContextMenu(const QPoint& pos)
{
    if(selectedItems().count() < 1) return;

    switch(TreeItemType(selectedItems().at(0)->type()))
    {
    case TreeItemType::Script:
    case TreeItemType::Sheet:
    case TreeItemType::Other:
    {
        fileMenu->exec(viewport()->mapToGlobal(pos));
        break;
    }
    case TreeItemType::Dir:
    {
        dirMenu->exec(viewport()->mapToGlobal(pos));
        break;
    }
    case TreeItemType::Root:
    {
        rootMenu->exec(viewport()->mapToGlobal(pos));
        break;
    }
    case TreeItemType::ScriptFolder:
    case TreeItemType::SheetFolder:
    case TreeItemType::OtherFolder:
    {
        rootMenu->exec(viewport()->mapToGlobal(pos));
        break;
    }
    default:
        break;
    }
}






void FileTreeWidget::setTreeModel(const int type)
{
    treeModel = FileTreeModel(type);
    loadFileTree();
}

/* セーブしてからloadしないと保存されないことに注意 */
void FileTreeWidget::loadFileTree()
{
    /* treeをクリア */
    clear(); //子TreeItemはdeleteされる(nullptrにはなっていない)

    TreeFileItem::list.clear();

    setHeaderHidden(true);

    rootTreeItem = new TreeFileItem(this, (int)TreeItemType::Root);
    rootTreeItem->setText(0, folderPath.sliced(folderPath.lastIndexOf('/')));
    rootTreeItem->setToolTip(0, folderPath);
    rootTreeItem->info = QFileInfo(folderPath);
    TreeFileItem::list.insert(folderPath, rootTreeItem);

    switch(treeModel)
    {
    case FileTreeModel::FileSystem:
    {
        updateFileSystemModelTree(folderPath, rootTreeItem);
        break;
    }
    case FileTreeModel::Gnuplot:
    {
        scriptFolderItem = new TreeFileItem(rootTreeItem, (int)TreeItemType::ScriptFolder);
        sheetFolderItem = new TreeFileItem(rootTreeItem, (int)TreeItemType::SheetFolder);
        otherFolderItem = new TreeFileItem(rootTreeItem, (int)TreeItemType::OtherFolder);

        scriptFolderItem->setText(0, "Script");
        sheetFolderItem->setText(0, "Sheet");
        otherFolderItem->setText(0, "Other");

        updateGnuplotModelTree(folderPath);
        break;
    }
    default:
        break;
    }
}

void FileTreeWidget::updateGnuplotModelTree(const QString &path)
{
    /* fileFilterをファイルにのみ適用するために、fileとdirでQDirを分ける */

    /* ファイル */
    QDir dirForFiles(path);
    dirForFiles.setNameFilters(fileFilter);
    const QList<QFileInfo> fileInfoList = dirForFiles.entryInfoList(QDir::Filter::Files);

    for(const QFileInfo& info : fileInfoList)
    {
        const QString absPath = info.absoluteFilePath();

        if(TreeFileItem::list.contains(absPath)) continue;

        TreeFileItem *item;

        if(TreeScriptItem::suffix.contains(info.suffix()))
            item = new TreeScriptItem(scriptFolderItem, (int)TreeItemType::Script);
        else if(TreeSheetItem::suffix.contains(info.suffix()))
            item = new TreeSheetItem(sheetFolderItem, (int)TreeItemType::Sheet);
        else if(ImageDisplay::isValidExtension(info.suffix()))
            item = new TreeImageItem(otherFolderItem, (int)TreeItemType::Image);
        else
            item = new TreeFileItem(otherFolderItem, (int)TreeItemType::Other);

        const QString rootFolderName = rootTreeItem->info.fileName();
        item->setText(0, absPath.sliced(absPath.lastIndexOf(rootFolderName) + rootFolderName.count() + 1)); //作業ディレクトリからの相対パスを表示する
        item->setToolTip(0, absPath);
        item->info = info;
        TreeFileItem::list.insert(absPath, item);
    }


    /* ディレクトリ */
    QDir dirForDir(path);
    const QList<QFileInfo> dirInfoList = dirForDir.entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::Dirs);

    for(const QFileInfo& info : dirInfoList)
    {
        const QString absPath = info.absoluteFilePath();

        dirWatcher->addPath(absPath);

        //再帰的にサブディレクトリについても設定する
        updateGnuplotModelTree(absPath);
    }
}

void FileTreeWidget::updateFileSystemModelTree(const QString &path, QTreeWidgetItem *parent)
{
    /* fileFilterをファイルにのみ適用するために、fileとdirでQDirを分ける */

    /* ファイル */
    QDir dirForFiles(path);
    dirForFiles.setNameFilters(fileFilter);
    const QList<QFileInfo> fileInfoList = dirForFiles.entryInfoList(QDir::Filter::Files);

    for(const QFileInfo& info : fileInfoList)
    {
        const QString absPath = info.absoluteFilePath();

        if(TreeFileItem::list.contains(absPath)) continue;

        TreeFileItem *item;

        if(TreeScriptItem::suffix.contains(info.suffix()))
            item = new TreeScriptItem(parent, (int)TreeItemType::Script);
        else if(TreeSheetItem::suffix.contains(info.suffix()))
            item = new TreeSheetItem(parent, (int)TreeItemType::Sheet);
        else if(ImageDisplay::isValidExtension(info.suffix()))
            item = new TreeImageItem(parent, (int)TreeItemType::Image);
        else
            item = new TreeFileItem(parent, (int)TreeItemType::Other);

        item->setText(0, info.fileName());
        item->setToolTip(0, absPath);
        item->info = info;
        TreeFileItem::list.insert(absPath, item);
    }


    /* ディレクトリ */
    QDir dirForDir(path);
    const QList<QFileInfo> dirInfoList = dirForDir.entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::Dirs);

    for(const QFileInfo& info : dirInfoList)
    {
        const QString absPath = info.absoluteFilePath();

        if(!TreeFileItem::list.contains(absPath))
        {
            TreeFileItem *item = new TreeFileItem(parent, (int)TreeItemType::Dir);
            item->setText(0, info.fileName());
            item->setToolTip(0, absPath);
            item->info = info;
            TreeFileItem::list.insert(absPath, item);
        }

        dirWatcher->addPath(absPath);

        //再帰的にサブディレクトリについても設定する
        updateFileSystemModelTree(absPath, TreeFileItem::list.value(absPath));
    }
}

void FileTreeWidget::updateFileTree()
{
    switch(treeModel)
    {
    case FileTreeModel::FileSystem:
        updateFileSystemModelTree(folderPath, rootTreeItem);
        break;
    case FileTreeModel::Gnuplot:
        updateGnuplotModelTree(folderPath);
        break;
    default:
        break;
    }
}

void FileTreeWidget::setFolderPath(const QString& folderPath)
{
    saveAllFile();

    /* 以前の監視対象のディレクトリをクリアする */
    const QStringList previousDirList = dirWatcher->directories();
    if(!previousDirList.isEmpty())
        dirWatcher->removePaths(previousDirList);

    this->folderPath = folderPath;
    dirWatcher->addPath(folderPath);
    loadFileTree();
}







void FileTreeWidget::saveAllFile()
{
    foreach(TreeFileItem *item, TreeFileItem::list)
    {
        if(!item->isSaved())
            item->save();
    }
}

void FileTreeWidget::saveAndLoad()
{
    saveAllFile();
    loadFileTree();
}

/* ローカルのフォルダーを選択し、選択されたフォルダーごと、再帰的にコピーしてもってくる */
void FileTreeWidget::addFolder()
{
    /* フォルダー選択 */
    const QString& folder = QFileDialog::getExistingDirectory(this);

    if(folder.isEmpty()) return;

    /* 再帰的にコピー */
    const QString toPath = folderPath + '/' + folder.sliced(folder.lastIndexOf('/') + 1);
    QDir dir(toPath);
    if(dir.mkdir(toPath))
        copyDirectoryRecursively(folder, toPath);
    else
        QMessageBox::critical(this, "Error", "Could not add a folder.");
}

/* 作業ディレクトリごと指定したディレクトリ下に保存する */
void FileTreeWidget::saveFolder()
{
    /* フォルダー選択 */
    const QString& folder = QFileDialog::getExistingDirectory(this);

    if(folder.isEmpty()) return;

    /* 再帰的にコピー */
    const QString toPath = folder + '/' + folderPath.sliced(folderPath.lastIndexOf('/') + 1);
    QDir dir(toPath);
    if(dir.mkdir(toPath))
        copyDirectoryRecursively(folderPath, toPath);
    else
        QMessageBox::critical(this, "Error", "Could not save a folder.");
}

void FileTreeWidget::copyDirectoryRecursively(const QString &fromPath, const QString &toPath)
{
    /* ファイル */
    QDir dirForFiles(fromPath);
    dirForFiles.setNameFilters(fileFilter);
    const QList<QFileInfo> fileInfoList = dirForFiles.entryInfoList(QDir::Filter::Files);

    for(const QFileInfo& info : fileInfoList)
    {
        const QString absPath = info.absoluteFilePath();         //コピー元のパス
        const QString makePath = toPath + '/' + info.fileName(); //コピー先のパス

        const bool ok = QFile::copy(absPath, makePath);
        if(!ok) emit errorCaused("Could not copy a file \"" + absPath + "\".", BrowserWidget::MessageType::FileSystemErr);
    }


    /* ディレクトリ */
    QDir dirForDir(fromPath);
    const QList<QFileInfo> dirInfoList = dirForDir.entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::Dirs);

    for(const QFileInfo& info : dirInfoList)
    {
        const QString absPath = info.absoluteFilePath();         //コピー元のパス
        const QString makePath = toPath + '/' + info.fileName(); //コピー先のパス

        QDir dir(toPath);
        const bool ok = dir.mkdir(makePath);
        if(!ok) emit errorCaused("Could not copy a directory \"" + absPath + "\".", BrowserWidget::MessageType::FileSystemErr);

        copyDirectoryRecursively(absPath, makePath);
    }
}









void FileTreeWidget::renameFile()
{
    if(selectedItems().count() < 1) return;

    TreeFileItem *item = static_cast<TreeFileItem*>(selectedItems().at(0));

    if(!item) return;

    const qsizetype dotIndex = item->info.fileName().lastIndexOf('.');
    const QString oldFileName = (dotIndex == qsizetype(-1)) ? item->info.fileName()
                                                            : item->info.fileName().first(dotIndex);

    QString newFileName;

    for(;;)
    {
        bool ok = false;
        newFileName = QInputDialog::getText(this, "Rename", "Enter the new name.", QLineEdit::EchoMode::Normal, oldFileName, &ok);

        if(!ok || newFileName.isEmpty()) return;

        if(newFileName.contains('.'))
            QMessageBox::critical(this, "Error", "Do not include the suffix.");
        else
            break;
    }

    const QString newAbsoluteFilePath = item->info.absoluteFilePath().replace(oldFileName, newFileName);
    QDir dir(item->info.absolutePath());
    if(!dir.rename(item->info.absoluteFilePath(), newAbsoluteFilePath))
    {
        emit errorCaused("failed to rename the file.", BrowserWidget::MessageType::FileSystemErr);
        return;
    }

    TreeFileItem::list.remove(item->info.absoluteFilePath());
    TreeFileItem::list.insert(newAbsoluteFilePath, item);
    item->info.setFile(newAbsoluteFilePath);
    item->setText(0, item->info.fileName());
    item->setToolTip(0, newAbsoluteFilePath);
}

void FileTreeWidget::removeFile()
{
    if(selectedItems().count() < 1) return;

    TreeFileItem *item = static_cast<TreeFileItem*>(selectedItems().at(0));

    if(!item) return;

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Remove", "Do you remove this \"" + item->info.fileName() + "\"??",
                                                              QMessageBox::Yes | QMessageBox::No);
    if(reply != QMessageBox::Yes) return;

    bool ok = false;
    const QString absPath = item->info.absoluteFilePath();
    if(item->type() == (int)TreeItemType::Dir)
    {   //ディレクトリの場合の削除
        dirWatcher->removePath(absPath);
        QDir dir(absPath);
        ok = dir.removeRecursively();
        if(!ok)
            dirWatcher->addPath(absPath);
    }
    else
    {   //ファイルの場合の削除
        QDir dir(item->info.absolutePath());
        ok = dir.remove(absPath);
    }

    if(!ok)
    {
        emit errorCaused("Failed to remove the file : " + item->info.fileName(), BrowserWidget::MessageType::FileSystemErr);
        return;
    }

    TreeFileItem::list.remove(item->info.absoluteFilePath());
    selectedItems().takeAt(0)->parent()->removeChild(item);
    delete item; item = nullptr;
}

void FileTreeWidget::exportFile()
{
    if(selectedItems().count() < 1) return;

    /* 保存するフォルダーパスをダイアログから取得 */
    const QString pathForSave = QFileDialog::getExistingDirectory(this);

    if(pathForSave.isEmpty()) return;

    TreeFileItem *item = static_cast<TreeFileItem*>(selectedItems().at(0));

    if(!item) return;

    item->save();

    const bool ok = QFile::copy(item->info.absoluteFilePath(),
                                pathForSave + '/' + item->info.fileName());

    if(!ok) emit errorCaused("Could not copy a file \"" + item->info.fileName() + "\"", BrowserWidget::MessageType::FileSystemErr);

    /* 削除するか確認する */
    removeFile();
}

void FileTreeWidget::addFile()
{
    if(selectedItems().count() < 1) return;

    TreeFileItem *item = static_cast<TreeFileItem*>(selectedItems().at(0));

    if(!item) return;

    /* 選択されたフォルダーによって，ダイアログのフィルターを設定する */
    QString nameFilter;
    switch((TreeItemType)item->type())
    {
    case TreeItemType::ScriptFolder:
        nameFilter += "Script files (";
        foreach(const QString& suffix, TreeScriptItem::suffix.keys())
            nameFilter += "*." + suffix + " ";
        nameFilter += ")";
        break;
    case TreeItemType::SheetFolder:
        nameFilter += "Sheet files (";
        foreach(const QString& suffix, TreeSheetItem::suffix.keys())
            nameFilter += "*." + suffix + " ";
        nameFilter += ")";
        break;
    default:
        break;
    }

    /* ファイルを選択するダイアログ。複数選択可能 */
    QFileDialog fileDialog(this);
    fileDialog.setFileMode(QFileDialog::FileMode::ExistingFiles);
    fileDialog.setNameFilter(nameFilter);
    if(!fileDialog.exec()) return;
    const QStringList filePathList = fileDialog.selectedFiles();

    for(const QString& filePath : filePathList)
    {
        if(!TreeFileItem::list.contains(filePath))
        {
            const QString fileName = filePath.sliced(filePath.lastIndexOf('/') + 1);
            const bool ok = QFile::copy(filePath, item->info.absoluteFilePath() + '/' + fileName);

            /* コピーしてこれば、自動的にdirWatcher::directoryChanged() --> updateFileTree() によってTreeに追加される */

            if(!ok) QMessageBox::critical(this, "Error", "Could not copy the file \"" + filePath + "\".");
        }
        else
            QMessageBox::critical(this, "Error", "Same name file \"" + filePath + "\" already exists.");
    }
}

void FileTreeWidget::newFile()
{
    if(selectedItems().count() < 1) return;

    TreeFileItem *item = static_cast<TreeFileItem*>(selectedItems().at(0));
    const QString folderPath = item->info.absoluteFilePath();

    /* ファイル名を入力する際のデフォルトテキストを拡張子によって変更 */
    QString defaultFileName = "";
    switch((TreeItemType)item->type())
    {
    case TreeItemType::ScriptFolder:
        if(TreeScriptItem::suffix.count() > 0)
            defaultFileName = "." + TreeScriptItem::suffix.begin().key();
        break;
    case TreeItemType::SheetFolder:
        if(TreeSheetItem::suffix.count() > 0)
            defaultFileName = "." + TreeSheetItem::suffix.begin().key();
        break;
    default:
        break;
    }

    if(!item) return;

    /* 新規ファイルの名前を取得するダイアログ */
    QString newFileName;
    for(;;)
    {
        bool ok = false;
        newFileName = QInputDialog::getText(this, "FileTree", "Enter the new file name.", QLineEdit::EchoMode::Normal, defaultFileName, &ok);

        if(!ok || newFileName.isEmpty()) return;

        if(TreeFileItem::list.contains(folderPath + '/' + newFileName))
            QMessageBox::critical(this, "Error", "Same name file already exists.");
        else
            break;
    }

    bool ok = false;
    if(newFileName.contains('.'))
    {
        QFile file(folderPath + '/' + newFileName);
        ok = file.open(QIODevice::OpenModeFlag::NewOnly);
        file.close();
    }
    else
    {
        QDir dir(folderPath);
        ok = dir.mkdir(folderPath + '/' + newFileName);
    }

    if(!ok)
        QMessageBox::critical(this, "Error", "Failed to create the new file.");

    /* ファイルが作成されれば、dirWatcherのdirectoryChanged() --> updateFileTree() によってTreeに追加される */
}






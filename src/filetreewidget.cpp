#include "filetreewidget.h"
#include <QApplication>

QStringList TreeScriptItem::suffix = QStringList() << "txt";
QStringList TreeSheetItem::suffix = QStringList() << "csv";
QHash<QString, TreeFileItem*> TreeFileItem::list = QHash<QString, TreeFileItem*>();

FileTreeWidget::FileTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
    , treeModel(FileTreeModel::FileSystem)
    , rootTreeItem(nullptr)
    , dirWatcher(new QFileSystemWatcher(this))
    , fileMenu(nullptr)
    , dirMenu(nullptr)
{
    setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(this, &FileTree::customContextMenuRequested, this, &FileTreeWidget::onCustomContextMenu);
    initializeContextMenu();

    connect(dirWatcher, &QFileSystemWatcher::directoryChanged, this, &FileTreeWidget::updateFileTree);

    connect(this, &FileTree::itemDoubleClicked, this, &FileTreeWidget::selectItem);

    setFolderPath(QApplication::applicationDirPath() + "/gnuplot-project"); //DEBUG
}

void FileTreeWidget::initializeContextMenu()
{
    {
        if(!fileMenu) delete fileMenu;

        fileMenu = new QMenu(this);

        QAction *actRename = new QAction("Rename", fileMenu);
        QAction *actRemove = new QAction("Remove", fileMenu);
        QAction *actExport = new QAction("Export", fileMenu);

        fileMenu->addAction(actRename);
        fileMenu->addAction(actRemove);
        fileMenu->addAction(actExport);

        connect(actRename, &QAction::triggered, this, &FileTreeWidget::renameFile);
        connect(actRemove, &QAction::triggered, this, &FileTreeWidget::removeFile);
        connect(actExport, &QAction::triggered, this, &FileTreeWidget::exportFile);
    }

    {
        if(!dirMenu) delete dirMenu;

        dirMenu = new QMenu(this);

        QAction *actAdd = new QAction("Add", dirMenu);
        QAction *actNew = new QAction("New", dirMenu);
        QAction *actRename = new QAction("Rename", dirMenu);
        QAction *actRemove = new QAction("Remove", dirMenu);

        dirMenu->addAction(actAdd);
        dirMenu->addAction(actNew);
        dirMenu->addAction(actRename);
        dirMenu->addAction(actRemove);

        connect(actAdd, &QAction::triggered, this, &FileTreeWidget::addFile);
        connect(actNew, &QAction::triggered, this, &FileTreeWidget::newFile);
        connect(actRename, &QAction::triggered, this, &FileTreeWidget::renameFile);
        connect(actRemove, &QAction::triggered, this, &FileTreeWidget::removeFile);
    }
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
    case TreeItemType::ScriptFolder:
    case TreeItemType::SheetFolder:
    case TreeItemType::OtherFolder:
        break;
    default:
        break;
    }
}

void FileTreeWidget::selectItem(QTreeWidgetItem *item, int)
{
    switch(TreeItemType(item->type()))
    {
    case TreeItemType::Script:
        emit scriptSelected(static_cast<TreeScriptItem*>(item));
        break;
    case TreeItemType::Sheet:
        emit sheetSelected(static_cast<TreeSheetItem*>(item));
        break;
    case TreeItemType::Other:
        emit otherSelected(static_cast<TreeFileItem*>(item));
        break;
    case TreeItemType::Dir:
    case TreeItemType::ScriptFolder:
    case TreeItemType::SheetFolder:
    case TreeItemType::OtherFolder:
        break;
    default:
        break;
    }
}






void FileTreeWidget::loadFileTree()
{
    /* treeをクリア */
    clear();

    TreeFileItem::list.clear();

    setHeaderHidden(true);

    rootTreeItem = new TreeFileItem(this, (int)TreeItemType::Dir);
    rootTreeItem->setText(0, folderPath.sliced(folderPath.lastIndexOf('/')));
    rootTreeItem->setToolTip(0, folderPath);
    rootTreeItem->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
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
        scriptFolderItem = new QTreeWidgetItem(rootTreeItem, (int)TreeItemType::ScriptFolder);
        sheetFolderItem = new QTreeWidgetItem(rootTreeItem, (int)TreeItemType::SheetFolder);
        otherFolderItem = new QTreeWidgetItem(rootTreeItem, (int)TreeItemType::OtherFolder);

        scriptFolderItem->setText(0, "Script");
        sheetFolderItem->setText(0, "Sheet");
        otherFolderItem->setText(0, "Other");

        scriptFolderItem->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
        sheetFolderItem->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
        otherFolderItem->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));

        updateGnuplotModelTree(folderPath);
        break;
    }
    default:
        break;
    }
}

void FileTreeWidget::updateGnuplotModelTree(const QString &path)
{
    QDir dir(path);
    const QList<QFileInfo> infoList = dir.entryInfoList(QDir::Filter::NoDotAndDotDot |
                                                        QDir::Filter::Dirs |
                                                        QDir::Filter::Files);

    for(const QFileInfo& info : infoList)
    {
        const QString absPath = info.absoluteFilePath();

        if(info.isFile())
        {
            if(TreeFileItem::list.contains(absPath)) continue;

            TreeFileItem *item;

            if(TreeScriptItem::suffix.contains(info.suffix()))
                item = new TreeScriptItem(scriptFolderItem, (int)TreeItemType::Script);
            else if(TreeSheetItem::suffix.contains(info.suffix()))
                item = new TreeSheetItem(sheetFolderItem, (int)TreeItemType::Sheet);
            else
                item = new TreeFileItem(otherFolderItem, (int)TreeItemType::Other);

            item->setText(0, info.fileName());
            item->info = info;
            TreeFileItem::list.insert(absPath, item);
        }
        else if(info.isDir())
        {
            dirWatcher->addPath(absPath);

            //再帰的にサブディレクトリについても設定する
            updateGnuplotModelTree(absPath);
        }
    }
}

void FileTreeWidget::updateFileSystemModelTree(const QString &path, QTreeWidgetItem *parent)
{
    QDir dir(path);
    const QList<QFileInfo> infoList = dir.entryInfoList(QDir::Filter::NoDotAndDotDot |
                                                        QDir::Filter::Dirs |
                                                        QDir::Filter::Files);

    for(const QFileInfo& info : infoList)
    {
        const QString absPath = info.absoluteFilePath();

        if(info.isFile())
        {   
            if(TreeFileItem::list.contains(absPath)) continue;

            TreeFileItem *item;

            if(TreeScriptItem::suffix.contains(info.suffix()))
                item = new TreeScriptItem(parent, (int)TreeItemType::Script);
            else if(TreeSheetItem::suffix.contains(info.suffix()))
                item = new TreeSheetItem(parent, (int)TreeItemType::Sheet);
            else
                item = new TreeFileItem(parent, (int)TreeItemType::Other);

            item->setText(0, info.fileName());
            item->info = info;
            TreeFileItem::list.insert(absPath, item);
        }
        else if(info.isDir())
        {
            if(!TreeFileItem::list.contains(absPath))
            {
                TreeFileItem *item = new TreeFileItem(parent, (int)TreeItemType::Dir);
                item->setText(0, info.fileName());
                item->info = info;
                item->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
                TreeFileItem::list.insert(absPath, item);
            }

            dirWatcher->addPath(absPath);

            //再帰的にサブディレクトリについても設定する
            updateFileSystemModelTree(absPath, TreeFileItem::list.value(absPath));
        }
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
    //saveAllScript();
    //saveAllSheet();

    /* 以前の監視対象のディレクトリをクリアする */
    const QStringList previousDirList = dirWatcher->directories();
    if(!previousDirList.isEmpty())
        dirWatcher->removePaths(previousDirList);

    this->folderPath = folderPath;
    dirWatcher->addPath(folderPath);
    loadFileTree();
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
}

void FileTreeWidget::removeFile()
{
    if(selectedItems().count() < 1) return;

    TreeFileItem *item = static_cast<TreeFileItem*>(selectedItems().at(0));

    if(!item) return;

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Remove", "Do you remove this \"" + item->text(0) + "\"??",
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
        emit errorCaused("Failed to remove the file : " + item->text(0), BrowserWidget::MessageType::FileSystemErr);
        return;
    }

    TreeFileItem::list.remove(item->info.absoluteFilePath());
    selectedItems().takeAt(0)->parent()->removeChild(selectedItems().takeAt(0));
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

    removeFile();
}

void FileTreeWidget::addFile()
{
    if(selectedItems().count() < 1) return;

    TreeFileItem *item = static_cast<TreeFileItem*>(selectedItems().at(0));

    if(!item) return;

    /* ファイルを選択するダイアログ。複数選択可能 */
    const QStringList filePathList = QFileDialog::getOpenFileNames(this);

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

    if(!item) return;

    /* 新規ファイルの名前を取得するダイアログ */
    QString newFileName;
    for(;;)
    {
        bool ok = false;
        newFileName = QInputDialog::getText(this, "FileTree", "Enter the new file name.", QLineEdit::EchoMode::Normal, "", &ok);

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






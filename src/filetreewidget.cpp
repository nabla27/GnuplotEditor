#include "filetreewidget.h"
#include <QApplication>

QStringList TreeScriptItem::suffix = QStringList() << "txt";
QStringList TreeSheetItem::suffix = QStringList() << "csv";
QStringList TreeScriptItem::list = QStringList();
QStringList TreeSheetItem::list = QStringList();
QStringList TreeOtherItem::list = QStringList();
QStringList TreeDirItem::list = QStringList();

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

void FileTreeWidget::selectItem(QTreeWidgetItem *item, int column)
{

}






void FileTreeWidget::loadFileTree()
{
    /* treeをクリア */
    clear();

    setHeaderHidden(true);

    rootTreeItem = new QTreeWidgetItem(this, (int)TreeItemType::Root);
    rootTreeItem->setText(0, folderPath.sliced(folderPath.lastIndexOf('/')));
    rootTreeItem->setToolTip(0, folderPath);
    rootTreeItem->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));

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
        if(info.isFile())
        {
            const QString absPath = info.absoluteFilePath();

            if(TreeScriptItem::suffix.contains(info.suffix()))
            {
                if(TreeScriptItem::list.contains(absPath)) continue;

                TreeScriptItem *item = new TreeScriptItem(scriptFolderItem, (int)TreeItemType::Script);
                item->setText(0, info.fileName());
                item->info = info;
                TreeScriptItem::list << absPath;
            }
            else if(TreeSheetItem::suffix.contains(info.suffix()))
            {
                if(TreeSheetItem::list.contains(absPath)) continue;

                TreeSheetItem *item = new TreeSheetItem(sheetFolderItem, (int)TreeItemType::Sheet);
                item->setText(0, info.fileName());
                item->info = info;
                TreeSheetItem::list << absPath;
            }
            else
            {
                if(TreeOtherItem::list.contains(absPath)) continue;

                TreeOtherItem *item = new TreeOtherItem(otherFolderItem, (int)TreeItemType::Other);
                item->setText(0, info.fileName());
                item->info = info;
                TreeOtherItem::list << absPath;
            }
        }
        else if(info.isDir())
        {
            //再帰的にサブディレクトリについても設定する
            updateGnuplotModelTree(info.absoluteFilePath());
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
        qDebug() << absPath;
        if(info.isFile())
        {
            if(TreeScriptItem::suffix.contains(info.suffix()))
            {
                if(TreeScriptItem::list.contains(absPath)) continue;

                TreeScriptItem *item = new TreeScriptItem(parent, (int)TreeItemType::Script);
                item->setText(0, info.fileName());
                item->info = info;
                TreeScriptItem::list << absPath;
            }
            else if(TreeSheetItem::suffix.contains(info.suffix()))
            {
                if(TreeSheetItem::list.contains(absPath)) continue;

                TreeSheetItem *item = new TreeSheetItem(parent, (int)TreeItemType::Sheet);
                item->setText(0, info.fileName());
                item->info = info;
                TreeSheetItem::list << absPath;
            }
            else
            {
                if(TreeOtherItem::list.contains(absPath)) continue;

                TreeOtherItem *item = new TreeOtherItem(parent, (int)TreeItemType::Other);
                item->setText(0, info.fileName());
                item->info = info;
                TreeOtherItem::list << absPath;
            }
        }
        else if(info.isDir())
        {
            if(TreeDirItem::list.contains(absPath)) continue;

            //再帰的にサブディレクトリについても設定する
            TreeDirItem *item = new TreeDirItem(parent, (int)TreeItemType::Dir);
            item->setText(0, info.fileName());
            item->info = info;
            item->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
            updateFileSystemModelTree(info.absoluteFilePath(), item);
            TreeDirItem::list << absPath;
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

    if(this->folderPath.isEmpty())
        dirWatcher->removePath(this->folderPath);

    this->folderPath = folderPath;
    dirWatcher->addPath(folderPath);
    loadFileTree();
}

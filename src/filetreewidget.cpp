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
#include <QFileSystemWatcher>
#include <QThread>
#include <QMenu>
#include <QAction>
#include <QTimer>
#include <QMimeData>
#include <QDrag>

#include "imagedisplay.h"
#include "pdfviewer.h"
#include "gnuplot.h"
#include "tablesettingwidget.h"
#include "texteditor.h"
#include "iofile.h"
#include "standardpixmap.h"
#include "logger.h"
#include "utility.h"



QThread TreeFileItem::iothread = QThread();
QHash<QString, TreeScriptItem::ReadType> TreeScriptItem::suffix = QHash<QString, TreeScriptItem::ReadType>();
QHash<QString, TreeSheetItem::ReadType> TreeSheetItem::suffix = QHash<QString, TreeSheetItem::ReadType>();
QHash<QString, TreeFileItem*> TreeFileItem::list = QHash<QString, TreeFileItem*>();
QStringList FileTreeWidget::fileFilter = QStringList();







TreeFileItem::TreeFileItem(QTreeWidgetItem *parent, int type, const QFileInfo &info)
    : QTreeWidgetItem(parent, type)
    , info(info)
    , updateTimer(new QTimer(this))
{
    setFilePath(info.absoluteFilePath());

    updateTimer->setSingleShot(true);
    connect(updateTimer, &QTimer::timeout, this, &TreeFileItem::updated);
}

TreeFileItem::TreeFileItem(QTreeWidget *parent, int type, const QFileInfo &info)
    : QTreeWidgetItem(parent, type)
    , info(info)
{
    setFilePath(info.absoluteFilePath());
}

void TreeFileItem::setFilePath(const QString &path)
{
    info.setFile(path);

    setText(0, info.fileName());
    setToolTip(0, info.absoluteFilePath());

    emit pathChanged(path);
}

void TreeFileItem::setData(int column, int role, const QVariant &variant)
{
    if(column == 0)
    {
        switch(Qt::ItemDataRole(role))
        {
        case Qt::ItemDataRole::DisplayRole:
        {
            if(variant.toString() != info.fileName())
            {
                //setFileName()を使うべき
                qDebug() << __LINE__ << __FILE__;
            }
            break;
        }
        case Qt::ItemDataRole::ToolTipRole:
        {
            if(variant.toString() != info.absoluteFilePath())
            {
                //setFile()を使うべき
                qDebug() << __LINE__ << __FILE__;
            }
            break;
        }
        default:
            break;
        }
    }

    QTreeWidgetItem::setData(column, role, variant);
}

void TreeFileItem::setSavedState(const bool isSaved)
{
    isSavedFlag = isSaved;
    emit editStateChanged(isSaved);

    if(enableUpdateTimer && !isSaved)
    {
        updateTimer->start(updateTime);
    }
}







TreeNoCategorizedItem::TreeNoCategorizedItem(QTreeWidgetItem *parent, const QFileInfo &info)
    : TreeFileItem(parent, (int)FileTreeWidget::TreeItemType::NoCategorized, info)
{
    setIcon(0, StandardPixmap::File::normal());
}

void TreeNoCategorizedItem::save()
{
    __LOGOUT__("no supported to save.", Logger::LogLevel::Warn);
}

void TreeNoCategorizedItem::load()
{
    __LOGOUT__("no supported to load.", Logger::LogLevel::Warn);
}

void TreeNoCategorizedItem::remove()
{
    QFile file(info.absoluteFilePath());

    emit removed(file.remove());
}








TreeFolderItem::TreeFolderItem(QTreeWidgetItem *parent, const QFileInfo &info)
    : TreeFileItem(parent, (int)FileTreeWidget::TreeItemType::Dir, info)
{
    setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
}

TreeFolderItem::TreeFolderItem(QTreeWidget *parent, const QFileInfo &info)
    : TreeFileItem(parent, (int)FileTreeWidget::TreeItemType::Dir, info)
{
    setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
}

void TreeFolderItem::save()
{
    __LOGOUT__("no supported to save.", Logger::LogLevel::Warn);
}

void TreeFolderItem::load()
{
    __LOGOUT__("no supported to load.", Logger::LogLevel::Warn);
}

void TreeFolderItem::remove()
{
    //childにpdfがある場合，操作中で削除できないだろう
    QDir dir(info.absoluteFilePath());
    emit removed(dir.removeRecursively());
}









TreeCategoryItem::TreeCategoryItem(QTreeWidgetItem *parent, const QFileInfo &info, const int type)
    : TreeFileItem(parent, (int)FileTreeWidget::TreeItemType::Category, info)
    , categoryType(type)
{
    setIcon(0, StandardPixmap::File::category());
}

void TreeCategoryItem::save()
{
    __LOGOUT__("no supported to save.", Logger::LogLevel::Warn);
}

void TreeCategoryItem::load()
{
    __LOGOUT__("no supported to load.", Logger::LogLevel::Warn);
}

void TreeCategoryItem::remove()
{
    __LOGOUT__("no supported to remove.", Logger::LogLevel::Warn);
    emit removed(false);
}

int TreeCategoryItem::category()
{
    return categoryType;
}










TreeScriptItem::TreeScriptItem(QTreeWidgetItem *parent, const QFileInfo& info)
    : TreeFileItem(parent, (int)FileTreeWidget::TreeItemType::Script, info)
    , editor(new TextEdit)
    , process(new GnuplotProcess(nullptr))
{
    TreeScriptItem::load();

    connect(editor, &TextEdit::textChanged, this, &TreeScriptItem::setEdited);
    connect(process, &GnuplotProcess::standardOutputRead, logger, QOverload<const QString&, const Logger::LogLevel&>::of(&Logger::output));
    connect(process, &GnuplotProcess::aboutToExecute, editor, &TextEdit::resetErrorLineNumber);
    connect(process, &GnuplotProcess::errorCaused, editor, &TextEdit::setErrorLineNumber);

    connect(this, &TreeScriptItem::closeProcessRequested, process, &GnuplotProcess::close);
    connect(this, &TreeScriptItem::destroyed, process, &GnuplotProcess::close);
    connect(this, &TreeScriptItem::destroyed, process, &GnuplotProcess::deleteLater);
    connect(this, &TreeScriptItem::pathChanged, [this](){ if(editor) editor->setParentFolderPath(this->info.absolutePath()); });

    editor->setParentFolderPath(info.absolutePath());
    process->moveToThread(gnuplotExecutor->gnuplotThread());

    setIcon(0, StandardPixmap::File::code());
}

TreeScriptItem::~TreeScriptItem()
{
    if(editor)
    {
        delete editor;
        editor = nullptr;
    }
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
        __LOGOUT__("failed to save this file \"" + info.absoluteFilePath() + "\".", Logger::LogLevel::Error);
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
        __LOGOUT__("Failed to load this file \"" + info.absoluteFilePath() + "\".", Logger::LogLevel::Error);
        return;
    }

    emit loadRequested(info.absoluteFilePath());
}

void TreeScriptItem::remove()
{
    QFile file(info.absoluteFilePath());

    emit removed(file.remove());
}

QWidget* TreeScriptItem::widget()
{
    return editor;
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
        __LOGOUT__("Failed to save this file \"" + info.absoluteFilePath() + "\".", Logger::LogLevel::Error);
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

    __LOGOUT__("Failed to load this file \"" + info.absoluteFilePath() + "\".", Logger::LogLevel::Error);
}









TreeSheetItem::TreeSheetItem(QTreeWidgetItem *parent, const QFileInfo &info)
    : TreeFileItem(parent, (int)FileTreeWidget::TreeItemType::Sheet, info)
    , table(new TableArea(nullptr))
{
    TreeSheetItem::load();

    setIcon(0, StandardPixmap::File::document());

    connect(table->tableWidget(), &GnuplotTable::cellChanged, this, &TreeSheetItem::setEdited);
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
        __LOGOUT__("Failed to save this file \"" + info.absoluteFilePath() + "\".", Logger::LogLevel::Error);
        return;
    }

    emit saveRequested(info.absoluteFilePath(), table->tableWidget()->getData<QString>());
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
        __LOGOUT__("Fialed to load this file \"" + info.absoluteFilePath() + "\".", Logger::LogLevel::Error);
        return;
    }

    emit loadRequested(info.absoluteFilePath());
}

void TreeSheetItem::remove()
{
    QFile file(info.absoluteFilePath());

    emit removed(file.remove());
}

QWidget *TreeSheetItem::widget()
{
    return table;
}

void TreeSheetItem::receiveSavedResult(const bool& ok)
{
    setSavedState(ok);
    if(!ok)
    {
        __LOGOUT__("Fialed to save this file \"" + info.absoluteFilePath() + "\".", Logger::LogLevel::Error);
    }
}

void TreeSheetItem::receiveLoadResult(const QList<QList<QString> >& data, const bool& ok)
{
    if(ok)
    {
        table->tableWidget()->setData(data);
        setSavedState(true);  //データをセットしてから
    }
    else
    {
        __LOGOUT__("Failed to load this file \"" + info.absoluteFilePath() + "\".", Logger::LogLevel::Error);
    }
}








TreeImageItem::TreeImageItem(QTreeWidgetItem *parent, const QFileInfo &info)
    : TreeFileItem(parent, (int)FileTreeWidget::TreeItemType::Image, info)
    , imageDisplay(new ImageDisplay(nullptr))
{
    imageDisplay->setImagePath(info.absoluteFilePath());

    setIcon(0, StandardPixmap::File::image());

    connect(this, &TreeImageItem::pathChanged, imageDisplay, &ImageDisplay::setImagePath);
}

void TreeImageItem::save()
{
    __LOGOUT__("no supported to save.", Logger::LogLevel::Warn);
}

void TreeImageItem::load()
{
    imageDisplay->updateImage();
}

void TreeImageItem::remove()
{
    QFile file(info.absoluteFilePath());

    emit removed(file.remove());
}

TreeImageItem::~TreeImageItem()
{
    if(imageDisplay)
    {
        imageDisplay->deleteLater();
        imageDisplay = nullptr;
    }
}

QWidget *TreeImageItem::widget()
{
    return imageDisplay;
}










TreePdfItem::TreePdfItem(QTreeWidgetItem *parent, const QFileInfo &info)
    : TreeFileItem(parent, (int)FileTreeWidget::TreeItemType::Pdf, info)
    , viewer(new PdfViewer(nullptr))
{
    viewer->setFilePath(info.absoluteFilePath());

    setIcon(0, StandardPixmap::File::pdf());

    connect(this, &TreePdfItem::pathChanged, viewer, &PdfViewer::setFilePath);
}

void TreePdfItem::save()
{
    __LOGOUT__("no supported to save.", Logger::LogLevel::Warn);
}

void TreePdfItem::load()
{
    viewer->reload();
}

void TreePdfItem::remove()
{
    viewer->closeDocument();

    /* closeDocument()を実行してからしばらくしないとファイルを消せない．
     * emitしてしばらくしてから成功していればFileTreeから消される */
    QFile *file = new QFile(info.absoluteFilePath(), nullptr);
    QTimer *timer = new QTimer(nullptr);

    timer->setInterval(500);
    timer->setSingleShot(true);

    auto removePdfLater = [=]()
    {
        bool ok = file->remove();

        file->deleteLater();
        timer->deleteLater();

        emit removed(ok);
    };

    connect(timer, &QTimer::timeout, removePdfLater);

    timer->start();
}

TreePdfItem::~TreePdfItem()
{
    if(viewer)
    {
        viewer->deleteLater();
        viewer = nullptr;
    }
}

QWidget *TreePdfItem::widget()
{
    return viewer;
}














/* Qtのファイルシステム系の関数はすべて、パスの区切りに'/'を使用している。
 * しかし、QDir::separator()はWindowsでは'\'を返すため、QDir::separator()を使わずに'/'を使った方が良い
 *
 * FileTreeのアイテムはすべてTreeFileItemかその派生を使う。アイコンがコンストラクタで設定される。
 * QTreeWidgetItemを直接使わない。
 *
 * TreeFileItem::setText()やTreeFileItem::setToolTip()は用いずにTreeFileItem::setFileName(),TreeFileItem::setFilePath()を用いる.
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

    setAcceptDrops(true);
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
        if(!categoryMenu) delete categoryMenu;

        fileMenu = new QMenu(this);
        dirMenu = new QMenu(this);
        categoryMenu = new QMenu(this);

        QAction *actRename = new QAction("Rename", fileMenu);
        QAction *actRemove = new QAction("Remove", fileMenu);
        QAction *actExport = new QAction("Export", fileMenu);
        QAction *actAdd = new QAction("Add", dirMenu);
        QAction *actNew = new QAction("New", dirMenu);

        connect(actRename, &QAction::triggered, this, &FileTreeWidget::renameFile);
        connect(actRemove, &QAction::triggered, this, &FileTreeWidget::removeFile);
        connect(actExport, &QAction::triggered, this, &FileTreeWidget::exportFile);
        connect(actAdd, &QAction::triggered, this, &FileTreeWidget::addFileFromDialog);
        connect(actNew, &QAction::triggered, this, &FileTreeWidget::newFileFromDialog);

        fileMenu->addAction(actRename);
        fileMenu->addAction(actRemove);
        fileMenu->addAction(actExport);

        dirMenu->addAction(actAdd);
        dirMenu->addAction(actNew);
        dirMenu->addAction(actRename);
        dirMenu->addAction(actRemove);

        categoryMenu->addAction(actAdd);
        categoryMenu->addAction(actNew);
}

void FileTreeWidget::onCustomContextMenu(const QPoint& pos)
{
    if(selectedItems().count() < 1) return;

    switch(TreeItemType(selectedItems().at(0)->type()))
    {
    case TreeItemType::Script:
    case TreeItemType::Sheet:
    case TreeItemType::Image:
    case TreeItemType::Pdf:
    case TreeItemType::NoCategorized:
    {
        fileMenu->exec(viewport()->mapToGlobal(pos));
        break;
    }
    case TreeItemType::Dir:
    {
        dirMenu->exec(viewport()->mapToGlobal(pos));
        break;
    }
    case TreeItemType::Category:
        categoryMenu->exec(viewport()->mapToGlobal(pos));
        break;
    default:
        break;
    }
}




/* mimeDataに一般のクラスオブジェクトをセットするとき，QVariantを用いてvoid*型でデータ渡し，
 * objectNameにクラス名を与える．
 */
void FileTreeWidget::mousePressEvent(QMouseEvent *e)
{
    if(e->button() != Qt::LeftButton ||
       selectedItems().count() < 1)
    {
        QTreeWidget::mousePressEvent(e);
        return;
    }

    const QPointF cursorPos = viewport()->mapFromGlobal(cursor().pos().toPointF());
    TreeFileItem *item = static_cast<TreeFileItem*>(itemAt(QPoint(cursorPos.x(), cursorPos.y())));

    if(!item)
    {
        QTreeWidget::mousePressEvent(e);
        return;
    }

    QMimeData *mimeData = new QMimeData;
    QDrag *drag = new QDrag(this);

    mimeData->setParent(this);
    mimeData->setColorData(QVariant(QVariant::fromValue(static_cast<QObject*>(item))));
    mimeData->setObjectName(TreeFileItem::staticMetaObject.className());

    drag->setMimeData(mimeData);
    drag->setPixmap(item->icon(0).pixmap(25, 25));

    Qt::DropAction dropAction = drag->exec(Qt::DropAction::MoveAction);

    switch(dropAction)
    {
    case Qt::DropAction::IgnoreAction:
    {
        mimeData->deleteLater();
        drag->deleteLater();
        break;
    }
    default:
        break;
    }

    QTreeWidget::mousePressEvent(e);
}

void FileTreeWidget::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasUrls() ||
       (e->mimeData()->objectName() == QString(TreeFileItem::staticMetaObject.className()) && e->source() == this))
    {
        e->acceptProposedAction(); //一度受け入れないとdragMoveEventは呼ばれない
    }

    //QTreeWidget::dragEnterEvent()を呼び出すとFileTreeWidget::dragMoveEvent()は呼ばれない
}

void FileTreeWidget::dragMoveEvent(QDragMoveEvent *e)
{
    //dragEnterEventでdragを受け付けるobjectは既にfilterされている

    const QPointF cursorPos = viewport()->mapFromGlobal(cursor().pos().toPointF());
    TreeFileItem *item = static_cast<TreeFileItem*>(itemAt(QPoint(cursorPos.x(), cursorPos.y())));

    if(item)
    {
        clearSelection();
        item->setSelected(true);

        switch(FileTreeWidget::TreeItemType(item->type()))
        {
        case FileTreeWidget::TreeItemType::Dir:
            e->acceptProposedAction();
            return;
        default:
            break;
        }
    }

    e->setAccepted(false);
}

void FileTreeWidget::dropEvent(QDropEvent *e)
{
    const QPointF cursorPos = viewport()->mapFromGlobal(cursor().pos().toPointF());
    TreeFileItem *itemUnderCursor = static_cast<TreeFileItem*>(itemAt(QPoint(cursorPos.x(), cursorPos.y())));

    if(!itemUnderCursor) return;

    const QString parentPath = itemUnderCursor->fileInfo().absoluteFilePath();

    if(e->mimeData()->hasUrls())
    {
        addFilesFromUrls(e->mimeData()->urls(), parentPath);
    }
    else if(e->mimeData()->objectName() == QString(TreeFileItem::staticMetaObject.className()))
    {
        TreeFileItem *itemDragged = qvariant_cast<TreeFileItem*>(e->mimeData()->colorData());
        moveItem(itemDragged, parentPath);
    }
}

void FileTreeWidget::addFilesFromUrls(const QList<QUrl>& urls, const QString& parentPath)
{
    for(const QUrl& url : urls)
    {
        QFileInfo info(url.toLocalFile());
        if(info.isFile())
        {
            const bool ok = QFile::copy(info.absoluteFilePath(), parentPath + "/" + info.fileName());

            if(!ok)
            {
                __LOGOUT__("failed to copy the file " + info.absoluteFilePath() + " to " + parentPath + "/" + info.fileName(), Logger::LogLevel::Warn);
            }
        }
        else
        {
            __LOGOUT__("item that is not file cannot be dragged & copied. url(" + url.toString() + ")", Logger::LogLevel::Warn);
        }
    }
}

void FileTreeWidget::moveItem(TreeFileItem *item, const QString& parentPath)
{
    if(!item) return;

    //フォルダーの場合，自分自身に自分コピーすると無限ループになる
    if(item->fileInfo().absoluteFilePath() == parentPath) return;

    const QString newPath = parentPath + "/" + item->fileInfo().fileName();

    if(item->fileInfo().isFile())
    {
        const bool ok = QFile::copy(item->fileInfo().absoluteFilePath(), newPath);
        if(ok)
        {
            connect(item, &TreeFileItem::removed, [=](const bool ok){
                if(ok)
                    removeItemFromTree(item);
                else
                    __LOGOUT__("failed to remove the file \"" + item->fileInfo().absoluteFilePath() + "\".", Logger::LogLevel::Error);
            });

            item->remove();
        }
        else
        {
            __LOGOUT__("fialed to copy the file \"" + item->fileInfo().absoluteFilePath() + "\" to \"" + newPath + "\".", Logger::LogLevel::Warn);
        }
    }
    else if(item->fileInfo().isDir())
    {
        if(newPath.contains(item->fileInfo().absoluteFilePath()))
        {
            __LOGOUT__("Copying itself to itself results in an infinite loop", Logger::LogLevel::Warn);
            return;
        }

        QDir oldDir(item->fileInfo().absoluteFilePath());
        const bool ok = oldDir.mkdir(newPath);

        if(!ok)
        {
            __LOGOUT__("failed to make dir \"" + newPath + "\"", Logger::LogLevel::Warn);
            return;
        }

        copyDirectoryRecursively(item->fileInfo().absoluteFilePath(), newPath);

        if(!oldDir.removeRecursively())
        {
            __LOGOUT__("failed to remove dir recursively \"" + item->fileInfo().absoluteFilePath() + "\"", Logger::LogLevel::Warn);
        }
        else
        {
            removeItemFromTree(item);
        }
    }
}

void FileTreeWidget::removeItemFromTree(TreeFileItem *item)
{
    if(!item) return;

    removeItemFromList(item);

    if(QTreeWidgetItem *parentItem = item->QTreeWidgetItem::parent())
    {
        parentItem->removeChild(item);
    }
    else
    {
        __LOGOUT__("no parent item", Logger::LogLevel::Debug);
        removeItemWidget(item, 0);
    }

    delete item;
    item = nullptr;
}

void FileTreeWidget::removeItemFromList(TreeFileItem *item)
{
    if(!item) return;

    TreeFileItem::list.remove(item->fileInfo().absoluteFilePath());

    const int childCount = item->childCount();

    for(int i = 0; i < childCount; ++i)
    {
        TreeFileItem *child = static_cast<TreeFileItem*>(item->child(i));
        if(child->childCount() > 0)
        {   //フォルダ
            removeItemFromList(child);
        }
        else
        {   //ファイル
            TreeFileItem::list.remove(child->fileInfo().absoluteFilePath());
        }
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

    //rootTreeItem = new TreeFileItem(this, (int)TreeItemType::Root);
    rootTreeItem = new TreeFolderItem(this, QFileInfo(folderPath));
    //rootTreeItem->setText(0, folderPath.sliced(folderPath.lastIndexOf('/')));
    //rootTreeItem->setToolTip(0, folderPath);
    //rootTreeItem->info = QFileInfo(folderPath);
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
        scriptFolderItem = new TreeCategoryItem(rootTreeItem, QFileInfo(folderPath), (int)FileTreeWidget::TreeItemType::Script);
        sheetFolderItem = new TreeCategoryItem(rootTreeItem, QFileInfo(folderPath ), (int)FileTreeWidget::TreeItemType::Sheet);
        otherFolderItem = new TreeCategoryItem(rootTreeItem, QFileInfo(folderPath), (int)FileTreeWidget::TreeItemType::NoCategorized);

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
            item = new TreeScriptItem(scriptFolderItem, info);
        else if(TreeSheetItem::suffix.contains(info.suffix()))
            item = new TreeSheetItem(sheetFolderItem, info);
        else if(ImageDisplay::isValidExtension(info.suffix()))
            item = new TreeImageItem(otherFolderItem, info);
        else if(info.suffix() == "pdf")
            item = new TreePdfItem(otherFolderItem, info);
        else
            item = new TreeNoCategorizedItem(otherFolderItem, info);

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
            item = new TreeScriptItem(parent, info);
        else if(TreeSheetItem::suffix.contains(info.suffix()))
            item = new TreeSheetItem(parent, info);
        else if(ImageDisplay::isValidExtension(info.suffix()))
            item = new TreeImageItem(parent, info);
        else if(info.suffix() == "pdf")
            item = new TreePdfItem(parent, info);
        else
            item = new TreeNoCategorizedItem(parent, info);

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
            //TreeFileItem *item = new TreeFileItem(parent, (int)TreeItemType::Dir);
            //item->setText(0, info.fileName());
            //item->setToolTip(0, absPath);
            //item->info = info;
            TreeFolderItem *item = new TreeFolderItem(parent, info);
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

    emit folderPathChanged(folderPath);
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
    {
        copyDirectoryRecursively(folder, toPath);
    }
    else
    {
        __LOGOUT__("failed to make the directory \"" + toPath + "\".", Logger::LogLevel::Error);
        //QMessageBox::critical(this, "Error", "Could not add a folder.");
    }
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
    {
        copyDirectoryRecursively(folderPath, toPath);
    }
    else
    {
        __LOGOUT__("failed to make the directory \"" + toPath + "\".", Logger::LogLevel::Error);
        //QMessageBox::critical(this, "Error", "Could not save a folder.");
    }
}

void FileTreeWidget::openFolder()
{
    const QString& folder = QFileDialog::getExistingDirectory(this);

    if(folder.isEmpty() || folder == folderPath) return;

    setFolderPath(folder);
}

void FileTreeWidget::copyDirectoryRecursively(const QString &fromPath, const QString &toPath)
{
    if(fromPath == toPath ||
       toPath.contains(fromPath))
    {
        __LOGOUT__("copying itself to itself results in an infinite loop", Logger::LogLevel::Warn);
        return;
    }

    /* ファイル */
    QDir oldDir(fromPath);
    oldDir.setNameFilters(fileFilter);
    const QList<QFileInfo> fileInfoList = oldDir.entryInfoList(QDir::Filter::Files);

    for(const QFileInfo& info : fileInfoList)
    {
        const QString oldPath = info.absoluteFilePath();         //コピー元のパス
        const QString newPath = toPath + '/' + info.fileName(); //コピー先のパス

        const bool ok = QFile::copy(oldPath, newPath);
        if(!ok) __LOGOUT__("Could not copy a file \"" + oldPath + "\" \"" + newPath + "\".", Logger::LogLevel::Error);
    }

    /* ディレクトリ */
    QDir dirForDir(fromPath);
    const QList<QFileInfo> dirInfoList = dirForDir.entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::Dirs);

    for(const QFileInfo& info : dirInfoList)
    {
        const QString oldPath = info.absoluteFilePath();         //コピー元のパス
        const QString newPath = toPath + '/' + info.fileName(); //コピー先のパス

        QDir dir(toPath);

        const bool ok = dir.mkdir(newPath);
        if(!ok) __LOGOUT__("Could not copy a directory \"" + oldPath + "\" \"" + newPath + "\".", Logger::LogLevel::Error);

        copyDirectoryRecursively(oldPath, newPath);
    }
}









void FileTreeWidget::renameFile()
{
    if(selectedItems().count() < 1) return;

    TreeFileItem *item = static_cast<TreeFileItem*>(selectedItems().at(0));

    if(!item) return;

    //const qsizetype dotIndex = item->info.fileName().lastIndexOf('.');
    //const QString oldFileName = (dotIndex == qsizetype(-1)) ? item->info.fileName() : item->info.fileName().first(dotIndex);
    const qsizetype dotIndex = item->fileInfo().fileName().lastIndexOf('.');
    const QString oldFileName = (dotIndex == qsizetype(-1)) ? item->fileInfo().fileName() : item->fileInfo().fileName().first(dotIndex);

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

    //const QString newAbsoluteFilePath = item->info.absoluteFilePath().replace(oldFileName, newFileName);
    //QDir dir(item->info.absolutePath());
    const QString newAbsoluteFilePath = item->fileInfo().absoluteFilePath().replace(oldFileName, newFileName);
    QDir dir(item->fileInfo().absolutePath());

    //if(!dir.rename(item->info.absoluteFilePath(), newAbsoluteFilePath))
    if(!dir.rename(item->fileInfo().absoluteFilePath(), newAbsoluteFilePath))
    {
        __LOGOUT__("Failed to rename the file \"" + oldFileName + "\" to \"" + newAbsoluteFilePath + "\".", Logger::LogLevel::Error);
        return;
    }

    //TreeFileItem::list.remove(item->info.absoluteFilePath());
    TreeFileItem::list.remove(item->fileInfo().absoluteFilePath());
    TreeFileItem::list.insert(newAbsoluteFilePath, item);
    //item->info.setFile(newAbsoluteFilePath);
    //item->setText(0, item->info.fileName());
    //item->setToolTip(0, newAbsoluteFilePath);
    item->setFilePath(newAbsoluteFilePath);
}

void FileTreeWidget::removeFile()
{
    if(selectedItems().count() < 1) return;

    TreeFileItem *item = static_cast<TreeFileItem*>(selectedItems().at(0));

    if(!item) return;

    //QMessageBox::StandardButton reply = QMessageBox::question(this, "Remove", "Do you remove this \"" + item->info.fileName() + "\"??", QMessageBox::Yes | QMessageBox::No);
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Remove", "Do you remove this \"" + item->fileInfo().fileName() + "\"??", QMessageBox::Yes | QMessageBox::No);

    if(reply != QMessageBox::Yes) return;

    //bool ok = false;

    //const QString absPath = item->fileInfo().absoluteFilePath();

    //if(item->type() == (int)TreeItemType::Dir)
    //{   //ディレクトリの場合の削除
    //    dirWatcher->removePath(absPath);
    //    QDir dir(absPath);
    //    ok = dir.removeRecursively();
    //    if(!ok)
    //        dirWatcher->addPath(absPath);
    //}
    //else
    //{   //ファイルの場合の削除
    //    QFile file(absPath);
    //    ok = file.remove();
    //}

    //if(!ok)
    //{
    //    __LOGOUT__("Failed to remove the file \"" + item->fileInfo().absoluteFilePath() + "\".", Logger::LogLevel::Error);
    //    return;
    //}

    connect(item, &TreeFileItem::removed, [=](bool ok)
    {
        if(!this) return;
        if(ok)
            removeItemFromTree(item);
        else
            __LOGOUT__("failed to remove the file \"" + item->fileInfo().absoluteFilePath() + "\".", Logger::LogLevel::Error);
    });

    item->remove();

    //TreeFileItem::list.remove(item->fileInfo().absoluteFilePath());
    //selectedItems().takeAt(0)->parent()->removeChild(item);
    //delete item; item = nullptr;
    //removeItemFromTree(item);
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

    const bool ok = QFile::copy(item->fileInfo().absoluteFilePath(), pathForSave + '/' + item->fileInfo().fileName());

    if(!ok) __LOGOUT__("Could not copy a file \"" + item->fileInfo().absoluteFilePath() + "\".", Logger::LogLevel::Error);

    /* 削除するか確認する */
    removeFile();
}

void FileTreeWidget::addFileFromDialog()
{
    QString nameFilter;
    QString parentPath = folderPath;

    if(selectedItems().count() > 0)
    {
        TreeFileItem *item = static_cast<TreeFileItem*>(selectedItems().at(0));

        if(item)
        {
            /* 選択されたフォルダーによって，ダイアログのフィルターを設定する */

            if(item->type() == (int)FileTreeWidget::TreeItemType::Category)
            {
                switch((TreeItemType)static_cast<TreeCategoryItem*>(item)->category())
                {
                case TreeItemType::Script:
                    nameFilter += "Script files (";
                    foreach(const QString& suffix, TreeScriptItem::suffix.keys())
                        nameFilter += "*." + suffix + " ";
                    nameFilter += ")";
                    break;
                case TreeItemType::Sheet:
                    nameFilter += "Sheet files (";
                    foreach(const QString& suffix, TreeSheetItem::suffix.keys())
                        nameFilter += "*." + suffix + " ";
                    nameFilter += ")";
                    break;
                default:
                    break;
                }
            }

            /* ファイルを追加する親ディレクトリのパスも選択されたフォルダーによって設定する */
            switch((TreeItemType)item->type())
            {
            case TreeItemType::Dir:
                //parentPath = item->info.absoluteFilePath();
                parentPath = item->fileInfo().absoluteFilePath();
                break;
            default:
                parentPath = folderPath;
            }
        }
    }

    /* ファイルを選択するダイアログ。複数選択可能 */
    QFileDialog fileDialog(this);
    fileDialog.setFileMode(QFileDialog::FileMode::ExistingFiles);
    fileDialog.setNameFilters(QStringList() << nameFilter << "All (*)");
    if(!fileDialog.exec()) return;
    const QStringList filePathList = fileDialog.selectedFiles();

    for(const QString& filePath : filePathList)
    {
        if(!TreeFileItem::list.contains(filePath))
        {
            const QString fileName = filePath.sliced(filePath.lastIndexOf('/') + 1);
            const bool ok = QFile::copy(filePath, parentPath + '/' + fileName);

            /* コピーしてこれば、自動的にdirWatcher::directoryChanged() --> updateFileTree() によってTreeに追加される */

            if(!ok) QMessageBox::critical(this, "Error", "Could not copy the file \"" + filePath + "\".");
        }
        else
            QMessageBox::critical(this, "Error", "Same name file \"" + filePath + "\" already exists.");
    }
}

void FileTreeWidget::newFileFromDialog()
{
    QString folderPath = this->folderPath;
    QString defaultFileName = "";

    if(selectedItems().count() > 0)
    {
        TreeFileItem *item = static_cast<TreeFileItem*>(selectedItems().at(0));

        if(item)
        {
            if(item->type() == (int)FileTreeWidget::TreeItemType::Category)
            {
                /* ファイル名を入力する際のデフォルトテキストを拡張子によって変更 */
                switch((TreeItemType)static_cast<TreeCategoryItem*>(item)->category())
                {
                case TreeItemType::Script:
                    if(TreeScriptItem::suffix.count() > 0)
                        defaultFileName = "." + TreeScriptItem::suffix.begin().key();
                    folderPath = this->folderPath;
                    break;
                case TreeItemType::Sheet:
                    if(TreeSheetItem::suffix.count() > 0)
                        defaultFileName = "." + TreeSheetItem::suffix.begin().key();
                    folderPath = this->folderPath;
                    break;
                default:
                    //folderPath = item->info.absoluteFilePath();
                    folderPath = item->fileInfo().absoluteFilePath();
                    break;
                }
            }
        }
    }

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


TreeModelCombo::TreeModelCombo(QWidget *parent)
    : QComboBox(parent)
{
    addItems(enumToStrings(FileTreeWidget::FileTreeModel(0)));
}










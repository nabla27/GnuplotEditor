/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef FILETREEWIDGET_H
#define FILETREEWIDGET_H

#include <QTreeWidget>
#include <QFileInfo>
#include <QTreeWidgetItem>
#include <QFileSystemWatcher>
#include <QComboBox>
#include "texteditor.h"
#include "gnuplottable.h"
#include "browserwidget.h"
#include "iofile.h"

class TreeFileItem : public QObject, public QTreeWidgetItem
{
    Q_OBJECT
public:
    explicit TreeFileItem(QTreeWidgetItem *parent, int type)
        : QTreeWidgetItem(parent, type) { setFileIcon(); }
    explicit TreeFileItem(QTreeWidget *parent, int type)
        : QTreeWidgetItem(parent, type) { setFileIcon(); }

public:
    virtual void save() {}
    virtual void load() {}
    void setText(int column, const QString& text);

    static QHash<QString, TreeFileItem*> list;
    QFileInfo info;
    static QThread iothread;
    bool isSaved() const { return isSavedFlag; }

public slots:
    void setEdited() { setSavedState(false); }

protected:
    void setSavedState(const bool isSaved);

private:
    const QPixmap scriptIcon = QPixmap(":/icon/file_code");
    const QPixmap sheetIcon = QPixmap(":/icon/file_doc");
    const QPixmap otherIcon = QPixmap(":/icon/file_normal");
    const QIcon folderIcon = QApplication::style()->standardIcon(QStyle::SP_DirIcon);
    void setFileIcon();
    bool isSavedFlag = true;

signals:
    void errorCaused(const QString& message, const BrowserWidget::MessageType& type);
    void renamed(TreeFileItem *item);
    void editStateChanged(const bool isSaved);
};





class TreeScriptItem : public TreeFileItem
{
    Q_OBJECT
public:
    explicit TreeScriptItem(QTreeWidgetItem *parent, int type);

    ~TreeScriptItem();

    enum class ReadType { Text, Html };
    Q_ENUM(ReadType)

public:
    void save() override;
    void load() override;

public:
    void requestCloseProcess();

private slots:
    void receiveSavedResult(const bool& ok);
    void receiveLoadedResult(const QString& text, const bool& ok);

public:
    static QHash<QString, ReadType> suffix;
    TextEdit *editor;
    QProcess *process;

signals:
    void saveRequested(const QString& path, const QString& text);
    void loadRequested(const QString& path);
    void closeProcessRequested();
};





class TreeSheetItem : public TreeFileItem
{
    Q_OBJECT
public:
    explicit TreeSheetItem(QTreeWidgetItem *parent, int type)
        : TreeFileItem(parent, type)
        , table(nullptr) {}

    ~TreeSheetItem();

    enum class ReadType { Csv, Tsv };
    Q_ENUM(ReadType)

    void save() override;

    void load() override;

private slots:
    void receiveSavedResult(const bool& ok);
    void receiveLoadResult(const QList<QList<QString> >& data, const bool& ok);

public:
    static QHash<QString, ReadType> suffix;
    GnuplotTable *table;

signals:
    void saveRequested(const QString& path, const QList<QList<QString> >& data);
    void loadRequested(const QString& path);
};








class FileTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    FileTreeWidget(QWidget *parent);
    ~FileTreeWidget();

    enum class FileTreeModel { FileSystem, Gnuplot };
    enum class TreeItemType { Script = 1000, Sheet, Other, Dir, Root, ScriptFolder, SheetFolder, OtherFolder };
    Q_ENUM(FileTreeModel)
    static QStringList fileFilter;
    void setTreeModel(const int type);
    QString currentFolderPath() const { return folderPath; }
    QSize minimumSizeHint() const override { return QSize(0, 0); }

public slots:
    void setFolderPath(const QString& folderPath);
    void updateFileTree();
    void saveAllFile();
    void saveAndLoad();
    void addFolder();
    void saveFolder();

private slots:
    void onCustomContextMenu(const QPoint& pos);
    void selectItem(QTreeWidgetItem *item, int column);

    void loadFileTree();

    void renameFile(); //File & Dir
    void removeFile(); //File & Dir
    void exportFile(); //File
    void addFile(); //Dir & Root
    void newFile(); //Dir & Root

private:
    void initializeContextMenu();

    void updateGnuplotModelTree(const QString& path);
    void updateFileSystemModelTree(const QString& path, QTreeWidgetItem *parent);

    void copyDirectoryRecursively(const QString& fromPath, const QString& toPath);

private:
    FileTreeModel treeModel;
    TreeFileItem *rootTreeItem;
    QTreeWidgetItem *scriptFolderItem;
    QTreeWidgetItem *sheetFolderItem;
    QTreeWidgetItem *otherFolderItem;

    const QPixmap scriptIcon = QPixmap(":/icon/file_code");
    const QPixmap sheetIcon = QPixmap(":/icon/file_doc");
    const QPixmap otherIcon = QPixmap(":/icon/file_normal");

    QString folderPath;
    QFileSystemWatcher *dirWatcher;

    QMenu *fileMenu;
    QMenu *dirMenu;
    QMenu *rootMenu;

signals:
    void scriptSelected(TreeScriptItem *item);
    void sheetSelected(TreeSheetItem *item);
    void otherSelected(TreeFileItem *item);
    void errorCaused(const QString& message, const BrowserWidget::MessageType& type);
};




class TreeModelCombo : public QComboBox
{
    Q_OBJECT
public:
    explicit TreeModelCombo(QWidget *parent) : QComboBox(parent)
    {
        addItems(enumToStrings(FileTreeWidget::FileTreeModel(0)));
    }

    QSize minimumSizeHint() const override { return QSize(0, 0); }
};




#endif // FILETREEWIDGET_H

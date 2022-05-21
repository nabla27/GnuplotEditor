#ifndef FILETREEWIDGET_H
#define FILETREEWIDGET_H

#include <QTreeWidget>
#include <QFileInfo>
#include <QTreeWidgetItem>
#include "gnuploteditor.h"
#include "gnuplottable.h"
#include "browserwidget.h"

class TreeFileItem : public QTreeWidgetItem
{
public:
    explicit TreeFileItem(QTreeWidgetItem *parent, int type)
        : QTreeWidgetItem(parent, type) {}
    explicit TreeFileItem(QTreeWidget *parent, int type)
        : QTreeWidgetItem(parent, type) {}

public:
    virtual void save() {};
    static QHash<QString, TreeFileItem*> list;
    QFileInfo info;
};

class TreeScriptItem : public TreeFileItem
{
public:
    explicit TreeScriptItem(QTreeWidgetItem *parent, int type)
        : TreeFileItem(parent, type) {}

public:
    static QStringList suffix;
    TextEdit *editor;
    QProcess *process;
};

class TreeSheetItem : public TreeFileItem
{
public:
    explicit TreeSheetItem(QTreeWidgetItem *parent, int type)
        : TreeFileItem(parent, type) {}

public:
    static QStringList suffix;
    GnuplotTable *table;
};






class FileTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    FileTreeWidget(QWidget *parent);

    QString currentFolderPath() const { return folderPath; }

public slots:
    void setFolderPath(const QString& folderPath);
    void updateFileTree();
    void saveAllFile();
    //void saveAllScript(){};
    //void saveAllSheet(){};
    void addFolder();
    void saveFolder(){};

private slots:
    void onCustomContextMenu(const QPoint& pos);
    void selectItem(QTreeWidgetItem *item, int column);

    void loadFileTree();

    void renameFile(); //File & Dir
    void removeFile(); //File & Dir
    void exportFile(); //File
    void addFile(); //Dir
    void newFile(); //Dir

private:
    void initializeContextMenu();

    void updateGnuplotModelTree(const QString& path);
    void updateFileSystemModelTree(const QString& path, QTreeWidgetItem *parent);

    void copyDirectoryRecursively(const QString& fromPath, const QString& toPath);

private:
    enum class TreeItemType { Script = 1000, Sheet, Other, Dir, ScriptFolder, SheetFolder, OtherFolder };
    enum class FileTreeModel { FileSystem, Gnuplot };

    FileTreeModel treeModel;
    TreeFileItem *rootTreeItem;
    QTreeWidgetItem *scriptFolderItem;
    QTreeWidgetItem *sheetFolderItem;
    QTreeWidgetItem *otherFolderItem;

    QString folderPath;
    QFileSystemWatcher *dirWatcher;

    QMenu *fileMenu;
    QMenu *dirMenu;

signals:
    void scriptSelected(TreeScriptItem *item);
    void sheetSelected(TreeSheetItem *item);
    void otherSelected(QTreeWidgetItem *item);
    void scriptRemoved(QTreeWidgetItem *item);
    void sheetRemoved(QTreeWidgetItem *item);
    void otherRemoved(QTreeWidgetItem *item);
    void errorCaused(const QString& message, const BrowserWidget::MessageType& type);
};

#endif // FILETREEWIDGET_H

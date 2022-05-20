#ifndef FILETREEWIDGET_H
#define FILETREEWIDGET_H

#include <QTreeWidget>
#include <QFileInfo>
#include <QTreeWidgetItem>
#include "gnuploteditor.h"
#include "gnuplottable.h"
#include "browserwidget.h"

class TreeScriptItem : public QTreeWidgetItem
{
public:
    explicit TreeScriptItem(QTreeWidgetItem *parent, int type)
        : QTreeWidgetItem(parent, type) {}

public:
    static QStringList list;
    static QStringList suffix;
    TextEdit *editor;
    QProcess *process;
    QFileInfo info;
};






class TreeSheetItem : public QTreeWidgetItem
{
public:
    explicit TreeSheetItem(QTreeWidgetItem *parent, int type)
        : QTreeWidgetItem(parent, type) {}

public:
    static QStringList list;
    static QStringList suffix;
    GnuplotTable *table;
    QFileInfo info;
};






class TreeOtherItem : public QTreeWidgetItem
{
public:
    explicit TreeOtherItem(QTreeWidgetItem *parent, int type)
        : QTreeWidgetItem(parent, type) {}

public:
    static QStringList list;
    QFileInfo info;
};





class TreeDirItem : public QTreeWidgetItem
{
public:
    explicit TreeDirItem(QTreeWidgetItem *parent, int type)
        : QTreeWidgetItem(parent, type) {}

public:
    static QStringList list;
    QFileInfo info;
};









class FileTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    FileTreeWidget(QWidget *parent);

    QString currentFolderPath() const { return folderPath; }

public slots:
    void setFolderPath(const QString& folderPath);
    void saveAndLoad(){};
    void updateFileTree();
    void addFolder(){};
    void saveFolder(){};
    void saveAllScript(){};
    void saveAllSheet(){};

private slots:
    void onCustomContextMenu(const QPoint& pos);
    void selectItem(QTreeWidgetItem *item, int column);

    void loadFileTree();

    void renameFile(){};
    void removeFile(){};
    void exportFile(){};
    void addFile(){};
    void newFile(){};

private:
    void initializeContextMenu();

    void updateGnuplotModelTree(const QString& path);
    void updateFileSystemModelTree(const QString& path, QTreeWidgetItem *parent);

private:
    enum class TreeItemType { Script = 1000, Sheet, Other, Dir, Root, ScriptFolder, SheetFolder, OtherFolder };
    enum class FileTreeModel { FileSystem, Gnuplot };

    FileTreeModel treeModel;
    QTreeWidgetItem *rootTreeItem;
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

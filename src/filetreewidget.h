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
        : QTreeWidgetItem(parent, type) {}
    explicit TreeFileItem(QTreeWidget *parent, int type)
        : QTreeWidgetItem(parent, type) {}

public:
    virtual void save() {}
    virtual void load() {}

    static QHash<QString, TreeFileItem*> list;
    QFileInfo info;

signals:
    void errorCaused(const QString& message, const BrowserWidget::MessageType& type);
};





class TreeScriptItem : public TreeFileItem
{
public:
    explicit TreeScriptItem(QTreeWidgetItem *parent, int type)
        : TreeFileItem(parent, type)
        , editor(nullptr)
        , process(new QProcess(nullptr)) {}

    ~TreeScriptItem()
    {
        delete editor; editor = nullptr;
        process->close();
        delete process; process = nullptr;
    }

    void save() override
    {
        if(editor)
        {
            bool ok = false;
            toFileTxt(info.absoluteFilePath(), editor->toPlainText(), &ok);
            if(!ok)
                emit errorCaused("Failed to save this file " + info.absoluteFilePath(),
                                 BrowserWidget::MessageType::FileSystemErr);
        }

    }

    void load() override
    {
        if(editor)
        {
            bool ok = false;
            editor->setPlainText(readFileTxt(info.absoluteFilePath(), &ok));
            if(!ok)
                emit errorCaused("Failed to load this file " + info.absoluteFilePath(),
                                 BrowserWidget::MessageType::FileSystemErr);
        }
    }

public:
    static QStringList suffix;
    TextEdit *editor;
    QProcess *process;
};





class TreeSheetItem : public TreeFileItem
{
public:
    explicit TreeSheetItem(QTreeWidgetItem *parent, int type)
        : TreeFileItem(parent, type)
        , table(nullptr) {}
    ~TreeSheetItem()
    {
        delete table; table = nullptr;
    }

    void save() override
    {
        if(table)
        {
            bool ok = false;
            toFileCsv(info.absoluteFilePath(), table->getData<QString>(), &ok);
            if(!ok)
                emit errorCaused("Failed to save this file " + info.absoluteFilePath(),
                                 BrowserWidget::MessageType::FileSystemErr);
        }
    }

    void load() override
    {
        if(table)
        {
            bool ok = false;
            table->setData(readFileCsv(info.absoluteFilePath(), &ok));
            if(!ok)
                emit errorCaused("Failed to load this file " + info.absoluteFilePath(),
                                 BrowserWidget::MessageType::FileSystemErr);
        }
    }

public:
    static QStringList suffix;
    GnuplotTable *table;
};







class FileTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    FileTreeWidget(QWidget *parent);

    enum class FileTreeModel { FileSystem, Gnuplot };
    Q_ENUM(FileTreeModel)
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
    enum class TreeItemType { Script = 1000, Sheet, Other, Dir, Root, ScriptFolder, SheetFolder, OtherFolder };

    FileTreeModel treeModel;
    TreeFileItem *rootTreeItem;
    QTreeWidgetItem *scriptFolderItem;
    QTreeWidgetItem *sheetFolderItem;
    QTreeWidgetItem *otherFolderItem;

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

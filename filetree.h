#ifndef FILETREE_H
#define FILETREE_H

#include <QTreeWidget>
#include <QFileSystemWatcher>
#include <QProcess>
#include <QMenu>
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QHash>
#include <QSet>
#include <QDir>
#include "utility.h"
#include "texteditor.h"
#include "gnuplottable.h"
#include "iofile.h"
#include "browserwidget.h"






struct ScriptInfo
{
public:
    ScriptInfo(QProcess *process = nullptr, TextEdit *editor = nullptr)
        : process(process), editor(editor) {}
    ~ScriptInfo() {
        delete process; process = nullptr;
        delete editor; editor = nullptr;
    }

    static bool isValidFormat(const QString& fileName){
        for(const QString& format : formatList) if(fileName.contains(format)) return true;
        return false;
    }

    QProcess *process;
    TextEdit *editor;
    static QStringList formatList;
};

struct SheetInfo
{
public:
    SheetInfo(GnuplotTable *table = nullptr) : table(table) {}
    ~SheetInfo() { delete table; table = nullptr; }

    static bool isValidFormat(const QString& fileName){
        for(const QString& format : formatList) if(fileName.contains(format)) return true;
        return false;
    }

    GnuplotTable *table;
    static QStringList formatList;
};

struct OtherInfo
{
public:
    OtherInfo() {}
    ~OtherInfo() {}
};














class FileTree : public QTreeWidget
{
    Q_OBJECT

public:
    FileTree(QWidget *parent);
    ~FileTree();

public slots:
    void loadFileTree();
    void updateFileTree();
    void setFolderPath(const QString& folderPath);

    void addScript(const QString& fileName);
    void addSheet(const QString& fileName);
    void addOther(const QString& fileName);
    void loadScript(const QString& fileName);
    void saveScript(const QString& fileName);
    void loadSheet(const QString& sheetName);
    void saveSheet(const QString& sheetName);
    void loadAllScript();
    void saveAllScript();
    void loadAllSheet();
    void saveAllSheet();
    void clearAllList();

    void addFile();
    void newFile();
    void renameFile();
    void removeFile();
    void exportFile();
    void addFolder();
    void saveFolder();

private:
    void initializeContextMenu();
    bool containsFile(const QString& fileName) const { return scriptList.contains(fileName) || sheetList.contains(fileName) || otherList.contains(fileName); }

private slots:
    void pushClickedItem(QTreeWidgetItem *item, int column);
    void onCustomContextMenu(const QPoint& point);

private:
    QString folderPath = QDir::currentPath() + "/" + BasicSet::tmpDirectory;
    QTreeWidgetItem *scriptTree;
    QTreeWidgetItem *sheetTree;
    QTreeWidgetItem *otherTree;
    QFileSystemWatcher *dirWatcher;
    QHash<QString, ScriptInfo*> scriptList;
    QHash<QString, SheetInfo*> sheetList;
    QHash<QString, OtherInfo*> otherList;

    QMenu *normalMenu;

signals:
    void scriptSelected(const QString& fileName, const ScriptInfo* info);
    void sheetSelected(const QString& fileName, const SheetInfo* info);
    void otherSelected(const QString& fileName, const OtherInfo* info);
    void fileNameChanged(const QString& oldName, const QString newName);
    void scriptRemoved(const QString& fileName, const ScriptInfo* info);
    void sheetRemoved(const QString& fileName, const SheetInfo* info);
    void otherRemoved(const QString& fileName, const OtherInfo* info);
    void errorCaused(const QString& message, const BrowserWidget::MessageType type);
};

#endif // FILETREE_H

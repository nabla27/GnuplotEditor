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
#include <QTreeWidgetItem>
#include <QComboBox>
#include "browserwidget.h"
#include <QFileInfo>
#include <QApplication>
#include <QStyle>

class TableArea;
class QProcess;
class QFileSystemWatcher;
class TextEdit;



class TreeFileItem : public QObject, public QTreeWidgetItem
{
    Q_OBJECT
public:
    explicit TreeFileItem(QTreeWidgetItem *parent, int type, const QFileInfo& info);
    explicit TreeFileItem(QTreeWidget *parent, int type, const QFileInfo& info);

public:
    virtual void save() {}
    virtual void load() {}
    virtual QWidget* widget() { return nullptr; }

    void setFilePath(const QString& path);
    void setData(int column, int role, const QVariant& variant) override;
    void setUpdateTimer(bool enable) { enableUpdateTimer = enable; }

    bool isSaved() const { return isSavedFlag; }
    bool isEnableUpdateTimer() const { return enableUpdateTimer; }
    const QFileInfo& fileInfo() const { return info; }

    static QHash<QString, TreeFileItem*> list;
    static QThread iothread;

public slots:
    void setEdited() { setSavedState(false); }

protected:
    void setSavedState(const bool isSaved);
    QFileInfo info;

private:
    void setFileIcon();

private:
    const QPixmap scriptIcon = QPixmap(":/icon/file_code");
    const QPixmap sheetIcon = QPixmap(":/icon/file_doc");
    const QPixmap imageIcon = QPixmap(":/icon/file_image");
    const QPixmap otherIcon = QPixmap(":/icon/file_normal");
    const QIcon folderIcon = QApplication::style()->standardIcon(QStyle::SP_DirIcon);

    bool isSavedFlag = true;
    QTimer *updateTimer;
    bool enableUpdateTimer = false;
    inline static int updateTime = 1000;

signals:
    void errorCaused(const QString& message, const BrowserWidget::MessageType& type);
    void pathChanged(const QString& path);
    void editStateChanged(const bool isSaved);
    void updated();
};





class TreeScriptItem : public TreeFileItem
{
    Q_OBJECT
public:
    explicit TreeScriptItem(QTreeWidgetItem *parent, int type, const QFileInfo& info);

    ~TreeScriptItem();

    enum class ReadType { Text, Html };
    Q_ENUM(ReadType)

public:
    void save() override;
    void load() override;
    QWidget* widget() override;

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
    explicit TreeSheetItem(QTreeWidgetItem *parent, int type, const QFileInfo& info);

    ~TreeSheetItem();

    enum class ReadType { Csv, Tsv };
    Q_ENUM(ReadType)

    void save() override;
    void load() override;
    QWidget *widget() override;

private slots:
    void receiveSavedResult(const bool& ok);
    void receiveLoadResult(const QList<QList<QString> >& data, const bool& ok);

public:
    static QHash<QString, ReadType> suffix;
    TableArea *table;

signals:
    void saveRequested(const QString& path, const QList<QList<QString> >& data);
    void loadRequested(const QString& path);
};




class ImageDisplay;
class TreeImageItem : public TreeFileItem
{
    Q_OBJECT
public:
    explicit TreeImageItem(QTreeWidgetItem *parent, int type, const QFileInfo& info);
    ~TreeImageItem();

public:
    //QWidget* widget() override { return imageDisplay; }

public:
    ImageDisplay *imageDisplay;
};








class FileTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    FileTreeWidget(QWidget *parent);
    ~FileTreeWidget();

    enum class FileTreeModel { FileSystem, Gnuplot };
    enum class TreeItemType { Script = 1000, Sheet, Image, Other, Dir, Root, ScriptFolder, SheetFolder, OtherFolder };
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
    void openFolder();
    void addFile(); //Dir & Root
    void newFile(); //Dir & Root

private slots:
    void onCustomContextMenu(const QPoint& pos);

    void loadFileTree();//saveする必要がある．publicでは代わりにsaveAndLoad()を使う

    void renameFile(); //File & Dir
    void removeFile(); //File & Dir
    void exportFile(); //File

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

    QString folderPath;
    QFileSystemWatcher *dirWatcher;

    QMenu *fileMenu;
    QMenu *dirMenu;
    QMenu *rootMenu;

signals:
    void folderPathChanged(const QString& path);
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

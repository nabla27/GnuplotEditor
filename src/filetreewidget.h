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
#include <QFileInfo>
#include <QApplication>
#include <QStyle>

class TableArea;
class QFileSystemWatcher;
class TextEdit;
class GnuplotProcess;
class ImageDisplay;
class PdfViewer;



class TreeFileItem : public QObject, public QTreeWidgetItem
{
    Q_OBJECT
public:
    explicit TreeFileItem(QTreeWidgetItem *parent, int type, const QFileInfo& info);
    explicit TreeFileItem(QTreeWidget *parent, int type, const QFileInfo& info);

public:
    virtual void save() = 0;
    virtual void load() { isLoadedFlag = true; }
    virtual void remove() = 0;
    virtual QWidget* widget() { return nullptr; }

    void setFilePath(const QString& path);
    void setData(int column, int role, const QVariant& variant) override;
    void setUpdateTimer(bool enable) { enableUpdateTimer = enable; }

    bool isSaved() const { return isSavedFlag; }
    bool isLoaded() const { return isLoadedFlag; }
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
    bool isSavedFlag = true;
    bool isLoadedFlag = false;
    QTimer *updateTimer;
    bool enableUpdateTimer = false;
    inline static int updateTime = 1000;

signals:
    void pathChanged(const QString& path);
    void editStateChanged(const bool isSaved);
    void updated();
    void removed(const bool ok);
};





class TreeNoCategorizedItem : public TreeFileItem
{
    Q_OBJECT
public:
    explicit TreeNoCategorizedItem(QTreeWidgetItem *parent, const QFileInfo& info);

public:
    void save() override;
    void load() override;
    void remove() override;
};






class TreeFolderItem : public TreeFileItem
{
    Q_OBJECT
public:
    explicit TreeFolderItem(QTreeWidgetItem *parent, const QFileInfo& info);
    explicit TreeFolderItem(QTreeWidget *parent, const QFileInfo& info);

public:
    void save() override;
    void load() override;
    void remove() override;
};






class TreeCategoryItem : public TreeFileItem
{
    Q_OBJECT
public:
    explicit TreeCategoryItem(QTreeWidgetItem *parent, const QFileInfo& info, const int type);

public:
    void save() override;
    void load() override;
    void remove() override;

    int category();

public:
    int categoryType;
};





class TreeScriptItem : public TreeFileItem
{
    Q_OBJECT
public:
    explicit TreeScriptItem(QTreeWidgetItem *parent, const QFileInfo& info);

    ~TreeScriptItem();

    enum class ReadType { Text, Html };
    Q_ENUM(ReadType)

public:
    void save() override;
    void load() override;
    void remove() override;
    QWidget* widget() override;

public:
    void requestCloseProcess();

private slots:
    void receiveSavedResult(const bool& ok);
    void receiveLoadedResult(const QString& text, const bool& ok);

public:
    static QHash<QString, ReadType> suffix;
    TextEdit *editor;
    GnuplotProcess *process;

signals:
    void saveRequested(const QString& path, const QString& text);
    void loadRequested(const QString& path);
    void closeProcessRequested();
};





class TreeSheetItem : public TreeFileItem
{
    Q_OBJECT
public:
    explicit TreeSheetItem(QTreeWidgetItem *parent, const QFileInfo& info);

    ~TreeSheetItem();

    enum class ReadType { Csv, Tsv };
    Q_ENUM(ReadType)

    void save() override;
    void load() override;
    void remove() override;
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





class TreeImageItem : public TreeFileItem
{
    Q_OBJECT
public:
    explicit TreeImageItem(QTreeWidgetItem *parent, const QFileInfo& info);
    ~TreeImageItem();

public:
    void save() override;
    void load() override;
    void remove() override;
    QWidget* widget() override;

private:
    ImageDisplay *imageDisplay;
};







class TreePdfItem : public TreeFileItem
{
    Q_OBJECT
public:
    explicit TreePdfItem(QTreeWidgetItem *parent, const QFileInfo& info);
    ~TreePdfItem();

public:
    void save() override;
    void load() override;
    void remove() override;
    QWidget *widget() override;

private:
    PdfViewer *viewer;
};








class FileTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    FileTreeWidget(QWidget *parent);
    ~FileTreeWidget();

    enum class FileTreeModel { FileSystem, Gnuplot };
    enum class TreeItemType { Script = 1000, Sheet, Image, Pdf, NoCategorized, Dir, Category };
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
    void addFileFromDialog(); //Dir & Root
    void newFileFromDialog(); //Dir & Root

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dropEvent(QDropEvent *e) override;

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
    void addFilesFromUrls(const QList<QUrl>& urls, const QString& parentPath);
    void moveItem(TreeFileItem *item, const QString& parentPath);

    void removeItemFromTree(TreeFileItem *item);
    void removeItemFromList(TreeFileItem *item);


private:
    FileTreeModel treeModel;
    TreeFileItem *rootTreeItem;
    TreeCategoryItem *scriptFolderItem;
    TreeCategoryItem *sheetFolderItem;
    TreeCategoryItem *otherFolderItem;

    QString folderPath;
    QFileSystemWatcher *dirWatcher;

    QMenu *fileMenu;
    QMenu *dirMenu;
    QMenu *categoryMenu;

signals:
    void folderPathChanged(const QString& path);
};




class TreeModelCombo : public QComboBox
{
    Q_OBJECT
public:
    explicit TreeModelCombo(QWidget *parent);

    QSize minimumSizeHint() const override { return QSize(0, 0); }
};




#endif // FILETREEWIDGET_H

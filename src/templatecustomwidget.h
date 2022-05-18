#ifndef TEMPLATECUSTOMWIDGET_H
#define TEMPLATECUSTOMWIDGET_H
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QApplication>
#include "texteditor.h"
#include "layoutparts.h"

class TemplateItemPanel;
class TemplateEditorPanel;
class TemplateItemWidget;

class TemplateCustomWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TemplateCustomWidget(QWidget *parent = nullptr);
    ~TemplateCustomWidget();

public slots:
    void addTemplate(const QString& script);

private:
    void setupTemplateList(const QString& folderPath);
    void setupNewTemplate(const QString& filePath);
    void setupNewFolder(const QString& folderPath);

    void getDirRecursively(const QString& folderPath, QStringList& folderList);

    void saveCurrentTemplateFile();

private slots:
    void requestImportingTemplate();

    void setTemplate(const QString& filePath);
    void renameTemplate(const QString& oldFilePath, const QString& newFilePath);
    void removeTemplate(TemplateItemWidget* item, const QString& filePath);

    void setFolder(const QString& folderPath);
    void removeFolder(TemplateItemWidget* item, const QString& folderPath);

    void backDirectory();
    void reloadTemplateList();
    void createNewTemplate();
    void createNewFolder();

private:
    const QString settingFolderPath;
    const QString rootFolderName;
    const QString rootFolderPath;

    TemplateItemPanel *templateItemPanel;
    QScrollArea *templateScriptTreeArea;

    QWidget *templateScriptTree;
    QVBoxLayout *templateScriptTreeLayout;

    QString currentTemplateFolderPath;
    QString currentTemplateFilePath;
    TemplateEditorPanel *editorPanel;
    TextEdit *editor;

signals:
    void importTemplateRequested(const QString& script);
};





class TemplateItemPanel : public QWidget
{
    Q_OBJECT
public:
    explicit TemplateItemPanel(QWidget *parent);

public:
    void setFolderName(const QString& folderName);

private:
    mlayout::IconLabel *backDirIcon;
    QLineEdit *folderNameEdit;
    mlayout::IconLabel *reloadDirIcon;
    mlayout::IconLabel *newFileIcon;
    mlayout::IconLabel *newFolderIcon;

signals:
    void backDirRequested();
    void reloadDirRequested();
    void createTemplateRequested();
    void createFolderRequested();
};







class TemplateItemWidget : public QWidget
{
    Q_OBJECT
public:
    enum class ItemType { File, Dir };
    Q_ENUM(ItemType)

    explicit TemplateItemWidget(const ItemType& itemType, const QString& name, const QString& filePath, QWidget *parent);

    ItemType itemType;

private:
    void setupToolMenu();
    void renameTempalteFile();
    void renameFolder();
    void removeTemplateFile();
    void removeFolder();

private slots:
    void emitSelectedSignals();
    void showToolMenu();
    void rename();
    void remove();

private:
    QString filePath;

    mlayout::IconLabel *typeIcon;
    QPushButton *scriptNameButton;
    QLabel *buttonLabel;
    mlayout::IconLabel *toolIcon;

    QMenu *toolMenu;

signals:
    void templateSelected(const QString& filePath);
    void templateRenamed(const QString& oldPath, const QString& newPath);
    void templateRemoved(TemplateItemWidget*, const QString& filePath);

    void folderSelected(const QString& filePath);
    void folderRenamed(const QString& oldPath, const QString& newPath);
    void folderRemoved(TemplateItemWidget*, const QString& folderPath);
};





class TemplateEditorPanel : public QWidget
{
    Q_OBJECT
public:
    explicit TemplateEditorPanel(const QString& rootFolderName, QWidget *parent);

public:
    void setTemplateName(const QString& filePath);
    void renameFolder(const QString& oldFolderPath, const QString& newFolderPath);

private slots:
    void changeReadOnly(const bool isReadOnly);

private:
    const QString rootFolderName;
    QLineEdit *templateNameEdit;
    QRadioButton *readOnlyButton;
    QPushButton *importButton;

signals:
    void importButtonReleased();
    void readOnlyChanged(const bool isReadOnly);
};


#endif // TEMPLATECUSTOMWIDGET_H

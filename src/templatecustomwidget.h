#ifndef TEMPLATECUSTOMWIDGET_H
#define TEMPLATECUSTOMWIDGET_H
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPushButton>
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

private:
    void setupTemplateList();

private slots:
    void setTemplate(const QString& filePath);
    void renameTemplate(const QString& oldFilePath, const QString& newFilePath);
    void removeTemplate(TemplateItemWidget* item, const QString& filePath);

private:
    const QString settingFolderPath;
    const QString rootFolderName;
    const QString templateFolderPath;

    TemplateItemPanel *templateItemPanel;
    QScrollArea *templateScriptTreeArea;
    QWidget *templateScriptTree;
    QVBoxLayout *templateScriptTreeLayout;

    QString currentTemplateFilePath;
    TemplateEditorPanel *editorPanel;
    TextEdit *editor;

signals:

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
};







class TemplateItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TemplateItemWidget(const QString& name, const QString& filePath, QWidget *parent);

private:
    void setupToolMenu();

private slots:
    void emitSelectedSignals();
    void showToolMenu();
    void rename();
    void remove();

private:
    QString filePath;
    QPushButton *scriptNameButton;
    QLabel *buttonLabel;
    mlayout::IconLabel *toolIcon;

    QMenu *toolMenu;

signals:
    void templateSelected(const QString& filePath);
    void templateRenamed(const QString& oldPath, const QString& newPath);
    void removed(TemplateItemWidget*, const QString& filePath);
};





class TemplateEditorPanel : public QWidget
{
    Q_OBJECT
public:
    explicit TemplateEditorPanel(const QString& rootFolderName, QWidget *parent);

public:
    void setTemplateName(const QString& filePath);

private:
    const QString rootFolderName;
    QLineEdit *templateNameEdit;
};


#endif // TEMPLATECUSTOMWIDGET_H

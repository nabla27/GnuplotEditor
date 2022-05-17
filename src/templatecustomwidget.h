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

class TemplateEditorPanel;

class TemplateCustomWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TemplateCustomWidget(QWidget *parent = nullptr);

private:
    void setupTemplateList();

private slots:
    void setTemplate(const QString& templateName);

private:
    const QString settingFolderPath = QApplication::applicationDirPath() + "/setting";
    const QString templateFolderPath = settingFolderPath + "/script-template";

    QScrollArea *templateScriptTreeArea;
    QWidget *templateScriptTree;
    QVBoxLayout *templateScriptTreeLayout;

    TemplateEditorPanel *editorPanel;
    TextEdit *editor;

signals:

};

class TemplateItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TemplateItemWidget(const QString& name, QWidget *parent);

private:
    QPushButton *scriptNameButton;
    QLabel *buttonLabel;
    mlayout::IconLabel *menuTool;

private slots:
    void emitSetEditorSignals();

signals:
    void templateSelected(const QString& templateName);
};





class TemplateEditorPanel : public QWidget
{
    Q_OBJECT
public:
    explicit TemplateEditorPanel(QWidget *parent);

public:
    void setTemplateName(const QString& templateName);

private:
    QLineEdit *templateNameEdit;
};


#endif // TEMPLATECUSTOMWIDGET_H

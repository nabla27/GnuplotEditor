#ifndef GNUPLOTEDITOR_H
#define GNUPLOTEDITOR_H
#include <QMainWindow>
#include <QMenuBar>
#include <QScreen>
#include <QVBoxLayout>
#include <QPalette>
#include <QStackedWidget>

#include "menubar.h"
#include "utility.h"
#include "filetreewidget.h"
#include "texteditor.h"
#include "gnuplottable.h"
#include "browserwidget.h"
#include "windowmenubar.h"
#include "layoutparts.h"
#include "editorsettingwidget.h"
#include "consolewidget.h"
#include "gnuplotsettingwidget.h"
#include "templatecustomwidget.h"



class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget *parent) : QTabWidget(parent) {}

    QSize minimumSizeHint() const override { return QSize(0, 0); }
};


class GnuplotEditor : public QMainWindow
{
    Q_OBJECT
public:
    explicit GnuplotEditor(QWidget *parent = nullptr);
    ~GnuplotEditor();

private:
    void initializeMenuBar();
    void initializeLayout();
    void connectEditorSetting(TextEdit *const editor);
    void postProcessing();

private slots:
    void setEditorWidget(TreeScriptItem *item);
    void setSheetWidget(TreeSheetItem *item);
    void setOtherWidget(TreeFileItem *item);
    void setMenuBarTitle(const QString& oldName, const QString& newName);

    void executeGnuplot();
    void receiveGnuplotStdOut(const QString& text);
    void receiveGnuplotStdErr(const QString& text, const int line);

    void closeCurrentProcess();
    void moveSheetToNewWindow();
    void changeSheetAutoUpdating();

    void importTemplate(const QString& script);
    void saveAsTemplate();

    void reboot();

private:
    ScriptMenu *scriptMenu;
    SheetMenu *sheetMenu;

    Gnuplot *gnuplot;
    QProcess *gnuplotProcess;

    EditorSettingWidget *editorSetting;
    GnuplotSettingWidget *gnuplotSetting;
    TemplateCustomWidget *templateCustom;

    TreeModelCombo *treeModelCombo;
    FileTreeWidget *fileTree;
    TabWidget *editorTab;
    TabWidget *displayTab;
    QStackedWidget *gnuplotWidget;
    QStackedWidget *sheetWidget;
    ConsoleWidget *consoleWidget;
    BrowserWidget *browserWidget;

signals:
    void workingDirectoryChanged(const QString& path);
};


#endif // GNUPLOTEDITOR_H

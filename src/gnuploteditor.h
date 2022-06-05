/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

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
#include "filetreesettingwidget.h"


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
    void postProcessing();

private slots:
    void setEditorWidget(TreeScriptItem *item);
    void setSheetWidget(TreeSheetItem *item);
    void setOtherWidget(TreeFileItem *item);

    void executeGnuplot();
    void receiveGnuplotStdOut(const QString& text);
    void receiveGnuplotStdErr(const QString& text, const int line);

    void closeCurrentProcess();
    void moveSheetToNewWindow();
    void changeSheetAutoUpdating();

    void importTemplate(const QString& script);
    void saveAsTemplate();

    void showGnuplotHelp();
    void showCommandHelp(const QString& command);

    void reboot();

private:
    MenuBarWidget *menuBarWidget;

    Gnuplot *gnuplot;
    QProcess *gnuplotProcess;

    EditorSetting *editorSetting;
    GnuplotSettingWidget *gnuplotSetting;
    TemplateCustomWidget *templateCustom;
    FileTreeSettingWidget *fileTreeSetting;

    TreeModelCombo *treeModelCombo;
    FileTreeWidget *fileTree;
    TabWidget *editorTab;
    TabWidget *displayTab;
    QStackedWidget *gnuplotWidget;
    QStackedWidget *sheetWidget;
    ConsoleWidget *consoleWidget;
    BrowserWidget *browserWidget;

    TreeFileItem *currentScript = nullptr;
    TreeFileItem *currentSheet = nullptr;

signals:
    void workingDirectoryChanged(const QString& path);
};


#endif // GNUPLOTEDITOR_H

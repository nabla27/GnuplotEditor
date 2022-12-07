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

class TableArea;
class EditorArea; //DEBUG

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

public slots:
    void setCurrentItem(QWidget *oldWidget, QWidget *newWidget);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void initializeMenuBar();
    void initializeLayout();

    void setupScriptItem(TreeScriptItem *item);
    void setupSheetItem(TreeSheetItem *item);
    void setEditorWidget(TreeScriptItem *item);
    void setSheetWidget(TreeSheetItem *item);
    void setImageWidget(TreeImageItem *item);

private slots:
    void receiveTreeItem(QTreeWidgetItem *item, const int column);

    /* execute gnuplot */
    void executeItem(TreeFileItem *item);
    void executeGnuplot(TreeScriptItem *item);
    void receiveGnuplotStdOut(const QString& text);
    void receiveGnuplotStdErr(const QString& text, const int line);

    /* menu bar */
    void findKeyword();
    void showGnuplotCmdHelp();
    void reboot();

private:
    //MenuBarWidget *menuBarWidget;

    Gnuplot *gnuplot;
    //QProcess *gnuplotProcess;
    //QThread gnuplotThread;

    //EditorMenu *editorMenu;

    EditorSetting *editorSetting;
    GnuplotSettingWidget *gnuplotSetting;
    TemplateCustomWidget *templateCustom;
    FileTreeSettingWidget *fileTreeSetting;

    FileMenu *fileMenu;
    EditorMenu *editorMenu;
    GnuplotMenu *gnuplotMenu;
    ViewMenu *viewMenu;
    HelpMenu *helpMenu;

    TreeModelCombo *treeModelCombo;
    FileTreeWidget *fileTree;
    //TabWidget *editorTab;
    TabWidget *displayTab;
    //QStackedWidget *gnuplotWidget;
    //TableArea *tableArea;
    EditorArea *editorArea; //DEBUG
    //QStackedWidget *sheetWidget;
    ConsoleWidget *consoleWidget;
    BrowserWidget *browserWidget;

    //TreeScriptItem *currentScript = nullptr;
    //TreeSheetItem *currentSheet = nullptr;



signals:
    void scriptPathChanged(const QString& path);
    void exeGnuplotRequested(QProcess *process, const QList<QString>& cmdlist, const bool preHandling = true);
};


#endif // GNUPLOTEDITOR_H

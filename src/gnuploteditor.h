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
#include "filetree.h"
#include "texteditor.h"
#include "gnuplottable.h"
#include "browserwidget.h"
#include "windowmenubar.h"
#include "layoutparts.h"
#include "editorsettingwidget.h"
#include "consolewidget.h"
#include "gnuplotsettingwidget.h"
#include "updatemanager.h"

class GnuplotEditor : public QMainWindow
{
    Q_OBJECT
public:
    explicit GnuplotEditor(const QString& oldAppPath = QString(), QWidget *parent = nullptr);
    ~GnuplotEditor();

private:
    void initializeMenuBar();
    void initializeLayout();
    void connectEditorSetting(TextEdit *const editor);
    void postProcess();

private slots:
    void setEditorWidget(const QString& fileName, const ScriptInfo* info);
    void setSheetWidget(const QString& fileName, const SheetInfo* info);
    void setOtherWidget(const QString& fileName, const OtherInfo* info);
    void setMenuBarTitle(const QString& oldName, const QString& newName);
    void executeGnuplot();
    void receiveGnuplotStdOut(const QString& text);
    void receiveGnuplotStdErr(const QString& text, const int line);
    void setFileTreeWidth(const int dx);
    void setDisplayTabHeight(const int dy);

    void closeApplication();

private:
    ScriptMenu *scriptMenu;
    SheetMenu *sheetMenu;

    Gnuplot *gnuplot;
    QProcess *gnuplotProcess;

    EditorSettingWidget *editorSetting;
    GnuplotSettingWidget *gnuplotSetting;

    FileTree *fileTree;
    QTabWidget *editorTab;
    QTabWidget *displayTab;
    QStackedWidget *gnuplotWidget;
    QStackedWidget *sheetWidget;
    ConsoleWidget *consoleWidget;
    BrowserWidget *browserWidget;
    UpdateManager *updateManager;

    const QString oldAppFolderPath;

signals:
    void workingDirectoryChanged(const QString& path);
};


#endif // GNUPLOTEDITOR_H

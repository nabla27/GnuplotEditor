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


class EditorArea;

class QTreeWidgetItem;
class TreeFileItem;
class TreeScriptItem;
class TreeSheetItem;
class TreeImageItem;

class EditorSetting;
class GnuplotSettingWidget;
class TemplateCustomWidget;
class FileTreeSettingWidget;

class FileMenu;
class EditorMenu;
class GnuplotMenu;
class ViewMenu;
class HelpMenu;

class TreeModelCombo;
class FileTreeWidget;
class TerminalTabWidget;






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
    /* QApplication::focusChanged(QWidget*,QWidget*)で呼ばれる */
    void setCurrentItem();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void initializeMenuBar();
    void initializeLayout();

    //void setupScriptItem(TreeScriptItem *item);
    //void setupSheetItem(TreeSheetItem *item);
    //void setEditorWidget(TreeScriptItem *item);
    //void setSheetWidget(TreeSheetItem *item);
    //void setImageWidget(TreeImageItem *item);

private slots:
    void receiveTreeItem(QTreeWidgetItem *item, const int column);

    /* execute */
    void executeItem(TreeFileItem *item);
    void executeGnuplot(TreeScriptItem *item);
    void sendGnuplotCmd();

    /* menu bar */
    void findKeyword();
    void showGnuplotCmdHelp();
    void showGnuplotHelpWindow();
    void reboot();

private:
    FileMenu *fileMenu;
    EditorMenu *editorMenu;
    GnuplotMenu *gnuplotMenu;
    ViewMenu *viewMenu;
    HelpMenu *helpMenu;

    EditorSetting *editorSetting;
    GnuplotSettingWidget *gnuplotSetting;
    TemplateCustomWidget *templateCustom;
    FileTreeSettingWidget *fileTreeSetting;

    TreeModelCombo *treeModelCombo;
    FileTreeWidget *fileTree;
    EditorArea *editorArea;
    TerminalTabWidget *terminalTab;

    TreeScriptItem *requestedItem = nullptr;
};


#endif // GNUPLOTEDITOR_H

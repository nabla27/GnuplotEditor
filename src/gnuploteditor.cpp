/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "gnuploteditor.h"

#include <QMessageBox>
#include <QSplitter>
#include <QMenuBar>
#include <QProcess>

#include "imagedisplay.h"
#include "editorwidget.h"

#include "editorsettingwidget.h"
#include "gnuplotsettingwidget.h"
#include "templatecustomwidget.h"
#include "tablesettingwidget.h"
#include "filetreesettingwidget.h"

#include "menubar.h"
#include "terminalwidget.h"
#include "logger.h"
#include "gnuplot.h"
#include "utility.h"

//DEBUG
#include "../plugin/editor_plugin.h"
#include "plugin.h"
#include <QPluginLoader>
#include <QThreadPool>
#include "texteditor.h"
#include <QTimer>
#include "imageviewer.h"
#include "layoutparts.h"
#include "gnuplottexteditor.h"


GnuplotEditor::GnuplotEditor(QWidget *parent)
    : QMainWindow(parent)

    , fileMenu(new FileMenu("&File", this))
    , editorMenu(new EditorMenu("&Editor", this))
    , gnuplotMenu(new GnuplotMenu("&Gnuplot", this))
    , viewMenu(new ViewMenu("&View", this))
    , helpMenu(new HelpMenu("&Help", this))

    , editorSetting(new EditorSetting(nullptr))
    , gnuplotSetting(new GnuplotSettingWidget(nullptr))
    , templateCustom(new TemplateCustomWidget(this))
    , fileTreeSetting(new FileTreeSettingWidget(nullptr))
{
    /* ウィンドウをスクリーン画面に対して(0.4,0.5)の比率サイズに設定 */
    setGeometry(mutility::getRectFromScreenRatio(screen()->size(), 0.4f, 0.5f));

    //{
    //    const QString dllPath = "E:/repos/qt_project/LabQ/GnuplotEditor/plugin/gnuplot-completion/x64/Release/gnuplot-completion.dll";
    //    const QString checkerPath = "E:/repos/qt_project/LabQ/GnuplotEditor/plugin/pluginchecker/bin/release/pluginchecker.exe";
    //    const QString symbolName = "createPluginInstance";

    //    GnuplotTextEditor *editor = new GnuplotTextEditor(nullptr);
    //    editor->show();
    //    connect(this, &GnuplotEditor::destroyed, editor, &GnuplotTextEditor::deleteLater);
    //}


    /* レイアウト生成 */
    initializeLayout();

    /* メニュバーの生成 */
    initializeMenuBar();

    /* workingDirectoryの設定 */
    QString workingDirectory = fileTreeSetting->previousFolderPath();
    QDir dir(workingDirectory);
    if(workingDirectory.isEmpty() || !dir.exists())
    {
        workingDirectory = QApplication::applicationDirPath() + "/setting/gnuplot-project";
        dir.mkdir(workingDirectory);
    }
    fileTree->setFolderPath(workingDirectory);
    fileTreeSetting->setPreviousFolderPath(workingDirectory);

    connect(fileTree, &FileTreeWidget::itemDoubleClicked, this, &GnuplotEditor::receiveTreeItem);
    connect(fileTree, &FileTreeWidget::folderPathChanged, fileTreeSetting, &FileTreeSettingWidget::setPreviousFolderPath);
    connect(fileTreeSetting, &FileTreeSettingWidget::reloadRequested, fileTree, &FileTreeWidget::saveAndLoad);

    /* gnuplotとそのプロセスは別スレッドで非同期処理 */
    //gnuplotの設定GnuplotSettingWidget
    connect(gnuplotSetting, &GnuplotSettingWidget::exePathSet, gnuplotExecutor, &GnuplotExecutor::setExePath);
    connect(gnuplotSetting, &GnuplotSettingWidget::initCmdSet, gnuplotExecutor, &GnuplotExecutor::setInitializeCmd);
    connect(gnuplotSetting, &GnuplotSettingWidget::preCmdSet, gnuplotExecutor, &GnuplotExecutor::setPreProcessingCmd);
    gnuplotSetting->loadXmlSetting(); //connectしてから読み込む

    connect(fileTree, &FileTreeWidget::allSaved, this, &GnuplotEditor::sendGnuplotCmd);
}

GnuplotEditor::~GnuplotEditor()
{

}

void GnuplotEditor::closeEvent(QCloseEvent *event)
{
    /* セーブしていないファイルがあれば，確認して保存する */
    QHashIterator<QString, TreeFileItem*> fileIter(TreeFileItem::list);
    while (fileIter.hasNext())
    {
        fileIter.next();
        if(!fileIter.value()->isSaved())
        {
            QMessageBox question(this);
            question.setIcon(QMessageBox::Icon::Question);
            question.setText("Some files are not saved. Do you want to save the files??");
            question.setStandardButtons(QMessageBox::StandardButton::Save | QMessageBox::StandardButton::No | QMessageBox::Cancel);
            question.setEscapeButton(QMessageBox::StandardButton::Cancel);

            switch(QMessageBox::StandardButton(question.exec()))
            {
            case QMessageBox::StandardButton::Save:
                fileTree->saveAllFile();
                break;
            case QMessageBox::StandardButton::No:
                break;
            case QMessageBox::StandardButton::Cancel:
                event->ignore(); //アプリ閉じるのをキャンセル
                return;
            default:
                break;
            }

            /* 一度確認したら抜け出す */
            break;
        }
    }

    editorSetting->hide();
    gnuplotSetting->hide();
    fileTreeSetting->hide();
    delete editorSetting;
    delete gnuplotSetting;
    delete fileTreeSetting;

    QApplication::exit(0);
}

void GnuplotEditor::initializeMenuBar()
{
    if(menuBar()) delete menuBar();

    //WindowMenuBar *menuBar = new WindowMenuBar(this);
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(editorMenu);
    menuBar->addMenu(gnuplotMenu);
    menuBar->addMenu(viewMenu);
    menuBar->addMenu(helpMenu);

    connect(fileMenu, &FileMenu::aboutToShow, [this](){ fileMenu->setCurrentItem(editorArea->currentTreeFileItem()); });
    connect(fileMenu, &FileMenu::openFolderRequested, fileTree, &FileTreeWidget::openFolder);
    connect(fileMenu, &FileMenu::addFolderRequested, fileTree, &FileTreeWidget::addFolder);
    connect(fileMenu, &FileMenu::saveFolderRequested, fileTree, &FileTreeWidget::saveFolder);
    connect(fileMenu, &FileMenu::updateFolderRequested, fileTree, &FileTreeWidget::updateFileTree);
    connect(fileMenu, &FileMenu::reloadFolderRequested, fileTree, &FileTreeWidget::saveAndLoad);
    connect(fileMenu, &FileMenu::openFileRequested, fileTree, &FileTreeWidget::addFileFromDialog);
    connect(fileMenu, &FileMenu::newFileRequested, fileTree, &FileTreeWidget::newFileFromDialog);
    connect(fileMenu, &FileMenu::saveAllFilesRequested, fileTree, &FileTreeWidget::saveAllFile);
    connect(fileMenu, &FileMenu::openTreeSettingRequested, fileTreeSetting, &FileTreeSettingWidget::show);

    connect(editorMenu, &EditorMenu::aboutToShow, [this](){ editorMenu->setCurrentItem(editorArea->currentTreeFileItem()); });
    connect(editorMenu, &EditorMenu::findRequested, this, &GnuplotEditor::findKeyword);

    connect(gnuplotMenu, &GnuplotMenu::aboutToShow, [this](){ gnuplotMenu->setCurrentItem(editorArea->currentTreeFileItem()); });
    connect(gnuplotMenu, &GnuplotMenu::runRequested, this, &GnuplotEditor::executeItem);
    connect(gnuplotMenu, &GnuplotMenu::showCmdHelpRequested, this, &GnuplotEditor::showGnuplotCmdHelp);
    connect(gnuplotMenu, &GnuplotMenu::showGnuplotHelpRequested, this, &GnuplotEditor::showGnuplotHelpWindow);
    connect(gnuplotMenu, &GnuplotMenu::saveAsTemplateRequested, templateCustom, &TemplateCustomWidget::addTemplate);
    connect(gnuplotMenu, &GnuplotMenu::showScriptTemplateRequested, templateCustom, &TemplateCustomWidget::show);
    connect(gnuplotMenu, &GnuplotMenu::showGnuplotSettingRequested, gnuplotSetting, &GnuplotSettingWidget::show);

    connect(viewMenu, &ViewMenu::splitVerticallyRequested, editorArea, &EditorArea::splitFocusedWidgetVertically);
    connect(viewMenu, &ViewMenu::splitHorizontallyRequested, editorArea, &EditorArea::splitFocusedWidgetHorizontally);
    connect(viewMenu, &ViewMenu::unsplitRequested, editorArea, &EditorArea::closeFocusedWidget);
    connect(viewMenu, &ViewMenu::removeAllStackedEditorRequested, editorArea, &EditorArea::removeAllStackedWidget);
    connect(viewMenu, &ViewMenu::clearTerminalRequested, terminalTab, &TerminalTabWidget::clearDisplay);
    connect(viewMenu, &ViewMenu::showEditorSettingRequested, editorSetting, &EditorSetting::show);

    connect(helpMenu, &HelpMenu::rebootRequested, this, &GnuplotEditor::reboot);
}

void GnuplotEditor::initializeLayout()
{
    if(centralWidget()) delete centralWidget();

    setCentralWidget(new QWidget(this));

    /* ウィンドウ内の大枠となるレイアウトとウィジェットの初期化 */
    QHBoxLayout *hLayout = new QHBoxLayout;
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, centralWidget());
    QSplitter *editorSplitter = new QSplitter(Qt::Vertical, mainSplitter);

    //左側のファイルツリー
    QWidget *fileTreeContents = new QWidget(mainSplitter);
    QVBoxLayout *treeLayout = new QVBoxLayout;
    treeModelCombo = new TreeModelCombo(fileTreeContents);
    fileTree = new FileTreeWidget(fileTreeContents);

    //右側のエディター
    QVBoxLayout *vLayout = new QVBoxLayout;
    editorArea = new EditorArea(editorSplitter);
    terminalTab = new TerminalTabWidget(editorSplitter);

    /* 配置 */
    centralWidget()->setLayout(hLayout);
    hLayout->addWidget(mainSplitter);
    mainSplitter->addWidget(fileTreeContents);
    mainSplitter->addWidget(editorSplitter);

    //左側のファイルツリー
    fileTreeContents->setLayout(treeLayout);
    treeLayout->addWidget(treeModelCombo);
    treeLayout->addWidget(fileTree);

    //右側のエディタ
    editorSplitter->addWidget(editorArea);
    editorSplitter->addWidget(terminalTab);

    hLayout->setSpacing(0);
    vLayout->setSpacing(0);
    treeLayout->setSpacing(0);
    hLayout->setContentsMargins(0, 0, 0, 0);
    mainSplitter->setContentsMargins(0, 0, 0, 0);
    editorSplitter->setContentsMargins(0, 0, 0, 0);
    fileTreeContents->setContentsMargins(0, 0, 0, 0);
    treeLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setContentsMargins(0, 0, 0, 0);

    /* widgetサイズの初期化 */
    const int windowWidth = geometry().width();
    const int windowHeight = geometry().height();
    mainSplitter->setSizes(QList<int>() << windowWidth * 0.2 << windowWidth * 0.7);
    editorSplitter->setSizes(QList<int>() << windowHeight * 0.75 << windowHeight * 0.25);

    connect(treeModelCombo, &QComboBox::currentIndexChanged, fileTree, &FileTreeWidget::setTreeModel);
    connect(editorArea, &EditorArea::executeRequested, this, &GnuplotEditor::executeItem);
}

void GnuplotEditor::setCurrentItem()
{
    TreeFileItem *item = editorArea->currentTreeFileItem();
    fileMenu->setCurrentItem(item);
    editorMenu->setCurrentItem(item);
    gnuplotMenu->setCurrentItem(item);
}

void GnuplotEditor::receiveTreeItem(QTreeWidgetItem *_item, const int column)
{
    TreeFileItem *item = static_cast<TreeFileItem*>(_item);

    if(column != 0)
    {
        logger->output(__FILE__, __LINE__, __FUNCTION__,
                       "received a item that column is not zero.", Logger::LogLevel::Warn);
        return;
    }

    if(!item)
    {
        logger->output(__FILE__, __LINE__, __FUNCTION__,
                       "received a nullptr item.", Logger::LogLevel::Warn);
        return;
    }

    logger->output(__FILE__, __LINE__, __FUNCTION__,
                   "received a item " + item->text(0), Logger::LogLevel::Info);

    if(!item->isLoaded())
        item->load();

    switch((FileTreeWidget::TreeItemType)item->type())
    {
    case FileTreeWidget::TreeItemType::Script:
    {
        editorSetting->addEditor(static_cast<TreeScriptItem*>(item)->editor);
        break;
    }
    case FileTreeWidget::TreeItemType::Sheet:
    case FileTreeWidget::TreeItemType::Image:
    case FileTreeWidget::TreeItemType::Pdf:
    default:
        break;
    }

    editorArea->setItem(item);

    fileMenu->setCurrentItem(item);
    editorMenu->setCurrentItem(item);
    gnuplotMenu->setCurrentItem(item);
}

//void GnuplotEditor::setEditorWidget(TreeScriptItem *item)
//{
//    if(!item) return;
//
//    editorSetting->addEditor(item->editor);
//    editorArea->setItem(item);
//    editorMenu->setCurrentItem(item);
//}
//
//void GnuplotEditor::setSheetWidget(TreeSheetItem *item)
//{
//    if(!item) return;
//
//    //DEBUG
//    editorArea->setItem(item);
//    editorMenu->setCurrentItem(item);
//
//    //connect(gnuplotSetting, &GnuplotSettingWidget::autoCompileMsecSet, item->table, &GnuplotTable::setUpdateMsec);
//}
//
//void GnuplotEditor::setImageWidget(TreeImageItem *item)
//{
//    if(!item) return;
//
//    editorArea->setItem(item);
//    editorMenu->setCurrentItem(item);
//}

void GnuplotEditor::executeItem(TreeFileItem *item)
{
    if(!item) return;

    terminalTab->logBrowser()->grayOutAll();

    switch(FileTreeWidget::TreeItemType(item->type()))
    {
    case FileTreeWidget::TreeItemType::Script:
        executeGnuplot(static_cast<TreeScriptItem*>(item));
        break;
    default:
        break;
    }
}

void GnuplotEditor::executeGnuplot(TreeScriptItem *item)
{
    if(!item) return;

    if(TextEdit *textEdit = item->editor)
    {
        textEdit->setErrorLineNumber(-1);
        textEdit->highlightLine();
    }

    /* FileTreeWidget::allSaved()が発せられたら，GnuplotEditor::sendGnuplotCmd()を実行
     * すべてセーブしてからでないと，gnuplot実行時にセーブされていないファイルを読み込んで実行してしまう */
    requestedItem = item;
    fileTree->saveAllFile();
}

void GnuplotEditor::sendGnuplotCmd()
{
    /* FileTreeWidget::allSaved() が発せられたら呼ばれるが，これが他のクラスから発せられたものである可能性があるため，
     * requestedItemを確認することで，これが実行時にFileTreeWidget::saveAllFile()を呼ぶことによって発せられたことが
     * 確認できる． */
    if(requestedItem)
    {
        editorArea->singleShotLoading();

        __LOGOUT__("execute gnuplot \"" + requestedItem->fileInfo().absoluteFilePath() + "\".", Logger::LogLevel::Info);

        gnuplotExecutor->setWorkingFolderPath(requestedItem->fileInfo().absolutePath());
        gnuplotExecutor->execGnuplot(requestedItem->process, QList<QString>() << "load '" + requestedItem->fileInfo().absoluteFilePath() + "'", true);

        requestedItem = nullptr;
    }
}

void GnuplotEditor::findKeyword()
{
    __LOGOUT__("not surpported.", Logger::LogLevel::Debug);
}

void GnuplotEditor::showGnuplotCmdHelp()
{
    if(TreeScriptItem *item = qobject_cast<TreeScriptItem*>(editorArea->currentTreeFileItem()))
    {
        if(!item->editor) return;

        const QString cmd = (item->editor->textCursor().hasSelection()) ? item->editor->textCursor().selectedText()
                                                                        : item->editor->textUnderCursor();

        gnuplotExecutor->execGnuplot(item->process, QStringList() << "help " + cmd, false);
    }
}

void GnuplotEditor::showGnuplotHelpWindow()
{
    gnuplotExecutor->execGnuplot(QStringList() << "help", false);
}

void GnuplotEditor::reboot()
{
    __LOGOUT__("reboot requested.", Logger::LogLevel::Info);

    /* アプリを終了させる。アプリが終了する前にQMainWindow::closeEvent()が呼び出される */
    QCoreApplication::quit();

    /* 実行ファイルを指定して新しく立ち上げる */
    QProcess::startDetached(QApplication::applicationFilePath());
}

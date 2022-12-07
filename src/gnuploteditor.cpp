/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "gnuploteditor.h"
#include "imagedisplay.h"
#include <QMessageBox>
#include <QSplitter>
#include "tablesettingwidget.h"
#include "editorwidget.h"

GnuplotEditor::GnuplotEditor(QWidget *parent)
    : QMainWindow(parent)
    , gnuplot(new Gnuplot(nullptr))
    //, gnuplotProcess(nullptr)

    , editorSetting(new EditorSetting(nullptr))
    , gnuplotSetting(new GnuplotSettingWidget(nullptr))
    , templateCustom(new TemplateCustomWidget(this))
    , fileTreeSetting(new FileTreeSettingWidget(nullptr))

    , fileMenu(new FileMenu("File", this))
    , editorMenu(new EditorMenu("Editor", this))
    , gnuplotMenu(new GnuplotMenu("Gnuplot", this))
    , viewMenu(new ViewMenu("View", this))
    , helpMenu(new HelpMenu("Help", this))
{
    /* ウィンドウをスクリーン画面に対して(0.4,0.5)の比率サイズに設定 */
    setGeometry(getRectFromScreenRatio(screen()->size(), 0.4f, 0.5f));

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
    connect(fileTree, &FileTreeWidget::errorCaused, browserWidget, &BrowserWidget::outputText);
    connect(fileTree, &FileTreeWidget::folderPathChanged, fileTreeSetting, &FileTreeSettingWidget::setPreviousFolderPath);
    connect(gnuplot, &Gnuplot::standardOutputPassed, this, &GnuplotEditor::receiveGnuplotStdOut);
    connect(gnuplot, &Gnuplot::standardErrorPassed, this, &GnuplotEditor::receiveGnuplotStdErr);
    connect(gnuplot, &Gnuplot::errorCaused, browserWidget, &BrowserWidget::outputText);
    connect(browserWidget, &BrowserWidget::textChanged, [this](){ displayTab->setCurrentIndex(1); });
    connect(fileTreeSetting, &FileTreeSettingWidget::reloadRequested, fileTree, &FileTreeWidget::saveAndLoad);

    /* gnuplotとそのプロセスは別スレッドで非同期処理 */
    //gnuplotの設定GnuplotSettingWidget
    connect(gnuplot, &Gnuplot::cmdPushed, gnuplotSetting, &GnuplotSettingWidget::addLogToBrowser);
    connect(gnuplotSetting, &GnuplotSettingWidget::exePathSet, gnuplot, &Gnuplot::setExePath);
    connect(gnuplotSetting, &GnuplotSettingWidget::initCmdSet, gnuplot, &Gnuplot::setInitCmd);
    connect(gnuplotSetting, &GnuplotSettingWidget::preCmdSet, gnuplot, &Gnuplot::setPreCmd);
    gnuplotSetting->loadXmlSetting();
    //gnuplot
    connect(this, &GnuplotEditor::scriptPathChanged, gnuplot, &Gnuplot::setWorkingDirectory);
    connect(this, &GnuplotEditor::exeGnuplotRequested, gnuplot, &Gnuplot::exc);
    //gnuplotThread.start();
    //gnuplot->moveToThread(&gnuplotThread);
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

    //gnuplotThread.quit();
    //gnuplotThread.wait();
    delete gnuplot;
}

void GnuplotEditor::initializeMenuBar()
{
    if(menuBar()) delete menuBar();

    WindowMenuBar *menuBar = new WindowMenuBar(this);
    setMenuBar(menuBar);

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(editorMenu);
    menuBar->addMenu(gnuplotMenu);
    menuBar->addMenu(viewMenu);
    menuBar->addMenu(helpMenu);

    connect(fileMenu, &FileMenu::openFolderRequested, fileTree, &FileTreeWidget::openFolder);
    connect(fileMenu, &FileMenu::addFolderRequested, fileTree, &FileTreeWidget::addFolder);
    connect(fileMenu, &FileMenu::saveFolderRequested, fileTree, &FileTreeWidget::saveFolder);
    connect(fileMenu, &FileMenu::updateFolderRequested, fileTree, &FileTreeWidget::updateFileTree);
    connect(fileMenu, &FileMenu::reloadFolderRequested, fileTree, &FileTreeWidget::saveAndLoad);
    connect(fileMenu, &FileMenu::openFileRequested, fileTree, &FileTreeWidget::addFile);
    connect(fileMenu, &FileMenu::newFileRequested, fileTree, &FileTreeWidget::newFile);
    connect(fileMenu, &FileMenu::openTreeSettingRequested, fileTreeSetting, &FileTreeSettingWidget::show);

    connect(editorMenu, &EditorMenu::saveAllFileRequested, fileTree, &FileTreeWidget::saveAllFile);
    connect(editorMenu, &EditorMenu::findRequested, this, &GnuplotEditor::findKeyword);
    connect(editorMenu, &EditorMenu::runRequested, this, &GnuplotEditor::executeItem);
    connect(editorMenu, &EditorMenu::showCmdHelpRequested, this, &GnuplotEditor::showGnuplotCmdHelp);
    connect(editorMenu, &EditorMenu::saveAsTemplateRequested, templateCustom, &TemplateCustomWidget::addTemplate);
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
    //editorTab = new TabWidget(editorSplitter);
    editorArea = new EditorArea(editorSplitter);
    displayTab = new TabWidget(editorSplitter);

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
    //editorSplitter->addWidget(editorTab);
    editorSplitter->addWidget(editorArea);
    editorSplitter->addWidget(displayTab);

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


    /* 各ウィジェット内のアイテムの初期化 */
    //gnuplotWidget = new QStackedWidget(editorTab);
    //sheetWidget = new QStackedWidget(editorTab);
    consoleWidget = new ConsoleWidget(displayTab);
    browserWidget = new BrowserWidget(displayTab);

    //tableArea = new TableArea(editorTab, sheetWidget);

    //editorTab->addTab(gnuplotWidget, "&Gnuplot");
    //editorTab->addTab(tableArea, "&Sheet");
    //editorTab->addTab(sheetWidget, "&Sheet");
    displayTab->addTab(consoleWidget, "&Console");
    displayTab->addTab(browserWidget, "&Output");

    displayTab->setTabPosition(QTabWidget::TabPosition::South);

    connect(treeModelCombo, &QComboBox::currentIndexChanged, fileTree, &FileTreeWidget::setTreeModel);
    connect(editorArea, &EditorArea::executeRequested, this, &GnuplotEditor::executeItem);
}

void GnuplotEditor::setCurrentItem(QWidget *, QWidget *)
{
    editorMenu->setCurrentItem(editorArea->currentTreeFileItem());
}

void GnuplotEditor::setupScriptItem(TreeScriptItem *item)
{
    //item->editor = new TextEdit(gnuplotWidget);
    //item->editor = new TextEdit(nullptr);
    //item->process = new QProcess(nullptr);
    //item->process->moveToThread(&gnuplotThread);
    item->load();
    //item->editor->setParentFolderPath(item->info.absolutePath());

    connect(item, &TreeFileItem::errorCaused, browserWidget, &BrowserWidget::outputText);
    //connect(item->editor, &TextEdit::fontSizeChanged, editorSetting, &EditorSetting::setTextSize);
    //connect(item->editor, &TextEdit::commandHelpRequested, this, &GnuplotEditor::showCommandHelp);
    //connect(item, &TreeScriptItem::closeProcessRequested, item->process, &QProcess::close);
    //connect(item, &TreeScriptItem::destroyed, item->process, &QProcess::close);
    //connect(item, &TreeScriptItem::destroyed, item->process, &QProcess::deleteLater);
    //connect(item->editor, &TextEdit::textChanged, item, &TreeFileItem::setEdited);           //EditorSettingWidget::setEditor()より後に呼び出す。
    //connect(item->editor, &TextEdit::executeRequested, this, &GnuplotEditor::executeGnuplot);
}

void GnuplotEditor::receiveTreeItem(QTreeWidgetItem *item, const int column)
{
    if(column != 0) return;

    switch((FileTreeWidget::TreeItemType)item->type())
    {
    case FileTreeWidget::TreeItemType::Script:
        setEditorWidget(static_cast<TreeScriptItem*>(item)); break;
    case FileTreeWidget::TreeItemType::Sheet:
        setSheetWidget(static_cast<TreeSheetItem*>(item)); break;
    case FileTreeWidget::TreeItemType::Image:
        setImageWidget(static_cast<TreeImageItem*>(item)); break;
    default:
        break;
    }
}

void GnuplotEditor::setEditorWidget(TreeScriptItem *item)
{
    if(!item) return;

    /* タブをGnuplotに設定 */
    //editorTab->setCurrentIndex(0);

    //if(item == currentScript) return;

    //currentScript = item;

    /* 前にセットされてたものは取り除く */
    //if(QWidget *w = gnuplotWidget->widget(gnuplotWidget->currentIndex()))
    //    gnuplotWidget->removeWidget(w);

    /* 初めて表示する場合 */
    //if(!item->editor) setupScriptItem(item);

    /* エディターの設定対象を変更 */
    //editorSetting->setEditor(item->editor);

    /* 新しくセット */
    //gnuplotWidget->addWidget(item->editor);       //editorのparentは自動的にgnuplotWidgetとなる

    /* プロセスをセット */
    //gnuplotProcess = item->process;

    /* Editorメニュー */
    //editorMenu->setEditor(item->editor);

    /* メニューバーの名前変更 */
    //menuBarWidget->setScript(item);

    //emit scriptPathChanged(item->info.absolutePath());

    //DEBUG
    editorSetting->addEditor(item->editor);

    //DEBUG
    editorArea->setWidget(item->editor, item);
    editorMenu->setCurrentItem(item);
}

void GnuplotEditor::setupSheetItem(TreeSheetItem *item)
{
    //connect(currentSheet, &TreeFileItem::errorCaused, browserWidget, &BrowserWidget::outputText);
    connect(item, &TreeSheetItem::errorCaused, browserWidget, &BrowserWidget::outputText);
    item->table = new GnuplotTable(gnuplot, nullptr);
    connect(item->table, &GnuplotTable::itemChanged, item, &TreeFileItem::setEdited);
    item->load();
    //tableArea->setupSheetWidget(item->table);
}

void GnuplotEditor::setSheetWidget(TreeSheetItem *item)
{
    if(!item) return;

    /* タブをSheetに設定 */
    //editorTab->setCurrentIndex(1);

    //if(item == currentSheet) return;

    //currentSheet = item;

    /* 前にセットされてたものは削除 */
    //if(QWidget *w = sheetWidget->widget(sheetWidget->currentIndex()))
    //    sheetWidget->removeWidget(w);

    /* 初めて表示する場合 */
    if(!item->table) setupSheetItem(item);

    /* 新しくセット */
    //sheetWidget->addWidget(item->table);

    /* メニューバーの名前変更 */
    //menuBarWidget->setSheet(item);
    //menuBarWidget->changeAutoUpdateSheetMenuText(item->table->isEnablenotifyUpdating());

    //DEBUG
    editorArea->setWidget(item->table, item);
    editorMenu->setCurrentItem(item);

    //connect(item->table, &GnuplotTable::tableUpdated, this, &GnuplotEditor::executeGnuplot);
    connect(gnuplotSetting, &GnuplotSettingWidget::autoCompileMsecSet, item->table, &GnuplotTable::setUpdateMsec);
}

void GnuplotEditor::setImageWidget(TreeImageItem *item)
{
    if(!item) return;

    if(item->imageDisplay)
    {
        item->imageDisplay->updateImage();
        item->imageDisplay->show(); //ウィンドウが閉じられていたら表示
        item->imageDisplay->activateWindow(); //他アプリ含めて一番前面にする
        item->imageDisplay->raise(); //同アプリ内で一番上に
    }
    else
    {
        item->imageDisplay = new ImageDisplay(nullptr);
        //item->imageDisplay->setImagePath(item->info.absoluteFilePath());
        item->imageDisplay->setImagePath(item->fileInfo().absoluteFilePath());
        item->imageDisplay->show();
    }
}

//void GnuplotEditor::executeGnuplot()
//{
//    if(TreeFileItem *i = editorArea->currentTreeFileItem())
//    {
//        qDebug() << i->text(0);
//    }
//    else
//        qDebug() << static_cast<TreeFileItem*>(fileTree->currentItem())->text(0);
//
//    return;
//
//    //if(!currentScript) return;
//
//    /* browserの過去の出力をグレイアウト */
//    browserWidget->grayOutAll();
//
//    /* ファイルが選ばれていない場合は無効 */
//    //if(gnuplotWidget->count() < 1) {
//    //    browserWidget->outputText("scripts are not selected.", BrowserWidget::MessageType::SystemErr);
//    //    return;
//    //}
//
//    /* エラー行のリセット */
//    //qobject_cast<TextEdit*>(gnuplotWidget->currentWidget())->setErrorLineNumber(-1);
//    //qobject_cast<TextEdit*>(gnuplotWidget->currentWidget())->highlightLine();
//    //if(TextEdit *editor = currentScript->editor)
//    //{
//    //    editor->setErrorLineNumber(-1);
//    //    editor->highlightLine();
//    //}
//
//    /* ファイルの保存 */
//    fileTree->saveAllFile();
//
//    /* gnuplotにコマンドを渡す */
//    //emit exeGnuplotRequested(gnuplotProcess, QList<QString>() << "load '" + currentScript->info.absoluteFilePath() + "'");
//}

void GnuplotEditor::executeItem(TreeFileItem *item)
{
    if(!item) return;

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

    browserWidget->grayOutAll();

    if(TextEdit *textEdit = item->editor)
    {
        textEdit->setErrorLineNumber(-1);
        textEdit->highlightLine();
    }

    fileTree->saveAllFile();

    emit scriptPathChanged(item->fileInfo().absolutePath());

    //emit exeGnuplotRequested(item->process, QList<QString>() << "load '" + item->info.absoluteFilePath() + "'", true);
    emit exeGnuplotRequested(item->process, QList<QString>() << "load '" + item->fileInfo().absoluteFilePath() + "'", true);
}

void GnuplotEditor::receiveGnuplotStdOut(const QString& text)
{
    /* 出力の表示 */
    browserWidget->outputText(text, BrowserWidget::MessageType::GnuplotStdOut);
}

void GnuplotEditor::receiveGnuplotStdErr(const QString& text, const int line)
{
    /* 出力の表示 */
    browserWidget->outputText(text, BrowserWidget::MessageType::GnuplotStdErr);

    /* エラー行の設定とハイライト */
    //if(TextEdit *currentEditor = qobject_cast<TextEdit*>(gnuplotWidget->widget(0)))
    //{
    //    currentEditor->setErrorLineNumber(line - 1);
    //    currentEditor->highlightLine();
    //}
    //if(!currentScript) return;
    //if(TextEdit *editor = currentScript->editor)
    //{
    //    editor->setErrorLineNumber(line - 1);
    //    editor->highlightLine();
    //}
}

void GnuplotEditor::findKeyword()
{

}

void GnuplotEditor::showGnuplotCmdHelp()
{
    if(TreeScriptItem *item = qobject_cast<TreeScriptItem*>(editorArea->currentTreeFileItem()))
    {
        if(!item->editor) return;

        const QString cmd = (item->editor->textCursor().hasSelection()) ? item->editor->textCursor().selectedText()
                                                                        : item->editor->textUnderCursor();

        emit exeGnuplotRequested(item->process, QStringList() << "help " + cmd, false);
    }
}

void GnuplotEditor::reboot()
{
    /* アプリを終了させる。アプリが終了する前にQMainWindow::closeEvent()が呼び出される */
    QCoreApplication::quit();

    /* 実行ファイルを指定して新しく立ち上げる */
    QProcess::startDetached(QApplication::applicationFilePath());
}































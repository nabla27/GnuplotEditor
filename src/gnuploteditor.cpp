#include "gnuploteditor.h"
#include "imagedisplay.h"
#include <QMessageBox>
#include <QSplitter>

GnuplotEditor::GnuplotEditor(QWidget *parent)
    : QMainWindow(parent)
    , gnuplot(new Gnuplot(this))
    , editorSetting(new EditorSetting(nullptr))
    , gnuplotSetting(new GnuplotSettingWidget(gnuplot, nullptr))
    , templateCustom(new TemplateCustomWidget(this))
    , fileTreeSetting(new FileTreeSettingWidget(nullptr))
{
    /* ウィンドウをスクリーン画面に対して(0.4,0.5)の比率サイズに設定 */
    setGeometry(getRectFromScreenRatio(screen()->size(), 0.4f, 0.5f));

    /* レイアウト生成 */
    initializeLayout();

    /* メニュバーの生成 */
    initializeMenuBar();

    /* workingDirectoryの設定 */
    const QString path = QApplication::applicationDirPath() + "/gnuplot-project";
    //フォルダーが存在していなければ作成
    QDir workingDir(path);
    if(!workingDir.exists()) workingDir.mkpath(path);
    //初期化
    fileTree->setFolderPath(path);
    gnuplot->setWorkingDirectory(path);

    connect(this, &GnuplotEditor::workingDirectoryChanged, fileTree, &FileTreeWidget::setFolderPath);
    connect(this, &GnuplotEditor::workingDirectoryChanged, gnuplot, &Gnuplot::setWorkingDirectory);
    connect(fileTree, &FileTreeWidget::scriptSelected, this, &GnuplotEditor::setEditorWidget);
    connect(fileTree, &FileTreeWidget::sheetSelected, this, &GnuplotEditor::setSheetWidget);
    connect(fileTree, &FileTreeWidget::otherSelected, this, &GnuplotEditor::setOtherWidget);
    connect(fileTree, &FileTreeWidget::errorCaused, browserWidget, &BrowserWidget::outputText);
    connect(gnuplot, &Gnuplot::standardOutputPassed, this, &GnuplotEditor::receiveGnuplotStdOut);
    connect(gnuplot, &Gnuplot::standardErrorPassed, this, &GnuplotEditor::receiveGnuplotStdErr);
    connect(gnuplot, &Gnuplot::errorCaused, browserWidget, &BrowserWidget::outputText);
    connect(browserWidget, &BrowserWidget::textChanged, [this](){ displayTab->setCurrentIndex(1); });
    connect(templateCustom, &TemplateCustomWidget::importTemplateRequested, this, &GnuplotEditor::importTemplate);

    fileTreeSetting->show();
}

GnuplotEditor::~GnuplotEditor()
{
    postProcessing();
}

void GnuplotEditor::postProcessing()
{
    editorSetting->hide();
    gnuplotSetting->hide();
    delete editorSetting;
    delete gnuplotSetting;
}

void GnuplotEditor::initializeMenuBar()
{
    if(menuBar()) delete menuBar();

    WindowMenuBar *menuBar = new WindowMenuBar(this);

    FileMenu *fileMenu = new FileMenu("File", menuBar);
    WidgetMenu *widgetMenu = new WidgetMenu("Widget", menuBar);
    HelpMenu *helpMenu = new HelpMenu("Help", menuBar);
    menuBarWidget = new MenuBarWidget(menuBar);

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(widgetMenu);
    menuBar->addMenu(helpMenu);
    menuBar->setCornerWidget(menuBarWidget);

    setMenuBar(menuBar);

    connect(fileMenu, &FileMenu::openFolderPushed, fileTree, &FileTreeWidget::setFolderPath);
    connect(fileMenu, &FileMenu::updateFolderPushed, fileTree, &FileTreeWidget::updateFileTree);
    connect(fileMenu, &FileMenu::addFolderPushed, fileTree, &FileTreeWidget::addFolder);
    connect(fileMenu, &FileMenu::saveFolderPushed, fileTree, &FileTreeWidget::saveFolder);
    connect(fileMenu, &FileMenu::reloadFolderPushed, fileTree, &FileTreeWidget::saveAndLoad);
    connect(widgetMenu, &WidgetMenu::clearOutputWindowRequested, browserWidget, &BrowserWidget::clear);
    //connect(widgetMenu, &WidgetMenu::clearConsoleWindowPushed, consoleWidget, &);
    connect(widgetMenu, &WidgetMenu::openEditorSettingRequested, editorSetting, &EditorSetting::show);
    connect(widgetMenu, &WidgetMenu::openGnuplotSettingRequested, gnuplotSetting, &GnuplotSettingWidget::show);
    connect(widgetMenu, &WidgetMenu::openTemplateCustomRequested, templateCustom, &TemplateCustomWidget::show);
    connect(helpMenu, &HelpMenu::rebootRequested, this, &GnuplotEditor::reboot);

    connect(menuBarWidget, &MenuBarWidget::closeProcessRequested, this, &GnuplotEditor::closeCurrentProcess);
    connect(menuBarWidget, &MenuBarWidget::saveAsTemplateRequested, this, &GnuplotEditor::saveAsTemplate);
    connect(menuBarWidget, &MenuBarWidget::openInNewWindowRequested, this, &GnuplotEditor::moveSheetToNewWindow);
    connect(menuBarWidget, &MenuBarWidget::autoSheetUpdateRequested, this, &GnuplotEditor::changeSheetAutoUpdating);
    connect(menuBarWidget, &MenuBarWidget::runRequested, this, &GnuplotEditor::executeGnuplot);
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
    editorTab = new TabWidget(editorSplitter);
    displayTab = new TabWidget(editorSplitter);

    /* 配置 */
    centralWidget()->setLayout(hLayout);
    hLayout->addWidget(mainSplitter);
    mainSplitter->addWidget(fileTreeContents);
    mainSplitter->addWidget(editorSplitter);

    //右側のファイルツリー
    fileTreeContents->setLayout(treeLayout);
    treeLayout->addWidget(treeModelCombo);
    treeLayout->addWidget(fileTree);

    //左側のファイルツリー
    editorSplitter->addWidget(editorTab);
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
    gnuplotWidget = new QStackedWidget(editorTab);
    sheetWidget = new QStackedWidget(editorTab);
    consoleWidget = new ConsoleWidget(displayTab);
    browserWidget = new BrowserWidget(displayTab);

    editorTab->addTab(gnuplotWidget, "&Gnuplot");
    editorTab->addTab(sheetWidget, "&Sheet");
    displayTab->addTab(consoleWidget, "&Console");
    displayTab->addTab(browserWidget, "&Output");

    displayTab->setTabPosition(QTabWidget::TabPosition::South);

    connect(treeModelCombo, &QComboBox::currentIndexChanged, fileTree, &FileTreeWidget::setTreeModel);
}

void GnuplotEditor::setEditorWidget(TreeScriptItem *item)
{
    if(!item) return;
    currentScript = item;

    /* 前にセットされてたものは取り除く */
    if(QWidget *w = gnuplotWidget->widget(gnuplotWidget->currentIndex()))
        gnuplotWidget->removeWidget(w);

    /* 初めて表示する場合 */
    if(!item->editor)
    {
        connect(currentScript, &TreeFileItem::errorCaused, browserWidget, &BrowserWidget::outputText);
        item->editor = new TextEdit(gnuplotWidget);
        item->load();
        connect(item->editor, &TextEdit::fontSizeChanged, editorSetting, &EditorSetting::setTextSize);
        connect(item->editor, &TextEdit::commandHelpRequested, this, &GnuplotEditor::showCommandHelp);
        editorSetting->setEditor(item->editor);
        gnuplot->setWorkingDirectory(item->info.absolutePath());
        item->editor->setParentFolderPath(item->info.absolutePath());
    }

    /* 新しくセット */
    gnuplotWidget->addWidget(item->editor);       //editorのparentは自動的にgnuplotWidgetとなる

    /* プロセスをセット */
    gnuplotProcess = item->process;

    /* タブをGnuplotに設定 */
    editorTab->setCurrentIndex(0);

    /* メニューバーの名前変更 */
    menuBarWidget->setScript(item);
}

void GnuplotEditor::setSheetWidget(TreeSheetItem *item)
{
    if(!item) return;
    currentSheet = item;

    /* 前にセットされてたものは削除 */
    if(QWidget *w = sheetWidget->widget(sheetWidget->currentIndex()))
        sheetWidget->removeWidget(w);

    /* 初めて表示する場合 */
    if(!item->table)
    {
        connect(currentSheet, &TreeFileItem::errorCaused, browserWidget, &BrowserWidget::outputText);
        item->table = new GnuplotTable(sheetWidget);
        item->load();
    }

    /* 新しくセット */
    sheetWidget->addWidget(item->table);
    item->table->setGnuplot(gnuplot);

    /* タブをSheetに設定 */
    editorTab->setCurrentIndex(1);

    /* メニューバーの名前変更 */
    menuBarWidget->setSheet(item);
    menuBarWidget->changeAutoUpdateSheetMenuText(item->table->isEnablenotifyUpdating());

    connect(item->table, &GnuplotTable::tableUpdated, this, &GnuplotEditor::executeGnuplot);
    connect(gnuplotSetting, &GnuplotSettingWidget::autoCompileMsecSet, item->table, &GnuplotTable::setUpdateMsec);
}

void GnuplotEditor::setOtherWidget(TreeFileItem *item)
{
    if(!item) return; //消された場合

    const QString suffix = item->info.completeSuffix(); //拡張子

    if(ImageDisplay::isValidExtension(suffix))
    {   //画像の表示
        ImageDisplay *imageDisplay = new ImageDisplay(nullptr);  //ウィンドウ閉じたら自動でdeleteされるよ!
        imageDisplay->setImageFile(item->info.absoluteFilePath());
        imageDisplay->show();
    }
}

void GnuplotEditor::executeGnuplot()
{
    if(!currentScript) return;

    /* browserの過去の出力をグレイアウト */
    browserWidget->grayOutAll();

    /* ファイルが選ばれていない場合は無効 */
    if(gnuplotWidget->count() < 1) {
        browserWidget->outputText("scripts are not selected.", BrowserWidget::MessageType::SystemErr);
        return;
    }

    /* エラー行のリセット */
    qobject_cast<TextEdit*>(gnuplotWidget->currentWidget())->setErrorLineNumber(-1);
    qobject_cast<TextEdit*>(gnuplotWidget->currentWidget())->highlightLine();

    /* ファイルの保存 */
    fileTree->saveAllFile();

    /* gnuplotにコマンドを渡す */
    gnuplot->exc(gnuplotProcess, QList<QString>() << "load '" + currentScript->info.absoluteFilePath() + "'");
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
    qobject_cast<TextEdit*>(gnuplotWidget->widget(0))->setErrorLineNumber(line - 1);
    qobject_cast<TextEdit*>(gnuplotWidget->widget(0))->highlightLine();
}

void GnuplotEditor::closeCurrentProcess()
{
    if(gnuplotProcess)
        gnuplotProcess->close();
}

void GnuplotEditor::moveSheetToNewWindow()
{
    if(QWidget *widget = sheetWidget->currentWidget())
    {
        widget->setParent(nullptr);
        widget->show();
        widget->setWindowTitle("GnuplotEditor  " + currentSheet->info.fileName());
        menuBarWidget->setSheet(nullptr);
    }
}

void GnuplotEditor::changeSheetAutoUpdating()
{
    if(GnuplotTable *table = qobject_cast<GnuplotTable*>(sheetWidget->currentWidget()))
    {
        table->changeUpdateNotification();
        menuBarWidget->changeAutoUpdateSheetMenuText(table->isEnablenotifyUpdating());
    }
}

void GnuplotEditor::importTemplate(const QString& script)
{
    TextEdit *currentEditor = qobject_cast<TextEdit*>(gnuplotWidget->currentWidget());

    if(!currentEditor)
    {
        QMessageBox::critical(this, "Error", "script is not selected.");
        return;
    }

    currentEditor->insertPlainText(script);
}

void GnuplotEditor::saveAsTemplate()
{
    TextEdit *currentEditor = qobject_cast<TextEdit*>(gnuplotWidget->currentWidget());

    if(!currentEditor)
    {
        QMessageBox::critical(this, "Error", "script is not selected.");
        return;
    }

    templateCustom->addTemplate(currentEditor->toPlainText());
}

void GnuplotEditor::showCommandHelp(const QString& command)
{
    gnuplot->exc(gnuplotProcess, QList<QString>() << "help " + command + "\n", false);
}

void GnuplotEditor::reboot()
{
    postProcessing();

    QProcess::startDetached(QApplication::applicationFilePath());

    exit(0);
}































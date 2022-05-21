#include "gnuploteditor.h"
#include "imagedisplay.h"
#include <QMessageBox>

GnuplotEditor::GnuplotEditor(QWidget *parent)
    : QMainWindow(parent)
    , gnuplot(new Gnuplot(this))
    , editorSetting(new EditorSettingWidget(nullptr))
    , gnuplotSetting(new GnuplotSettingWidget(gnuplot, nullptr))
    , templateCustom(new TemplateCustomWidget(this))
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
    connect(fileTree, &FileTreeWidget::fileNameChanged, this, &GnuplotEditor::setMenuBarTitle);
    connect(fileTree, &FileTreeWidget::errorCaused, browserWidget, &BrowserWidget::outputText);
    connect(gnuplot, &Gnuplot::standardOutputPassed, this, &GnuplotEditor::receiveGnuplotStdOut);
    connect(gnuplot, &Gnuplot::standardErrorPassed, this, &GnuplotEditor::receiveGnuplotStdErr);
    connect(gnuplot, &Gnuplot::errorCaused, browserWidget, &BrowserWidget::outputText);
    connect(browserWidget, &BrowserWidget::textChanged, [this](){ displayTab->setCurrentIndex(1); });
    connect(templateCustom, &TemplateCustomWidget::importTemplateRequested, this, &GnuplotEditor::importTemplate);
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

    FileMenu *const fileMenu = new FileMenu("File", menuBar);
    WidgetMenu *const widgetMenu = new WidgetMenu("Widget", menuBar);
    HelpMenu *const helpMenu = new HelpMenu("Help", menuBar);
    QAction *const blank1 = new QAction("         ", menuBar);
    scriptMenu = new ScriptMenu("      ", menuBar);
    sheetMenu = new SheetMenu("     ", menuBar);
    QAction *const blank2 = new QAction("         ", menuBar);
    QAction *const runAction = new QAction("&Run", menuBar);

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(widgetMenu);
    menuBar->addMenu(helpMenu);
    menuBar->addAction(blank1);
    menuBar->addMenu(scriptMenu);
    menuBar->addMenu(sheetMenu);
    menuBar->addAction(blank2);
    menuBar->addAction(runAction);

    setMenuBar(menuBar);

    connect(fileMenu, &FileMenu::openFolderPushed, fileTree, &FileTreeWidget::setFolderPath);
    connect(fileMenu, &FileMenu::updateFolderPushed, fileTree, &FileTreeWidget::updateFileTree);
    connect(fileMenu, &FileMenu::addFolderPushed, fileTree, &FileTreeWidget::addFolder);
    connect(fileMenu, &FileMenu::saveFolderPushed, fileTree, &FileTreeWidget::saveFolder);
    connect(fileMenu, &FileMenu::reloadFolderPushed, fileTree, &FileTreeWidget::saveAndLoad);
    connect(widgetMenu, &WidgetMenu::clearOutputWindowRequested, browserWidget, &BrowserWidget::clear);
    //connect(widgetMenu, &WidgetMenu::clearConsoleWindowPushed, consoleWidget, &);
    connect(widgetMenu, &WidgetMenu::openEditorSettingRequested, editorSetting, &EditorSettingWidget::show);
    connect(widgetMenu, &WidgetMenu::openGnuplotSettingRequested, gnuplotSetting, &GnuplotSettingWidget::show);
    connect(widgetMenu, &WidgetMenu::openTemplateCustomRequested, templateCustom, &TemplateCustomWidget::show);
    connect(helpMenu, &HelpMenu::rebootRequested, this, &GnuplotEditor::reboot);
    connect(scriptMenu, &ScriptMenu::closeProcessRequested, this, &GnuplotEditor::closeCurrentProcess);
    connect(scriptMenu, &ScriptMenu::saveAsTemplateRequested, this, &GnuplotEditor::saveAsTemplate);
    connect(sheetMenu, &SheetMenu::openInNewWindowRequested, this, &GnuplotEditor::moveSheetToNewWindow);
    connect(sheetMenu, &SheetMenu::autoTableUpdateRequested, this, &GnuplotEditor::changeSheetAutoUpdating);
    connect(runAction, &QAction::triggered, this, &GnuplotEditor::executeGnuplot);
}

void GnuplotEditor::initializeLayout()
{
    if(centralWidget()) delete centralWidget();

    setCentralWidget(new QWidget(this));

    /* ウィンドウ内の大枠となるレイアウトとウィジェットの初期化 */
    QHBoxLayout *hLayout = new QHBoxLayout;
    QVBoxLayout *treeLayout = new QVBoxLayout;
    fileTreeContents = new QWidget(centralWidget());
    treeModelCombo = new QComboBox(fileTreeContents);
    fileTree = new FileTreeWidget(fileTreeContents);
    VerticalDragBar *fileTreeDragBar = new VerticalDragBar(centralWidget());
    QVBoxLayout *vLayout = new QVBoxLayout;
    editorTab = new QTabWidget(this);
    HorizontalDragBar *displayTabDragBar = new HorizontalDragBar(centralWidget());
    displayTab = new QTabWidget(centralWidget());

    /* 配置 */
    centralWidget()->setLayout(hLayout);
    hLayout->addLayout(treeLayout);
    hLayout->addWidget(fileTreeContents);
    fileTreeContents->setLayout(treeLayout);
    treeLayout->addWidget(treeModelCombo);
    treeLayout->addWidget(fileTree);
    hLayout->addWidget(fileTreeDragBar);
    hLayout->addLayout(vLayout);
    vLayout->addWidget(editorTab);
    vLayout->addWidget(displayTabDragBar);
    vLayout->addWidget(displayTab);
    /* 設定 */
    hLayout->setSpacing(0);
    fileTree->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    fileTreeContents->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    fileTreeContents->setMaximumWidth(150);
    vLayout->setSpacing(0);

    /* 各ウィジェット内のアイテムの初期化 */
    gnuplotWidget = new QStackedWidget(editorTab);
    sheetWidget = new QStackedWidget(editorTab);
    consoleWidget = new ConsoleWidget(displayTab);
    browserWidget = new BrowserWidget(displayTab);
    /* 配置 */
    editorTab->addTab(gnuplotWidget, "&Gnuplot");
    editorTab->addTab(sheetWidget, "&Sheet");
    displayTab->addTab(consoleWidget, "&Console");
    displayTab->addTab(browserWidget, "&Output");
    /* 設定 */
    displayTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    displayTab->setMaximumHeight(150);
    editorTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    centralWidget()->layout()->setContentsMargins(0, 0, 0, 0);
    displayTab->setTabPosition(QTabWidget::TabPosition::South);

    treeModelCombo->addItems(enumToStrings(FileTreeWidget::FileTreeModel(0)));

    /* 配色設定 */
    //setPalette(QPalette(QPalette::Window, Qt::black));
    //fileTree->setPalette(QPalette(QPalette::Window, Qt::white));
    //for(int i = 0; i < editorTab->count(); ++i) editorTab->tabBar()->setTabTextColor(i, Qt::black);
    //for(int i = 0; i < displayTab->count(); ++i) displayTab->tabBar()->setTabTextColor(i, Qt::black);
    //sheetWidget->setPalette(QPalette(QPalette::Window, Qt::white));
    //browserWidget->setPalette(QPalette(QPalette::Window, Qt::white));

    connect(treeModelCombo, &QComboBox::currentIndexChanged, fileTree, &FileTreeWidget::setTreeModel);
    connect(fileTreeDragBar, &VerticalDragBar::barDraged, this, &GnuplotEditor::setFileTreeWidth);
    connect(displayTabDragBar, &HorizontalDragBar::barDraged, this, &GnuplotEditor::setDisplayTabHeight);
}

void GnuplotEditor::connectEditorSetting(TextEdit *const editor)
{
    //editor->setWorkingDirectory(fileTree->currentFolderPath());
    connect(editorSetting, &EditorSettingWidget::backgroundColorSet, editor, &TextEdit::setBackgroundColor);
    connect(editorSetting, &EditorSettingWidget::textColorSet, editor, &TextEdit::setTextColor);
    connect(editorSetting, &EditorSettingWidget::textSizeSet, editor, &TextEdit::setTextSize);
    connect(editorSetting, &EditorSettingWidget::tabSpaceSet, editor, &TextEdit::setTabSpace);
    connect(editorSetting, &EditorSettingWidget::checkWrapSet, editor, &TextEdit::setWrap);
    connect(editorSetting, &EditorSettingWidget::checkItaricSet, editor, &TextEdit::setItaric);
    connect(editorSetting, &EditorSettingWidget::checkBoldSet, editor, &TextEdit::setBold);
    connect(editorSetting, &EditorSettingWidget::mainCmdColorSet, editor, &TextEdit::setMainCmdColor);
    connect(editorSetting, &EditorSettingWidget::commentColorSet, editor, &TextEdit::setCommentColor);
    connect(editorSetting, &EditorSettingWidget::cursorLineColorSet, editor, &TextEdit::setCursorLineColor);
    connect(editorSetting, &EditorSettingWidget::stringColorSet, editor, &TextEdit::setStringColor);
    connect(editor, &TextEdit::fontSizeChanged, editorSetting, &EditorSettingWidget::setTextSize);
    editorSetting->set(editor);
}

void GnuplotEditor::setEditorWidget(TreeScriptItem *item)
{
    if(!item) return;

    /* 前にセットされてたものは削除 */
    if(QWidget *w = gnuplotWidget->widget(gnuplotWidget->currentIndex()))
        gnuplotWidget->removeWidget(w);

    /* 初めて表示する場合 */
    if(!item->editor)
    {
        item->editor = new TextEdit(gnuplotWidget);
        item->load();
    }

    /* 新しくセット */
    gnuplotWidget->addWidget(item->editor);       //editorのparentは自動的にgnuplotWidgetとなる
    connectEditorSetting(item->editor);

    /* 選択されているスクリプトの親ディレクトリを作業用ディレクトリに設定する */
    item->editor->setWorkingDirectory(item->info.absolutePath());
    gnuplot->setWorkingDirectory(item->info.absolutePath());

    /* プロセスをセット */
    gnuplotProcess = item->process;

    /* タブをGnuplotに設定 */
    editorTab->setCurrentIndex(0);

    /* メニューバーの名前変更 */
    scriptMenu->setTitle(item->info.fileName());
}

void GnuplotEditor::setSheetWidget(TreeSheetItem *item)
{
    if(!item) return;

    /* 前にセットされてたものは削除 */
    if(QWidget *w = sheetWidget->widget(sheetWidget->currentIndex()))
        sheetWidget->removeWidget(w);

    /* 初めて表示する場合 */
    if(!item->table)
    {
        item->table = new GnuplotTable(sheetWidget);
        item->load();
    }

    /* 新しくセット */
    sheetWidget->addWidget(item->table);
    item->table->setGnuplot(gnuplot);

    /* タブをSheetに設定 */
    editorTab->setCurrentIndex(1);

    /* メニューバーの名前変更 */
    sheetMenu->setTitle(item->info.fileName());
    sheetMenu->setAutoUpdateMenuText(item->table->isEnablenotifyUpdating());

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

void GnuplotEditor::setMenuBarTitle(const QString& oldName, const QString& newName)
{
    if(scriptMenu->title() == oldName) scriptMenu->setTitle(newName);
    else if(sheetMenu->title() == oldName) sheetMenu->setTitle(newName);
}

void GnuplotEditor::executeGnuplot()
{
    /* browserの過去の出力をグレイアウト */
    browserWidget->grayOutAll();

    /* ファイルが選ばれていない場合は無効 */
    if(gnuplotWidget->count() < 1) {
        browserWidget->outputText("scripts are not selected.", BrowserWidget::MessageType::SystemErr);
        return;
    }

    /* エラー行のリセット */
    qobject_cast<TextEdit*>(gnuplotWidget->widget(0))->setErrorLineNumber(-1);
    qobject_cast<TextEdit*>(gnuplotWidget->widget(0))->highlightLine();

    /* ファイルの保存 */
    fileTree->saveAllFile();

    /* gnuplotにコマンドを渡す */
    gnuplot->exc(gnuplotProcess, QList<QString>() << "load '" + scriptMenu->title() + "'");
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
        widget->setWindowTitle("GnuplotEditor  " + sheetMenu->title());
        sheetMenu->setTitle("");
    }
}

void GnuplotEditor::changeSheetAutoUpdating()
{
    if(GnuplotTable *table = qobject_cast<GnuplotTable*>(sheetWidget->currentWidget()))
    {
        table->changeUpdateNotification();
        sheetMenu->setAutoUpdateMenuText(table->isEnablenotifyUpdating());
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

void GnuplotEditor::setFileTreeWidth(const int dx)
{
    const int nextWidth = fileTreeContents->maximumWidth() - dx;
    if(nextWidth < 0 || nextWidth > 260) return;
    fileTreeContents->setMaximumWidth(nextWidth);
    fileTree->setMaximumWidth(nextWidth);
    treeModelCombo->setMaximumWidth(nextWidth);
    fileTree->setColumnWidth(0, nextWidth + 50);
}

void GnuplotEditor::setDisplayTabHeight(const int dy)
{
    const int nextHeight = displayTab->maximumHeight() + dy;
    if(nextHeight < 0 || nextHeight > 600) return;
    displayTab->setMaximumHeight(nextHeight);
}

void GnuplotEditor::reboot()
{
    postProcessing();

    QProcess::startDetached(QApplication::applicationFilePath());

    exit(0);
}































#include "menubar.h"


FileMenu::FileMenu(const QString& title, QWidget *parent)
    : QMenu(title, parent)
{
    QAction *openFolder = new QAction("Open folder", this);
    addAction(openFolder);
    connect(openFolder, &QAction::triggered, this, &FileMenu::openFolder);

    QAction *addFolder = new QAction("Add folder", this);
    addAction(addFolder);
    connect(addFolder, &QAction::triggered, this, &FileMenu::addFolderPushed);

    QAction *saveFolder = new QAction("Save folder", this);
    addAction(saveFolder);
    connect(saveFolder, &QAction::triggered, this, &FileMenu::saveFolderPushed);

    QAction *updateFolder = new QAction("Update folder", this);
    addAction(updateFolder);
    connect(updateFolder, &QAction::triggered, this, &FileMenu::updateFolderPushed);

    QAction *reloadFolder = new QAction("Reload folder", this);
    addAction(reloadFolder);
    connect(reloadFolder, &QAction::triggered, this, &FileMenu::reloadFolderPushed);
}

void FileMenu::openFolder()
{
    /* フォルダーの選択 */
    const QString folderPath = QFileDialog::getExistingDirectory(this);

    if(folderPath.isEmpty()) return;

    emit openFolderPushed(folderPath);
}







WidgetMenu::WidgetMenu(const QString& title, QWidget *parent)
    : QMenu(title, parent)
{
    QAction *clearConsoleWindow = new QAction("Clear Console Widget", this);
    addAction(clearConsoleWindow);
    connect(clearConsoleWindow, &QAction::triggered, this, &WidgetMenu::clearConsoleWindowRequested);

    QAction *clearOutputWindow = new QAction("Clear Output Widget", this);
    addAction(clearOutputWindow);
    connect(clearOutputWindow, &QAction::triggered, this, &WidgetMenu::clearOutputWindowRequested);

    QAction *openEditorSetting = new QAction("Editor Setting", this);
    addAction(openEditorSetting);
    connect(openEditorSetting, &QAction::triggered, this, &WidgetMenu::openEditorSettingRequested);

    QAction *openGnuplotSetting = new QAction("Gnuplot Setting", this);
    addAction(openGnuplotSetting);
    connect(openGnuplotSetting, &QAction::triggered, this, &WidgetMenu::openGnuplotSettingRequested);

    QAction *openTemplateCustom = new QAction("Script Template", this);
    addAction(openTemplateCustom);
    connect(openTemplateCustom, &QAction::triggered, this, &WidgetMenu::openTemplateCustomRequested);
}





HelpMenu::HelpMenu(const QString& title, QWidget *parent)
    : QMenu(title, parent)
{
    QAction *rebootAction = new QAction("Reboot", this);
    addAction(rebootAction);
    connect(rebootAction, &QAction::triggered, this, &HelpMenu::rebootRequested);
}













MenuBarWidget::MenuBarWidget(QWidget *parent)
    : QWidget(parent)
    , scriptButton(new QPushButton(this))
    , sheetButton(new QPushButton(this))
    , scriptIcon(new mlayout::IconLabel(this))
    , sheetIcon(new mlayout::IconLabel(this))
    , spacer(new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed))
    , runButton(new QPushButton(this))
    , emptyMenu(new QMenu(this))
    , scriptMenu(new QMenu(scriptButton))
    , sheetMenu(new QMenu(sheetButton))
{
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    setLayout(hLayout);

    hLayout->addWidget(scriptIcon);
    hLayout->addWidget(scriptButton);
    hLayout->addWidget(sheetIcon);
    hLayout->addWidget(sheetButton);
    hLayout->addItem(spacer);
    hLayout->addWidget(runButton);

    hLayout->setSpacing(0);
    hLayout->setContentsMargins(0, 0, 0, 0);

    runButton->setText("&Run");

    scriptButton->setStyleSheet(emptyButtonSheet);
    sheetButton->setStyleSheet(emptyButtonSheet);

    connect(runButton, &QPushButton::released, this, &MenuBarWidget::runRequested);

    initializeMenu();

    scriptIcon->setPixmap(QPixmap(":/icon/icon_code").scaled(barHeight, barHeight));
    sheetIcon->setPixmap(QPixmap(":/icon/icon_doc").scaled(barHeight, barHeight));

    scriptButton->setFixedHeight(barHeight);
    sheetButton->setFixedHeight(barHeight);
    runButton->setFixedHeight(barHeight);
    parentWidget()->adjustSize();
}

void MenuBarWidget::setScript(const QFileInfo& info)
{
    if(info.fileName().isEmpty())
    {
        scriptButton->setMenu(emptyMenu);
        scriptButton->setStyleSheet(emptyButtonSheet);
    }
    else
    {
        scriptButton->setMenu(scriptMenu);
        scriptButton->setStyleSheet(activeButtonSheet);
    }

    scriptButton->setText(info.fileName());
    scriptButton->setToolTip(info.absoluteFilePath());
    parentWidget()->adjustSize();
}

void MenuBarWidget::setSheet(const QFileInfo& info)
{
    if(info.fileName().isEmpty())
    {
        sheetButton->setMenu(emptyMenu);
        sheetButton->setStyleSheet(emptyButtonSheet);
    }
    else
    {
        sheetButton->setMenu(sheetMenu);
        sheetButton->setStyleSheet(activeButtonSheet);
    }

    sheetButton->setText(info.fileName());
    sheetButton->setToolTip(info.absoluteFilePath());
    parentWidget()->adjustSize();
}

void MenuBarWidget::initializeMenu()
{
    QAction *closeProcess = new QAction("Close this process", scriptMenu);
    QAction *saveAsTemplate = new QAction("Save as template", scriptMenu);

    scriptMenu->addAction(closeProcess);
    scriptMenu->addAction(saveAsTemplate);

    connect(closeProcess, &QAction::triggered, this, &MenuBarWidget::closeProcessRequested);
    connect(saveAsTemplate, &QAction::triggered, this, &MenuBarWidget::saveAsTemplateRequested);


    QAction *openInNewWindow = new QAction("Open in new window.", this);
    autoUpdateAction = new QAction("Enable auto updating", this);

    sheetMenu->addAction(openInNewWindow);
    sheetMenu->addAction(autoUpdateAction);

    connect(openInNewWindow, &QAction::triggered, this, &MenuBarWidget::openInNewWindowRequested);
    connect(autoUpdateAction, &QAction::triggered, this, &MenuBarWidget::autoSheetUpdateRequested);
}

void MenuBarWidget::changeAutoUpdateSheetMenuText(const bool isAuto)
{
    if(isAuto)
        autoUpdateAction->setText("Enable auto updating");
    else
        autoUpdateAction->setText("Disable auto updating");
}





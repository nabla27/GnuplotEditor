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
    QAction *clearConsoleWindow = new QAction("Clear console window", this);
    addAction(clearConsoleWindow);
    connect(clearConsoleWindow, &QAction::triggered, this, &WidgetMenu::clearConsoleWindowPushed);

    QAction *clearOutputWindow = new QAction("Clear output window", this);
    addAction(clearOutputWindow);
    connect(clearOutputWindow, &QAction::triggered, this, &WidgetMenu::clearOutputWindowPushed);

    QAction *openEditorSetting = new QAction("Editor Setting", this);
    addAction(openEditorSetting);
    connect(openEditorSetting, &QAction::triggered, this, &WidgetMenu::editorSettingOpened);

    QAction *openGnuplotSetting = new QAction("Gnuplot Setting", this);
    addAction(openGnuplotSetting);
    connect(openGnuplotSetting, &QAction::triggered, this, &WidgetMenu::gnuplotSettingOpened);

    QAction *openTemplateCustom = new QAction("Template Script", this);
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






ScriptMenu::ScriptMenu(const QString& title, QWidget *parent)
    : QMenu(title, parent)
{
    QAction *const closeProcess = new QAction("Close this process", this);
    addAction(closeProcess);
    connect(closeProcess, &QAction::triggered, this, &ScriptMenu::closeProcessRequested);
}






SheetMenu::SheetMenu(const QString& title, QWidget *parent)
    : QMenu(title, parent)
{
    QAction *openInNewWindowAction = new QAction("Open in new window", this);
    addAction(openInNewWindowAction);
    connect(openInNewWindowAction, &QAction::triggered, this, &SheetMenu::openInNewWindowRequested);

    autoUpdateAction = new QAction("Enable auto updating", this);
    addAction(autoUpdateAction);
    connect(autoUpdateAction, &QAction::triggered, this, &SheetMenu::autoTableUpdateRequested);
}

void SheetMenu::setAutoUpdateMenuText(const bool isEnable)
{
    if(isEnable)
        autoUpdateAction->setText("Disable auto updating");
    else
        autoUpdateAction->setText("Enable auto updating");
}











































#include "templatecustomwidget.h"
#include <QHBoxLayout>
#include <QScreen>
#include <QStyle>
#include <QDir>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include "utility.h"
#include "iofile.h"

TemplateCustomWidget::TemplateCustomWidget(QWidget *parent)
    : QWidget(parent)
    , settingFolderPath(QApplication::applicationDirPath() + "/setting")
    , rootFolderName("script-template")
    , rootFolderPath(settingFolderPath + '/' + rootFolderName)

    , templateItemPanel(new TemplateItemPanel(this))
    , templateScriptTreeArea(new QScrollArea(this))
    , templateScriptTree(nullptr)
    , templateScriptTreeLayout(nullptr)

    , currentTemplateFolderPath(settingFolderPath + '/' + rootFolderName)
    , editorPanel(new TemplateEditorPanel(rootFolderName, this))
    , editor(new TextEdit(this))
{
    setWindowFlag(Qt::WindowType::Window, true);
    setGeometry(getRectFromScreenRatio(screen()->size(), 0.4f, 0.4f));
    setWindowTitle("GnuplotEditor  Script-Template");

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    QVBoxLayout *templateListLayout = new QVBoxLayout;
    QVBoxLayout *displayLayout = new QVBoxLayout;

    setLayout(hLayout);
    hLayout->addLayout(templateListLayout);
    hLayout->addLayout(displayLayout);

    templateListLayout->addWidget(templateItemPanel);
    templateListLayout->addWidget(templateScriptTreeArea);
    displayLayout->addWidget(editorPanel);
    displayLayout->addWidget(editor);

    templateItemPanel->setMaximumWidth(200);
    templateScriptTreeArea->setLayout(new QVBoxLayout);
    templateScriptTreeArea->setMaximumWidth(200);
    templateScriptTreeArea->setWidgetResizable(true);
    editor->setReadOnly(true);

    setContentsMargins(0, 0, 0, 0);
    templateListLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(0);
    templateListLayout->setSpacing(0);
    templateScriptTreeArea->setContentsMargins(0, 0, 0, 0);
    templateScriptTreeArea->layout()->setContentsMargins(0, 0, 0, 0);
    templateScriptTreeArea->layout()->setSpacing(0);

    connect(templateItemPanel, &TemplateItemPanel::backDirRequested, this, &TemplateCustomWidget::backDirectory);
    connect(templateItemPanel, &TemplateItemPanel::reloadDirRequested, this, &TemplateCustomWidget::reloadTemplateList);
    connect(templateItemPanel, &TemplateItemPanel::createTemplateRequested, this, &TemplateCustomWidget::createNewTemplate);
    connect(templateItemPanel, &TemplateItemPanel::createFolderRequested, this, &TemplateCustomWidget::createNewFolder);

    /* フォルダーの設定 */
    QDir settingFolderDir(settingFolderPath);
    if(!settingFolderDir.exists()) settingFolderDir.mkdir(settingFolderPath);
    templateItemPanel->setFolderName('/' + rootFolderName);
    setupTemplateList(currentTemplateFolderPath);
}

void TemplateCustomWidget::setupTemplateList(const QString& folderPath)
{
    /* ディレクトリが存在するか */
    QDir templateFolderDir(folderPath);
    if(!templateFolderDir.exists()) templateFolderDir.mkdir(folderPath);

    /* widgetとレイアウトの初期化 */
    if(templateScriptTreeLayout) delete templateScriptTreeLayout;
    if(templateScriptTree) delete templateScriptTree;
    templateScriptTree = new QWidget(this);
    templateScriptTreeLayout = new QVBoxLayout(templateScriptTree);

    templateScriptTreeArea->setWidget(templateScriptTree);
    templateScriptTree->setLayout(templateScriptTreeLayout);
    templateScriptTreeLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
    templateScriptTreeLayout->setSpacing(0);
    templateScriptTreeLayout->setContentsMargins(0, 0, 0, 0);
    templateScriptTree->setContentsMargins(0, 0, 0, 0);

    /* ディレクトリのファイルを走査し、リストとして追加していく */
    const QList<QFileInfo> fileInfoList = templateFolderDir.entryInfoList(QStringList(),
                                                                          QDir::Filter::NoDotAndDotDot |
                                                                          QDir::Filter::Files |
                                                                          QDir::Filter::Dirs);
    for(const QFileInfo& info : fileInfoList)
    {
        TemplateItemWidget::ItemType itemType;
        if(info.isFile())
        {
            if(info.suffix() != "txt") continue;
            itemType = TemplateItemWidget::ItemType::File;
        }
        else
            itemType = TemplateItemWidget::ItemType::Dir;

        switch(itemType)
        {
        case TemplateItemWidget::ItemType::File:
            setupNewTemplate(info.filePath());
            break;
        case TemplateItemWidget::ItemType::Dir:
            setupNewFolder(info.filePath());
            break;
        default:
            break;
        }
    }
}

void TemplateCustomWidget::setupNewTemplate(const QString &filePath)
{
    if(filePath.last(1) == "/") return;

    const qsizetype barIndex = filePath.lastIndexOf('/');
    const QString fileName = (barIndex == qsizetype(-1)) ? filePath : filePath.sliced(barIndex + 1);
    const qsizetype dotIndex = fileName.lastIndexOf('.');
    const QString templateName = (dotIndex == qsizetype(-1)) ? fileName : fileName.first(dotIndex);

    TemplateItemWidget *item = new TemplateItemWidget(TemplateItemWidget::ItemType::File, templateName, filePath, templateScriptTree);
    templateScriptTreeLayout->addWidget(item);

    connect(item, &TemplateItemWidget::templateSelected, this, &TemplateCustomWidget::setTemplate);
    connect(item, &TemplateItemWidget::templateRenamed, this, &TemplateCustomWidget::renameTemplate);
    connect(item, &TemplateItemWidget::templateRemoved, this, &TemplateCustomWidget::removeTemplate);
}

void TemplateCustomWidget::setupNewFolder(const QString &folderPath)
{
    if(folderPath.last(1) == "/") return;

    const qsizetype barIndex = folderPath.lastIndexOf('/');
    const QString folderName = (barIndex == qsizetype(-1)) ? folderPath : folderPath.sliced(barIndex + 1);

    TemplateItemWidget *item = new TemplateItemWidget(TemplateItemWidget::ItemType::Dir, folderName, folderPath, templateScriptTree);
    templateScriptTreeLayout->addWidget(item);
}

void TemplateCustomWidget::setTemplate(const QString& filePath)
{
    QFile file(filePath);

    if(!file.exists())
    {
        editor->setPlainText(filePath + " was not found.");
        return;
    }

    bool ok = false;
    const QString script = readFileTxt(filePath, &ok);

    if(!ok)
    {
        editor->setPlainText("Could not read a file " + filePath + "\n\n" + script);
        return;
    }

    editorPanel->setTemplateName(filePath);
    editor->setPlainText(script);
    currentTemplateFilePath = filePath;
}

void TemplateCustomWidget::renameTemplate(const QString& oldFilePath, const QString& newFilePath)
{
    if(currentTemplateFilePath == oldFilePath)
        editorPanel->setTemplateName(newFilePath);
}

void TemplateCustomWidget::removeTemplate(TemplateItemWidget *item, const QString& filePath)
{
    if(currentTemplateFilePath == filePath)
    {
        editorPanel->setTemplateName("");
        editor->setPlainText("");
    }

    templateScriptTreeLayout->removeWidget(item);
    delete item;
}

void TemplateCustomWidget::backDirectory()
{
    if(currentTemplateFolderPath == rootFolderPath) return;

    currentTemplateFolderPath = currentTemplateFolderPath.first(currentTemplateFolderPath.lastIndexOf('/'));
    templateItemPanel->setFolderName(currentTemplateFolderPath.sliced(currentTemplateFolderPath.lastIndexOf('/')));
    setupTemplateList(currentTemplateFolderPath);
}

void TemplateCustomWidget::reloadTemplateList()
{
    setupTemplateList(currentTemplateFolderPath);
}

void TemplateCustomWidget::createNewTemplate()
{
    QString newTemplateName;

    for(;;)
    {
        newTemplateName = QInputDialog::getText(this, "Create New Template", "Enter the name.");

        if(newTemplateName.contains('.'))
            QMessageBox::critical(this, "File Name Error", "Do not include a suffix.");
        else
            break;
    }

    if(newTemplateName.isEmpty()) return;

    QFile newFile(currentTemplateFolderPath + '/' + newTemplateName + ".txt");
    if(newFile.open(QFile::OpenModeFlag::NewOnly))
    {
        setupNewTemplate(newFile.fileName());
        setTemplate(newFile.fileName());

        newFile.close();
    }
    else
        QMessageBox::critical(this, "File System Error", "Could not create the file.");
}

void TemplateCustomWidget::createNewFolder()
{
    const QString newFolderName = QInputDialog::getText(this, "Create New Folder", "Enter the name");

    if(newFolderName.isEmpty()) return;

    const QString newFolderPath = currentTemplateFolderPath + '/' + newFolderName;

    QDir currentDir(newFolderPath);

    if(currentDir.mkdir(newFolderPath))
        setupNewFolder(newFolderPath);
    else
        QMessageBox::critical(this, "File System Error", "Could not create the folder");
}








TemplateItemPanel::TemplateItemPanel(QWidget *parent)
    : QWidget(parent)
    , backDirIcon(new mlayout::IconLabel(this))
    , folderNameEdit(new QLineEdit("script-template", this))
    , reloadDirIcon(new mlayout::IconLabel(this))
    , newFileIcon(new mlayout::IconLabel(this))
    , newFolderIcon(new mlayout::IconLabel(this))
{
    QHBoxLayout *hLayout = new QHBoxLayout(this);

    setLayout(hLayout);
    hLayout->addWidget(backDirIcon);
    hLayout->addWidget(folderNameEdit);
    hLayout->addWidget(reloadDirIcon);
    hLayout->addWidget(newFileIcon);
    hLayout->addWidget(newFolderIcon);

    folderNameEdit->setReadOnly(true);
    constexpr int iconSize = 20;
    backDirIcon->setPixmap(QApplication::style()->standardIcon(QStyle::SP_FileDialogBack).pixmap(iconSize, iconSize));
    reloadDirIcon->setPixmap(QApplication::style()->standardIcon(QStyle::SP_BrowserReload).pixmap(iconSize, iconSize));
    newFileIcon->setPixmap(QApplication::style()->standardIcon(QStyle::SP_FileIcon).pixmap(iconSize, iconSize));
    newFolderIcon->setPixmap(QApplication::style()->standardIcon(QStyle::SP_FileDialogNewFolder).pixmap(iconSize, iconSize));
    backDirIcon->setHoveredFrameShape(QFrame::Box);
    reloadDirIcon->setHoveredFrameShape(QFrame::Box);
    newFileIcon->setHoveredFrameShape(QFrame::Box);
    newFolderIcon->setHoveredFrameShape(QFrame::Box);

    hLayout->setSpacing(0);
    hLayout->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);

    connect(backDirIcon, &mlayout::IconLabel::released, this, &TemplateItemPanel::backDirRequested);
    connect(reloadDirIcon, &mlayout::IconLabel::released, this, &TemplateItemPanel::reloadDirRequested);
    connect(newFileIcon, &mlayout::IconLabel::released, this, &TemplateItemPanel::createTemplateRequested);
    connect(newFolderIcon, &mlayout::IconLabel::released, this, &TemplateItemPanel::createFolderRequested);
}

void TemplateItemPanel::setFolderName(const QString &folderName)
{
    folderNameEdit->setText(folderName);
}










TemplateItemWidget::TemplateItemWidget(const ItemType& itemType, const QString& name, const QString& filePath, QWidget *parent)
    : QWidget(parent)
    , itemType(itemType)
    , filePath(filePath)
    , scriptNameButton(new QPushButton(this))
    , buttonLabel(new QLabel(name, scriptNameButton))
    , toolIcon(new mlayout::IconLabel(this))
    , toolMenu(new QMenu(toolIcon))
{
    //アイコンのサイズ。ボタンの高さ(=このwidget)もこれに合わせる
    constexpr int iconSize = 22;

    setFixedHeight(iconSize);

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(scriptNameButton);

    setLayout(hLayout);
    hLayout->addWidget(scriptNameButton);
    hLayout->addWidget(toolIcon);

    scriptNameButton->setLayout(buttonLayout);
    buttonLayout->addWidget(buttonLabel);

    toolIcon->setPixmap(QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView).pixmap(iconSize, iconSize));
    toolIcon->setHoveredFrameShape(QFrame::Shape::Box);

    setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(0);
    hLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLabel->setIndent(10); //ボタン左先頭部分の余白

    scriptNameButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(scriptNameButton, &QPushButton::released, this, &TemplateItemWidget::emitSelectedSignals);
    connect(toolIcon, &mlayout::IconLabel::released, this, &TemplateItemWidget::showToolMenu);

    setupToolMenu();
}

void TemplateItemWidget::setupToolMenu()
{
    QAction *renameAction = new QAction("Rename", toolMenu);
    QAction *removeAction = new QAction("Remove", toolMenu);

    toolMenu->addAction(renameAction);
    toolMenu->addAction(removeAction);

    renameAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton));
    removeAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCancelButton));

    connect(renameAction, &QAction::triggered, this, &TemplateItemWidget::rename);
    connect(removeAction, &QAction::triggered, this, &TemplateItemWidget::remove);
}

void TemplateItemWidget::emitSelectedSignals()
{
    switch (itemType)
    {
    case ItemType::File: emit templateSelected(filePath); break;
    case ItemType::Dir: emit folderSelected(filePath); break;
    default: break;
    }
}

void TemplateItemWidget::showToolMenu()
{
    toolMenu->exec(cursor().pos());
}

void TemplateItemWidget::rename()
{
    switch (itemType)
    {
    case ItemType::File: renameTempalteFile(); break;
    case ItemType::Dir: renameFolder(); break;
    default: break;
    }
}

void TemplateItemWidget::renameTempalteFile()
{
    const QString oldFileName = buttonLabel->text();
    const QString newFileName = QInputDialog::getText(this, "Rename", "new file name", QLineEdit::Normal, oldFileName);

    if(newFileName.isEmpty() || newFileName == oldFileName)
        return;

    QFile file(filePath);
    if(!file.exists())
    {
        QMessageBox::warning(this, "Failed to rename.", "Could not find the file " + filePath);
        return;
    }

    const qsizetype barIndex = filePath.lastIndexOf('/');
    const QString newFilePath = (qsizetype(-1) == barIndex)
            ? filePath + '/' + newFileName + ".txt"
            : filePath.first(barIndex) + '/' + newFileName + ".txt";

    if(!file.rename(filePath, newFilePath))
    {
        QMessageBox::critical(this, "Failed to rename.", "Could not rename the file.");
        return;
    }

    emit templateRenamed(filePath, newFilePath);

    buttonLabel->setText(newFileName);
    filePath = newFilePath;
}

void TemplateItemWidget::renameFolder()
{

}

void TemplateItemWidget::remove()
{
    switch(itemType)
    {
    case ItemType::File: removeTemplateFile(); break;
    case ItemType::Dir: removeFolder(); break;
    default: break;
    }
}

void TemplateItemWidget::removeTemplateFile()
{
    QFile file(filePath);
    if(file.remove())
    {
        emit templateRemoved(this, filePath);
    }
    else
        QMessageBox::critical(this, "Failed to remove.", "Could not remove the file.");
}

void TemplateItemWidget::removeFolder()
{

}








TemplateEditorPanel::TemplateEditorPanel(const QString& rootFolderName, QWidget *parent)
    : QWidget(parent)
    , rootFolderName(rootFolderName)
    , templateNameEdit(new QLineEdit(this))
{
    QHBoxLayout *hLayout = new QHBoxLayout(this);

    setLayout(hLayout);
    hLayout->addWidget(templateNameEdit);

    templateNameEdit->setReadOnly(true);

    hLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);
    hLayout->setSpacing(0);
    hLayout->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);
}

void TemplateEditorPanel::setTemplateName(const QString &filePath)
{
    const qsizetype rootNameIndex = filePath.lastIndexOf(rootFolderName);

    if(rootNameIndex == qsizetype(-1))
        templateNameEdit->setText(filePath);
    else
        templateNameEdit->setText(filePath.sliced(filePath.lastIndexOf(rootFolderName)));
}









/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "templatecustomwidget.h"
#include <QHBoxLayout>
#include <QScreen>
#include <QStyle>
#include <QDir>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QSplitter>
#include "utility.h"
#include "iofile.h"
#include "logger.h"



TemplateCustomWidget::TemplateCustomWidget(QWidget *parent)
    : QWidget(parent)

    , templateItemPanel(new TemplateItemPanel(this))
    , templateScriptTreeArea(new QScrollArea(this))
    , templateScriptTree(nullptr)
    , templateScriptTreeLayout(nullptr)

    , currentTemplateFolderPath(rootFolderPath())
    , editorPanel(new TemplateEditorPanel(rootFolderName(), this))
    , editor(new TextEdit(this))
{
    setWindowFlag(Qt::WindowType::Window, true);
    setGeometry(getRectFromScreenRatio(screen()->size(), 0.4f, 0.4f));
    setWindowTitle("GnuplotEditor  Script-Template");

    setupLayout();

    connect(templateItemPanel, &TemplateItemPanel::backDirRequested, this, &TemplateCustomWidget::backDirectory);
    connect(templateItemPanel, &TemplateItemPanel::showFolderListRequested, this, &TemplateCustomWidget::showFolderList);
    connect(templateItemPanel, &TemplateItemPanel::reloadDirRequested, this, &TemplateCustomWidget::reloadTemplateList);
    connect(templateItemPanel, &TemplateItemPanel::createTemplateRequested, this, &TemplateCustomWidget::createNewTemplate);
    connect(templateItemPanel, &TemplateItemPanel::createFolderRequested, this, &TemplateCustomWidget::createNewFolder);
    connect(editorPanel, &TemplateEditorPanel::importButtonReleased, this, &TemplateCustomWidget::requestImportingTemplate);
    connect(editorPanel, &TemplateEditorPanel::readOnlyChanged, editor, &TextEdit::setReadOnly);
    connect(editorPanel, &TemplateEditorPanel::readOnlyChanged, this, &TemplateCustomWidget::saveCurrentTemplateFile);

    /* フォルダーの設定 */
    QDir settingFolderDir(settingFolderPath());
    if(!settingFolderDir.exists()) settingFolderDir.mkdir(settingFolderPath());
    setFolder(rootFolderPath());

    //templateItemPanel->setFolderPath(rootFolderPath());
    //setupTemplateList(currentTemplateFolderPath);
}

TemplateCustomWidget::~TemplateCustomWidget()
{
    saveCurrentTemplateFile();
}

void TemplateCustomWidget::setupLayout()
{
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    QSplitter *splitter = new QSplitter(Qt::Orientation::Horizontal, this);
    QWidget *templateListWidget = new QWidget(this);
    QWidget *displayWidget = new QWidget(this);
    QVBoxLayout *templateListLayout = new QVBoxLayout(templateListWidget);
    QVBoxLayout *displayLayout = new QVBoxLayout(displayWidget);

    setLayout(hLayout);
    hLayout->addWidget(splitter);
    splitter->addWidget(templateListWidget);
    splitter->addWidget(displayWidget);

    templateListWidget->setLayout(templateListLayout);
    displayWidget->setLayout(displayLayout);
    templateListLayout->addWidget(templateItemPanel);
    templateListLayout->addWidget(templateScriptTreeArea);
    displayLayout->addWidget(editorPanel);
    displayLayout->addWidget(editor);

    constexpr int defaultTreeWidth = 200;
    splitter->setSizes(QList<int>() << defaultTreeWidth << geometry().width() - defaultTreeWidth);
    templateItemPanel->setMaximumWidth(defaultTreeWidth);
    templateScriptTreeArea->setLayout(new QVBoxLayout);
    templateScriptTreeArea->setWidgetResizable(true);
    editor->setReadOnly(true);

    setContentsMargins(0, 0, 0, 0);
    splitter->setContentsMargins(0, 0, 0, 0);
    templateListWidget->setContentsMargins(0, 0, 0, 0);
    displayWidget->setContentsMargins(0, 0, 0, 0);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(0);
    templateListLayout->setContentsMargins(0, 0, 0, 0);
    templateListLayout->setSpacing(0);
    displayLayout->setSpacing(0);
    displayLayout->setContentsMargins(0, 0, 0, 0);
    templateScriptTreeArea->setContentsMargins(0, 0, 0, 0);
    templateScriptTreeArea->layout()->setContentsMargins(0, 0, 0, 0);
    templateScriptTreeArea->layout()->setSpacing(0);
}

void TemplateCustomWidget::addTemplate(const QString& script)
{
    QDialog dialog(this);
    QFormLayout formLayout(&dialog);
    QComboBox folderCombo(&dialog);
    QLineEdit templateName(&dialog);
    QDialogButtonBox button(QDialogButtonBox::Ok, &dialog);

    dialog.setLayout(&formLayout);
    formLayout.addRow(new QLabel("Folder", &dialog), &folderCombo);
    formLayout.addRow(new QLabel("Name", &dialog), &templateName);
    formLayout.addRow(&button);

    /* comboにフォルダー一覧を設定 */
    QStringList folderList;
    folderList << rootFolderName();
    getDirRecursively(rootFolderPath(), folderList);
    folderCombo.addItems(folderList);

    connect(&button, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

    dialog.exec();

    /* エラー条件 */
    if(templateName.text().isEmpty()) return;
    if(templateName.text().contains('.'))
    {
        __LOGOUT__("do not include the '.'", Logger::LogLevel::Error);
        return;
    }
    if(QFile::exists(currentTemplateFolderPath + '/' + templateName.text() + ".txt"))
    {
        __LOGOUT__("the same file \"" + currentTemplateFolderPath + "/" + templateName.text() + "\" already exists.", Logger::LogLevel::Error);
        return;
    }

    const QString templateFilePath = settingFolderPath() + '/' + folderCombo.currentText() + '/' + templateName.text() + ".txt";

    bool ok = false;
    WriteTxtFile::toTxt(templateFilePath, script, ok);

    if(ok)
    {
        if(currentTemplateFolderPath == settingFolderPath() + '/' + folderCombo.currentText())
        {
            setupNewTemplate(templateFilePath);
            setTemplate(templateFilePath);
        }
    }
    else
    {
        __LOGOUT__("Faied to create the template \"" + templateFilePath + "\".", Logger::LogLevel::Error);
    }
}

void TemplateCustomWidget::getDirRecursively(const QString& folderPath, QStringList& folderList)
{
    QDir dir(folderPath);
    const QList<QFileInfo> infoList = dir.entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::Dirs);

    for(const QFileInfo& info : infoList)
    {
        const QString absolutePath = info.absoluteFilePath();
        folderList << absolutePath.sliced(absolutePath.lastIndexOf(rootFolderName()));
        getDirRecursively(absolutePath, folderList);
    }
}

void TemplateCustomWidget::setupTemplateList(const QString& folderPath)
{
    /* ディレクトリが存在するか */
    QDir templateFolderDir(folderPath);
    if(!templateFolderDir.exists())
    {
        if(templateFolderDir.mkdir(folderPath))
        {
            __LOGOUT__("template folder \"" + folderPath + "\" was made.", Logger::LogLevel::Info);
        }
        else
        {
            __LOGOUT__("failed to make dir " + folderPath, Logger::LogLevel::Warn);
        }
    }

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
            if(info.suffix() != "txt")
            {
                __LOGOUT__("skip the file \"" + info.absoluteFilePath() + "\".", Logger::LogLevel::Warn);
                continue;
            }
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

    connect(item, &TemplateItemWidget::folderSelected, this, &TemplateCustomWidget::setFolder);
    connect(item, &TemplateItemWidget::folderRenamed, editorPanel, &TemplateEditorPanel::renameFolder);
    connect(item, &TemplateItemWidget::folderRemoved, this, &TemplateCustomWidget::removeFolder);
}

void TemplateCustomWidget::saveCurrentTemplateFile()
{
    if(currentTemplateFilePath.isEmpty()) return;

    QFile file(currentTemplateFilePath);
    if(file.open(QFile::OpenModeFlag::WriteOnly))
    {
        QTextStream text(&file);
        text << editor->toPlainText();

        file.close();
    }
    else
    {
        __LOGOUT__("cound not open and save the file \"" + currentTemplateFilePath + "\".", Logger::LogLevel::Error);
    }
}

void TemplateCustomWidget::requestImportingTemplate()
{
    emit importTemplateRequested(editor->toPlainText());
}

void TemplateCustomWidget::setTemplate(const QString& filePath)
{
    /* 現在のテンプレートを保存 */
    saveCurrentTemplateFile();

    QFile file(filePath);

    if(!file.exists())
    {
        editor->setPlainText(filePath + " was not found.");
        __LOGOUT__(filePath + " was not found.", Logger::LogLevel::Error);
        return;
    }

    bool ok = false;
    const QString script = readFileTxt(filePath, &ok);

    if(!ok)
    {
        editor->setPlainText("Could not read a file " + filePath + "\n\n" + script);
        __LOGOUT__("could not read a file " + filePath, Logger::LogLevel::Error);
        return;
    }

    editorPanel->setTemplateName(filePath);
    editor->setPlainText(script);
    currentTemplateFilePath = filePath;
}

void TemplateCustomWidget::renameTemplate(const QString& oldFilePath, const QString& newFilePath)
{
    if(currentTemplateFilePath == oldFilePath)
    {
        editorPanel->setTemplateName(newFilePath);
        currentTemplateFilePath = newFilePath;
    }
}

void TemplateCustomWidget::removeTemplate(TemplateItemWidget *item, const QString& filePath)
{
    if(currentTemplateFilePath == filePath)
    {
        editorPanel->setTemplateName("");
        editor->setPlainText("");
        currentTemplateFilePath = "";
    }

    templateScriptTreeLayout->removeWidget(item);
    delete item;
}

void TemplateCustomWidget::setFolder(const QString& folderPath)
{
    currentTemplateFolderPath = folderPath;
    templateItemPanel->setFolderPath(folderPath);
    setupTemplateList(folderPath);
}

void TemplateCustomWidget::removeFolder(TemplateItemWidget *item, const QString& folderPath)
{
    if(currentTemplateFilePath.contains(folderPath))
    {
        editorPanel->setTemplateName("");
        editor->setPlainText("");
        currentTemplateFilePath = "";
    }

    templateScriptTreeLayout->removeWidget(item);
    delete item;
}

void TemplateCustomWidget::backDirectory()
{
    if(currentTemplateFolderPath == rootFolderPath()) return;

    currentTemplateFolderPath = currentTemplateFolderPath.first(currentTemplateFolderPath.lastIndexOf('/'));
    templateItemPanel->setFolderPath(currentTemplateFolderPath);
    setupTemplateList(currentTemplateFolderPath);
}

void TemplateCustomWidget::showFolderList()
{
    QMenu menu(this);
    QStringList folderList;
    folderList << rootFolderName();
    getDirRecursively(rootFolderPath(), folderList);

    /* menuからactionが選択されて、actionに設定されたobjectName(=選択されたパス)からフォルダーを設定する */
    auto setFolderFromAction = [this](QAction *action)
    {
        setFolder(settingFolderPath() + '/' + action->objectName());
    };

    for(const QString& folderName : folderList)
    {
        QAction *act = new QAction(folderName, &menu);
        menu.addAction(act);
        act->setObjectName(folderName);
    }

    connect(&menu, &QMenu::triggered, setFolderFromAction);

    menu.exec(cursor().pos());
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
            __LOGOUT__("do not include a suffix.", Logger::LogLevel::Error);
        else
            break;
    }

    if(newTemplateName.isEmpty()) return;

    QFile newFile(currentTemplateFolderPath + '/' + newTemplateName + ".txt");
    if(newFile.open(QFile::OpenModeFlag::NewOnly))
    {
        newFile.close();
        setupNewTemplate(newFile.fileName());
        setTemplate(newFile.fileName());
    }
    else
    {
        __LOGOUT__("Failed to open the file \"" + newFile.fileName() + "\".", Logger::LogLevel::Error);
    }
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
    {
        __LOGOUT__("Failed to make the dir \"" + newFolderPath + "\".", Logger::LogLevel::Error);
    }
}








TemplateItemPanel::TemplateItemPanel(QWidget *parent)
    : QWidget(parent)
    , backDirIcon(new mlayout::IconLabel(this))
    , folderNameEdit(new QLineEdit(this))
    , folderListIcon(new mlayout::IconLabel(this))
    , reloadDirIcon(new mlayout::IconLabel(this))
    , newFileIcon(new mlayout::IconLabel(this))
    , newFolderIcon(new mlayout::IconLabel(this))
{
    QHBoxLayout *hLayout = new QHBoxLayout(this);

    setLayout(hLayout);
    hLayout->addWidget(backDirIcon);
    hLayout->addWidget(folderNameEdit);
    hLayout->addWidget(folderListIcon);
    hLayout->addWidget(reloadDirIcon);
    hLayout->addWidget(newFileIcon);
    hLayout->addWidget(newFolderIcon);

    folderNameEdit->setReadOnly(true);

    constexpr int iconSize = 20;
    backDirIcon->setPixmap(QApplication::style()->standardIcon(QStyle::SP_FileDialogBack).pixmap(iconSize, iconSize));
    folderListIcon->setPixmap(QApplication::style()->standardIcon(QStyle::SP_TitleBarUnshadeButton).pixmap(iconSize / 2, iconSize));
    reloadDirIcon->setPixmap(QApplication::style()->standardIcon(QStyle::SP_BrowserReload).pixmap(iconSize, iconSize));
    newFileIcon->setPixmap(QApplication::style()->standardIcon(QStyle::SP_FileIcon).pixmap(iconSize, iconSize));
    newFolderIcon->setPixmap(QApplication::style()->standardIcon(QStyle::SP_FileDialogNewFolder).pixmap(iconSize, iconSize));

    backDirIcon->setHoveredFrameShape(QFrame::Box);
    folderListIcon->setHoveredFrameShape(QFrame::Box);
    reloadDirIcon->setHoveredFrameShape(QFrame::Box);
    newFileIcon->setHoveredFrameShape(QFrame::Box);
    newFolderIcon->setHoveredFrameShape(QFrame::Box);

    backDirIcon->setToolTip("move to parent directory");
    folderListIcon->setToolTip("folder list");
    reloadDirIcon->setToolTip("reload");
    newFileIcon->setToolTip("create new file");
    newFolderIcon->setToolTip("create new folder");

    hLayout->setSpacing(0);
    hLayout->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);

    connect(backDirIcon, &mlayout::IconLabel::released, this, &TemplateItemPanel::backDirRequested);
    connect(folderListIcon, &mlayout::IconLabel::released, this, &TemplateItemPanel::showFolderListRequested);
    connect(reloadDirIcon, &mlayout::IconLabel::released, this, &TemplateItemPanel::reloadDirRequested);
    connect(newFileIcon, &mlayout::IconLabel::released, this, &TemplateItemPanel::createTemplateRequested);
    connect(newFolderIcon, &mlayout::IconLabel::released, this, &TemplateItemPanel::createFolderRequested);
}

void TemplateItemPanel::setFolderPath(const QString &folderPath)
{
    currentFolderPath = folderPath;
    folderNameEdit->setToolTip(currentFolderPath);
    folderNameEdit->setText(folderPath.sliced(folderPath.lastIndexOf('/')));
}










TemplateItemWidget::TemplateItemWidget(const ItemType& itemType, const QString& name, const QString& filePath, QWidget *parent)
    : QWidget(parent)
    , itemType(itemType)
    , filePath(filePath)
    , typeIcon(new mlayout::IconLabel(this))
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

    hLayout->addWidget(typeIcon);
    hLayout->addWidget(scriptNameButton);
    hLayout->addWidget(toolIcon);

    scriptNameButton->setLayout(buttonLayout);
    buttonLayout->addWidget(buttonLabel);

    switch(itemType)
    {
    case ItemType::File:
        typeIcon->setPixmap(QApplication::style()->standardIcon(QStyle::SP_FileIcon).pixmap(iconSize, iconSize));
        break;
    case ItemType::Dir:
    {
        typeIcon->setPixmap(QApplication::style()->standardIcon(QStyle::SP_DirIcon).pixmap(iconSize, iconSize));
        QFont font = buttonLabel->font();
        font.setBold(true);
        buttonLabel->setFont(font);
        break;
    }
    default: break;
    }

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

    if(newFileName.isEmpty() || newFileName == oldFileName) return;

    QFile file(filePath);
    if(!file.exists())
    {
        __LOGOUT__("could not find the file \"" + filePath + "\".", Logger::LogLevel::Error);
        return;
    }

    const qsizetype barIndex = filePath.lastIndexOf('/');
    const QString newFilePath = (qsizetype(-1) == barIndex)
            ? filePath + '/' + newFileName + ".txt"
            : filePath.first(barIndex) + '/' + newFileName + ".txt";

    if(!file.rename(filePath, newFilePath))
    {
        __LOGOUT__("failed to rename the file \"" + filePath + "\" to \"" + newFilePath + "\".", Logger::LogLevel::Error);
        return;
    }

    emit templateRenamed(filePath, newFilePath);

    buttonLabel->setText(newFileName);
    filePath = newFilePath;
}

void TemplateItemWidget::renameFolder()
{
    const QString oldFolderName = buttonLabel->text();
    const QString newFolderName = QInputDialog::getText(this, "Rename", "new folder name", QLineEdit::Normal, oldFolderName);

    if(newFolderName.isEmpty() || newFolderName == oldFolderName) return;

    const qsizetype barIndex = filePath.lastIndexOf('/');
    if(barIndex == qsizetype(-1)) return; //無効なパス
    const QString newFolderPath = filePath.first(barIndex) + '/' + newFolderName;

    QDir dir(filePath.first(barIndex));
    if(!dir.rename(oldFolderName, newFolderName))
    {
        __LOGOUT__("Failed to rename the folder \"" + oldFolderName + "\" to \"" + newFolderName + "\".", Logger::LogLevel::Error);
        return;
    }

    emit folderRenamed(filePath, newFolderPath);

    buttonLabel->setText(newFolderName);
    filePath = newFolderPath;
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
    {
        __LOGOUT__("Fialed to remove the file \"" + filePath + "\".", Logger::LogLevel::Error);
    }
}

void TemplateItemWidget::removeFolder()
{

    const QMessageBox::StandardButton result = QMessageBox::question(this, "", "Do you remove this \"" + buttonLabel->text() + "\" folder ??");

    if(result != QMessageBox::StandardButton::Yes) return;

    QDir dir(filePath);

    if(!dir.removeRecursively())
    {
        __LOGOUT__("Failed to remove dir recursively \"" + filePath + "\".", Logger::LogLevel::Error);
        return;
    }

    emit folderRemoved(this, filePath);
}








TemplateEditorPanel::TemplateEditorPanel(const QString& rootFolderName, QWidget *parent)
    : QWidget(parent)
    , rootFolderName(rootFolderName)
    , templateNameEdit(new QLineEdit(this))
    , readOnlyButton(new QRadioButton("Read", this))
    , importButton(new QPushButton("import", this))
{
    QHBoxLayout *hLayout = new QHBoxLayout(this);

    setLayout(hLayout);
    hLayout->addWidget(templateNameEdit);
    hLayout->addWidget(readOnlyButton);
    hLayout->addWidget(importButton);

    templateNameEdit->setReadOnly(true);
    readOnlyButton->setChecked(true);

    hLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);
    hLayout->setSpacing(0);
    hLayout->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);

    connect(readOnlyButton, &QRadioButton::toggled, this, &TemplateEditorPanel::changeReadOnly);
    connect(importButton, &QPushButton::released, this, &TemplateEditorPanel::importButtonReleased);
}

void TemplateEditorPanel::setTemplateName(const QString &filePath)
{
    const qsizetype rootNameIndex = filePath.lastIndexOf(rootFolderName);

    if(rootNameIndex == qsizetype(-1)) //templateが消された場合などに、filePathにから文字列が渡される
        templateNameEdit->setText(filePath);
    else
        templateNameEdit->setText(filePath.sliced(rootNameIndex));
}

void TemplateEditorPanel::renameFolder(const QString &oldFolderPath, const QString &newFolderPath)
{
    const QString old = oldFolderPath.sliced(oldFolderPath.lastIndexOf(rootFolderName));
    const QString next = newFolderPath.sliced(newFolderPath.lastIndexOf(rootFolderName));

    templateNameEdit->setText(templateNameEdit->text().replace(old, next));
}

void TemplateEditorPanel::changeReadOnly(const bool isReadOnly)
{
    if(isReadOnly)
    {
        readOnlyButton->setText("Read");
        emit readOnlyChanged(true);
    }
    else
    {
        readOnlyButton->setText("Write");
        emit readOnlyChanged(false);
    }
}









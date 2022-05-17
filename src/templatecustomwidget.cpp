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
    , templateFolderPath(settingFolderPath + '/' + rootFolderName)

    , templateItemPanel(new TemplateItemPanel(this))
    , templateScriptTreeArea(new QScrollArea(this))
    , templateScriptTree(new QWidget(this))
    , templateScriptTreeLayout(new QVBoxLayout(templateScriptTree))
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
    templateScriptTreeArea->setWidget(templateScriptTree);
    templateScriptTree->setLayout(templateScriptTreeLayout);
    displayLayout->addWidget(editorPanel);
    displayLayout->addWidget(editor);

    templateItemPanel->setMaximumWidth(200);
    templateItemPanel->setFolderName(rootFolderName);
    templateScriptTreeArea->setLayout(new QVBoxLayout);
    templateScriptTreeArea->setMaximumWidth(200);
    templateScriptTreeArea->setWidgetResizable(true);
    templateScriptTreeLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
    editor->setReadOnly(true);

    setContentsMargins(0, 0, 0, 0);
    templateListLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(0);
    templateListLayout->setSpacing(0);
    templateScriptTreeLayout->setSpacing(0);
    templateScriptTreeLayout->setContentsMargins(0, 0, 0, 0);
    templateScriptTree->setContentsMargins(0, 0, 0, 0);
    templateScriptTreeArea->setContentsMargins(0, 0, 0, 0);
    templateScriptTreeArea->layout()->setContentsMargins(0, 0, 0, 0);
    templateScriptTreeArea->layout()->setSpacing(0);

    setupTemplateList();
}

void TemplateCustomWidget::setupTemplateList()
{
    QDir settingFolderDir(settingFolderPath);
    if(!settingFolderDir.exists()) settingFolderDir.mkdir(settingFolderPath);
    QDir templateFolderDir(templateFolderPath);
    if(!templateFolderDir.exists()) templateFolderDir.mkdir(templateFolderPath);

    const QList<QFileInfo> fileInfoList = templateFolderDir.entryInfoList(QStringList() << "*.txt");
    for(const QFileInfo& info : fileInfoList)
    {
        const QString fileName = info.fileName();

        const qsizetype dotIndex = fileName.lastIndexOf('.');
        const QString templateName = (qsizetype(-1) == dotIndex) ? fileName : fileName.first(dotIndex);

        TemplateItemWidget *item = new TemplateItemWidget(templateName, info.filePath(), templateScriptTree);
        templateScriptTreeLayout->addWidget(item);

        connect(item, &TemplateItemWidget::templateSelected, this, &TemplateCustomWidget::setTemplate);
        connect(item, &TemplateItemWidget::templateRenamed, this, &TemplateCustomWidget::renameTemplate);
        connect(item, &TemplateItemWidget::removed, this, &TemplateCustomWidget::removeTemplate);
    }
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
}

void TemplateItemPanel::setFolderName(const QString &folderName)
{
    folderNameEdit->setText(folderName);
}










TemplateItemWidget::TemplateItemWidget(const QString& name, const QString& filePath, QWidget *parent)
    : QWidget(parent)
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
    emit templateSelected(filePath);
}

void TemplateItemWidget::showToolMenu()
{
    toolMenu->exec(cursor().pos());
}

void TemplateItemWidget::rename()
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

void TemplateItemWidget::remove()
{
    QFile file(filePath);
    if(file.remove())
    {
        emit removed(this, filePath);
    }
    else
        QMessageBox::critical(this, "Failed to remove.", "Could not remove the file.");
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









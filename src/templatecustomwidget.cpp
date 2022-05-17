#include "templatecustomwidget.h"
#include <QHBoxLayout>
#include <QScreen>
#include <QStyle>
#include <QDir>
#include "utility.h"
#include "iofile.h"

TemplateCustomWidget::TemplateCustomWidget(QWidget *parent)
    : QWidget(parent)
    , templateScriptTreeArea(new QScrollArea(this))
    , templateScriptTree(new QWidget(this))
    , templateScriptTreeLayout(new QVBoxLayout(templateScriptTree))
    , editorPanel(new TemplateEditorPanel(this))
    , editor(new TextEdit(this))
{
    setWindowFlag(Qt::WindowType::Window, true);
    setGeometry(getRectFromScreenRatio(screen()->size(), 0.4f, 0.4f));

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    QVBoxLayout *templateListLayout = new QVBoxLayout;
    QVBoxLayout *displayLayout = new QVBoxLayout;

    setLayout(hLayout);
    hLayout->addLayout(templateListLayout);
    hLayout->addLayout(displayLayout);

    templateListLayout->addWidget(templateScriptTreeArea);
    templateScriptTreeArea->setWidget(templateScriptTree);
    templateScriptTree->setLayout(templateScriptTreeLayout);
    displayLayout->addWidget(editorPanel);
    displayLayout->addWidget(editor);

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

    const QStringList fileList = templateFolderDir.entryList(QStringList() << "*.txt");
    for(const QString& fileName : fileList)
    {
        const QString name = fileName.first(fileName.lastIndexOf('.'));
        TemplateItemWidget *item = new TemplateItemWidget(name, templateScriptTree);
        templateScriptTreeLayout->addWidget(item);

        connect(item, &TemplateItemWidget::templateSelected, this, &TemplateCustomWidget::setTemplate);
    }
}

void TemplateCustomWidget::setTemplate(const QString& templateName)
{
    const QString templateFilePath = templateFolderPath + "/" + templateName + ".txt";

    QFile file(templateFilePath);

    if(!file.exists())
    {
        editor->setPlainText(templateFilePath + " was not found.");
        return;
    }

    bool ok = false;
    const QString script = readFileTxt(templateFilePath, &ok);

    if(!ok)
    {
        editor->setPlainText("Could not read a file " + templateFilePath + "\n\n" + script);
        return;
    }

    editorPanel->setTemplateName(templateName);
    editor->setPlainText(script);
}














TemplateItemWidget::TemplateItemWidget(const QString& name, QWidget *parent)
    : QWidget(parent)
    , scriptNameButton(new QPushButton(this))
    , buttonLabel(new QLabel(name, scriptNameButton))
    , menuTool(new mlayout::IconLabel(this))
{
    //アイコンのサイズ。ボタンの高さ(=このwidget)もこれに合わせる
    constexpr int iconSize = 22;

    setFixedHeight(iconSize);

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(scriptNameButton);

    setLayout(hLayout);
    hLayout->addWidget(scriptNameButton);
    hLayout->addWidget(menuTool);

    scriptNameButton->setLayout(buttonLayout);
    buttonLayout->addWidget(buttonLabel);

    menuTool->setPixmap(QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView).pixmap(iconSize, iconSize));
    menuTool->setHoveredFrameShape(QFrame::Shape::Box);

    setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(0);
    hLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLabel->setIndent(10); //ボタン左先頭部分の余白

    scriptNameButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(scriptNameButton, &QPushButton::released, this, &TemplateItemWidget::emitSetEditorSignals);
}

void TemplateItemWidget::emitSetEditorSignals()
{
    emit templateSelected(buttonLabel->text());
}








TemplateEditorPanel::TemplateEditorPanel(QWidget *parent)
    : QWidget(parent)
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

void TemplateEditorPanel::setTemplateName(const QString &templateName)
{
    templateNameEdit->setText(templateName);
}









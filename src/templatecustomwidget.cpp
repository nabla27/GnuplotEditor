#include "templatecustomwidget.h"
#include <QHBoxLayout>
#include <QScreen>
#include <QStyle>
#include <QDir>
#include "utility.h"

TemplateCustomWidget::TemplateCustomWidget(QWidget *parent)
    : QWidget(parent)
    , templateScriptTreeArea(new QScrollArea(this))
    , templateScriptTree(new QWidget(this))
    , templateScriptTreeLayout(new QVBoxLayout(templateScriptTree))
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
    displayLayout->addWidget(editor);

    templateScriptTreeArea->setLayout(new QVBoxLayout);
    templateScriptTreeArea->setMaximumWidth(200);
    templateScriptTreeArea->setWidgetResizable(true);
    templateScriptTreeLayout->setAlignment(Qt::AlignmentFlag::AlignTop);

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
        templateScriptTreeLayout->addWidget(new TemplateItemWidget(name, templateScriptTree));
    }
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
}





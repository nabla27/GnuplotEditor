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
{
    setWindowFlag(Qt::WindowType::Window, true);
    setGeometry(getRectFromScreenRatio(screen()->size(), 0.4f, 0.4f));

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    QVBoxLayout *templateListLayout = new QVBoxLayout;
    QVBoxLayout *displayLayout = new QVBoxLayout;

    setLayout(hLayout);
    hLayout->addLayout(templateListLayout);
    hLayout->addLayout(displayLayout);

    displayLayout->addWidget(templateScriptTreeArea);
    templateScriptTreeArea->setWidget(templateScriptTree);
    templateScriptTree->setLayout(templateScriptTreeLayout);

    templateScriptTreeArea->setWidgetResizable(true);
    templateScriptTreeLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
    templateScriptTreeLayout->setSpacing(0);
    templateScriptTree->setContentsMargins(0, 0, 0, 0);
    templateScriptTreeArea->setContentsMargins(0, 0, 0, 0);

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
    , scriptName(new QToolButton(this))
    , menuTool(new mlayout::IconLabel(this))
{
    setFixedHeight(40);

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->addWidget(scriptName);
    hLayout->addWidget(menuTool);

    scriptName->setText(name);
    menuTool->setPixmap(QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView).pixmap(20, 20));
    menuTool->setHoveredFrameShape(QFrame::Shape::Box);

    setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(0);
    scriptName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}





#include "filetreesettingwidget.h"
#include "boost/property_tree/xml_parser.hpp"
#include "boost/lexical_cast.hpp"
#include <QFile>
#include <QDir>

#include <QScreen>
#include "utility.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QStackedWidget>

FileTreeSettingWidget::FileTreeSettingWidget(QWidget *parent)
    : QWidget{parent}
{
    setGeometry(getRectFromScreenRatio(screen()->size(), 0.2f, 0.3f));

    setupLayout();
}

void FileTreeSettingWidget::setupLayout()
{
    QVBoxLayout *vLayout = new QVBoxLayout;
    QHBoxLayout *hLayout = new QHBoxLayout;
    QVBoxLayout *buttonLayout = new QVBoxLayout;
    QSpacerItem *buttonLayoutSpacer = new QSpacerItem(80, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    QComboBox *settingPageCombo = new QComboBox(this);
    QStackedWidget *settingPage = new QStackedWidget(this);

    setLayout(vLayout);
    vLayout->addWidget(settingPageCombo);
    vLayout->addLayout(hLayout);
    hLayout->addWidget(settingPage);
    hLayout->addLayout(buttonLayout);


    QPushButton *addButton = new QPushButton("Add", this);
    QPushButton *removeButton = new QPushButton("Remove", this);

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);
    buttonLayout->addItem(buttonLayoutSpacer);


    filterList = new QTreeWidget(settingPage);
    scriptExtensionList = new QTreeWidget(settingPage);
    sheetExtensionList = new QTreeWidget(settingPage);

    settingPage->addWidget(filterList);
    settingPage->addWidget(scriptExtensionList);
    settingPage->addWidget(sheetExtensionList);

    settingPageCombo->addItems(QStringList() << "Filter" << "Script" << "Sheet");
    filterList->setHeaderLabel("Filter List");
    scriptExtensionList->setHeaderLabels(QStringList() << "Extension List" << "Read");
    sheetExtensionList->setHeaderLabels(QStringList() << "Extension List" << "Read");

    connect(settingPageCombo, &QComboBox::currentIndexChanged, settingPage, &QStackedWidget::setCurrentIndex);
}

void FileTreeSettingWidget::loadXmlSetting()
{
    using namespace boost::property_tree;

    const QString settingFile = settingFolderPath + '/' + settingFileName;

    if(QFile::exists(settingFile))
    {
        ptree pt;
        read_xml(settingFile.toUtf8().constData(), pt); //存在しないファイルやフォルダーを指定するとエラー(落ちる)

    }
}

void FileTreeSettingWidget::saveXmlSetting()
{
    using namespace boost::property_tree;

    ptree pt;



    constexpr int indent = 4;

    //保存用のフォルダがなければ作成
    QDir dir(settingFolderPath);
    if(!dir.exists())
    {
        QDir dir;
        const bool success = dir.mkdir(settingFolderPath);
        if(!success)
        {
            /* エラー処理を追加する */ qDebug() << __FILE__ << __LINE__;
            return;
        }
    }

    //存在しないフォルダーを含むパスを指定した場合はクラッシュする
    //存在しないファイルは指定しても大丈夫
    write_xml((settingFolderPath + "/" + settingFileName).toUtf8().constData(),
              pt, std::locale(), xml_writer_make_settings<std::string>(' ', indent, "utf-8"));
}

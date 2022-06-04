#include "filetreesettingwidget.h"
#include "boost/property_tree/xml_parser.hpp"
#include "boost/lexical_cast.hpp"
#include <QFile>
#include <QDir>

#include <QScreen>
#include "utility.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QFormLayout>
#include <QDialogButtonBox>

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
    settingPage = new QStackedWidget(this);

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
    connect(addButton, &QPushButton::released, this, &FileTreeSettingWidget::addItem);
    connect(removeButton, &QPushButton::released, this, &FileTreeSettingWidget::removeItem);
}

void FileTreeSettingWidget::addItem()
{
    QWidget *currentPage = settingPage->currentWidget();

    if(currentPage == filterList)
    {
        InputDialog dialog(this, "Filter", "");
        if(dialog.exec() == 0) return;

        filterList->addTopLevelItem(new QTreeWidgetItem(QStringList() << dialog.lineEditText()));
    }
    else if(currentPage == scriptExtensionList)
    {
        InputDialog dialog(this, "Extension", "ReadType");
        dialog.addComboItems(enumToStrings(TreeScriptItem::ReadType(0)));

        if(dialog.exec() == 0) return;

        scriptExtensionList->addTopLevelItem(new QTreeWidgetItem(QStringList() << dialog.lineEditText() << dialog.comboText()));
    }
    else if(currentPage == sheetExtensionList)
    {
        InputDialog dialog(this, "Extension", "ReadType");
        dialog.addComboItems(enumToStrings(TreeSheetItem::ReadType(0)));

        if(dialog.exec() == 0) return;

        sheetExtensionList->addTopLevelItem(new QTreeWidgetItem(QStringList() << dialog.lineEditText() << dialog.comboText()));
    }
}

void FileTreeSettingWidget::removeItem()
{
    QWidget *currentPage = settingPage->currentWidget();

    if(currentPage == filterList)
        ;
    else if(currentPage == scriptExtensionList)
        ;
    else if(currentPage == sheetExtensionList)
        ;
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








InputDialog::InputDialog(QWidget *parent, const QString& lineEditLabel, const QString& comboLabel)
    : QDialog(parent)
    , lineEdit(nullptr)
    , combo(nullptr)
{
    QFormLayout *fLayout = new QFormLayout(this);
    QDialogButtonBox *button = new QDialogButtonBox(QDialogButtonBox::Ok, this);

    setLayout(fLayout);

    if(!lineEditLabel.isEmpty())
    {
        lineEdit = new QLineEdit(this);
        fLayout->addRow(lineEditLabel, lineEdit);
    }
    if(!comboLabel.isEmpty())
    {
        combo = new QComboBox(this);
        fLayout->addRow(comboLabel, combo);
    }

    fLayout->addRow(button);

    connect(button, &QDialogButtonBox::accepted, this, &InputDialog::accept);
}










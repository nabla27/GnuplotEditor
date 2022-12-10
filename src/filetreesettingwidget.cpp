/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "filetreesettingwidget.h"
#include <QFile>
#include <QDir>

#include <QScreen>
#include "utility.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QFormLayout>
#include <QDialogButtonBox>

#include "logger.h"


FileTreeSettingWidget::FileTreeSettingWidget(QWidget *parent)
    : QWidget{parent}
{
    setGeometry(getRectFromScreenRatio(screen()->size(), 0.2f, 0.2f));

    setupLayout();

    loadXmlSetting();
}

FileTreeSettingWidget::~FileTreeSettingWidget()
{
    saveXmlSetting();
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
    QPushButton *editButton = new QPushButton("Edit", this);
    QPushButton *reloadButton = new QPushButton("Reload", this);

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addItem(buttonLayoutSpacer);
    buttonLayout->addWidget(reloadButton);


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
    connect(editButton, &QPushButton::released, this, &FileTreeSettingWidget::editItem);
    connect(reloadButton, &QPushButton::released, this, &FileTreeSettingWidget::requestReload);
}

void FileTreeSettingWidget::addScriptExt(const QString& ext, const int index)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(scriptExtensionList,
                                                QStringList() << ext
                                                              << enumToString(TreeScriptItem::ReadType(index))
                                                              << QString::number(index));
    item->setIcon(0, QIcon(":/icon/file_code"));
    scriptExtensionList->addTopLevelItem(item);

    TreeScriptItem::suffix.insert(ext, TreeScriptItem::ReadType(index));
}

void FileTreeSettingWidget::addSheetExt(const QString& ext, const int index)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(sheetExtensionList,
                                                QStringList() << ext
                                                              << enumToString(TreeSheetItem::ReadType(index))
                                                              << QString::number(index));
    item->setIcon(0, QIcon(":/icon/file_doc"));
    sheetExtensionList->addTopLevelItem(item);

    TreeSheetItem::suffix.insert(ext, TreeSheetItem::ReadType(index));
}

void FileTreeSettingWidget::addItem()
{
    QWidget *currentPage = settingPage->currentWidget();

    if(currentPage == filterList)
    {
        InputDialog dialog(this, "Filter", "");
        if(dialog.exec() == 0) return;

        filterList->addTopLevelItem(new QTreeWidgetItem(QStringList() << dialog.lineEditText()));
        FileTreeWidget::fileFilter << dialog.lineEditText();
    }
    else if(currentPage == scriptExtensionList)
    {
        InputDialog dialog(this, "Extension", "ReadType");
        dialog.addComboItems(enumToStrings(TreeScriptItem::ReadType(0)));

        if(dialog.exec() == 0) return;

        addScriptExt(dialog.lineEditText(), dialog.comboIndex());
    }
    else if(currentPage == sheetExtensionList)
    {
        InputDialog dialog(this, "Extension", "ReadType");
        dialog.addComboItems(enumToStrings(TreeSheetItem::ReadType(0)));

        if(dialog.exec() == 0) return;

        addSheetExt(dialog.lineEditText(), dialog.comboIndex());
    }
}

void FileTreeSettingWidget::removeItem()
{
    QWidget *currentPage = settingPage->currentWidget();

    if(currentPage == filterList)
    {
        const QList<QTreeWidgetItem*> list = filterList->selectedItems();

        for(QTreeWidgetItem* item : list)
        {
            QTreeWidgetItem *removeItem = filterList->takeTopLevelItem(filterList->indexOfTopLevelItem(item));
            FileTreeWidget::fileFilter.removeOne(removeItem->text(0));
            delete removeItem;
        }
    }
    else if(currentPage == scriptExtensionList)
    {
        const QList<QTreeWidgetItem*> list = scriptExtensionList->selectedItems();

        for(QTreeWidgetItem *item : list)
        {
            QTreeWidgetItem *removeItem = scriptExtensionList->takeTopLevelItem(scriptExtensionList->indexOfTopLevelItem(item));
            TreeScriptItem::suffix.remove(removeItem->text(0));
            delete removeItem;
        }
    }
    else if(currentPage == sheetExtensionList)
    {
        const QList<QTreeWidgetItem*> list = sheetExtensionList->selectedItems();

        for(QTreeWidgetItem *item : list)
        {
            QTreeWidgetItem *removeItem = sheetExtensionList->takeTopLevelItem(sheetExtensionList->indexOfTopLevelItem(item));
            TreeSheetItem::suffix.remove(removeItem->text(0));
            delete removeItem;
        }
    }
}

void FileTreeSettingWidget::editItem()
{
    QWidget *currentPage = settingPage->currentWidget();

    if(currentPage == filterList)
    {
        QList<QTreeWidgetItem*> list = filterList->selectedItems();
        if(list.count() < 1) return;
        QTreeWidgetItem *item = list.at(0);

        InputDialog dialog(this, "filter", "");
        dialog.setData(item->text(0), -1);

        if(dialog.exec() == 0) return;

        FileTreeWidget::fileFilter.replaceInStrings(item->text(0), dialog.lineEditText());
        item->setText(0, dialog.lineEditText());
    }
    else if(currentPage == scriptExtensionList)
    {
        QList<QTreeWidgetItem*> list = scriptExtensionList->selectedItems();
        if(list.count() < 1) return;
        QTreeWidgetItem *item = list.at(0);

        InputDialog dialog(this, "Extension", "ReadType");
        dialog.setData(item->text(0), item->text(2).toInt());
        dialog.addComboItems(enumToStrings(TreeScriptItem::ReadType(0)));

        if(dialog.exec() == 0) return;

        TreeScriptItem::suffix.remove(item->text(0));

        item->setText(0, dialog.lineEditText());
        item->setText(1, enumToString(TreeScriptItem::ReadType(dialog.comboIndex())));
        item->setText(2, QString::number(dialog.comboIndex()));

        TreeScriptItem::suffix.insert(item->text(0), TreeScriptItem::ReadType(item->text(2).toInt()));
    }
    else if(currentPage == sheetExtensionList)
    {
        QList<QTreeWidgetItem*> list = sheetExtensionList->selectedItems();
        if(list.count() < 1) return;
        QTreeWidgetItem *item = list.at(0);

        InputDialog dialog(this, "Extension", "ReadType");
        dialog.setData(item->text(0), item->text(2).toInt());
        dialog.addComboItems(enumToStrings(TreeSheetItem::ReadType(0)));

        if(dialog.exec() == 0) return;

        TreeSheetItem::suffix.remove(item->text(0));

        item->setText(0, dialog.lineEditText());
        item->setText(1, enumToString(TreeSheetItem::ReadType(dialog.comboIndex())));
        item->setText(2, QString::number(dialog.comboIndex()));

        TreeSheetItem::suffix.insert(item->text(0), TreeSheetItem::ReadType(item->text(2).toInt()));
    }
}

void FileTreeSettingWidget::requestReload()
{
    logger->output(__FILE__, __LINE__, __FUNCTION__,
                   "filetree reload requested from " + QString(FileTreeSettingWidget::staticMetaObject.className()), Logger::LogLevel::Info);

    hide();
    emit reloadRequested();
}










/*          Copyright Joe Coder 2004 - 2006.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://www.boost.org/LICENSE_1_0.txt)
 */
#include "boost/property_tree/xml_parser.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/foreach.hpp"

void FileTreeSettingWidget::loadXmlSetting()
{
    using namespace boost::property_tree;

    const QString settingFile = settingFolderPath + '/' + settingFileName;

    if(QFile::exists(settingFile))
    {
        logger->output(__FILE__, __LINE__, __FUNCTION__,
                       "file tree setting xml file is loaded.", Logger::LogLevel::Info);

        ptree pt;
        read_xml(settingFile.toUtf8().constData(), pt); //存在しないファイルやフォルダーを指定するとエラー(落ちる)

        if(boost::optional<std::string> path = pt.get_optional<std::string>("root.previousPath"))
            _previousFolderPath = QString::fromStdString(path.value());

        BOOST_FOREACH(const ptree::value_type& child, pt.get_child("root.filterList"))
        {
            const std::string filter = boost::lexical_cast<std::string>(child.second.data());
            filterList->addTopLevelItem(new QTreeWidgetItem(QStringList() << QString::fromStdString(filter)));
            FileTreeWidget::fileFilter << QString::fromStdString(filter);
        }


        BOOST_FOREACH(const ptree::value_type& child, pt.get_child("root.scriptExtList"))
            if(const boost::optional<std::string>& ext = child.second.get_optional<std::string>("ext"))
                if(const boost::optional<int>& readType = child.second.get_optional<int>("readType"))
                    addScriptExt(QString::fromStdString(ext.value()), readType.value());


        BOOST_FOREACH(const ptree::value_type& child, pt.get_child("root.sheetExtList"))
            if(const boost::optional<std::string>& ext = child.second.get_optional<std::string>("ext"))
                if(const boost::optional<int>& readType = child.second.get_optional<int>("readType"))
                    addSheetExt(QString::fromStdString(ext.value()), readType.value());

    }
    else
    {
        logger->output(__FILE__, __LINE__, __FUNCTION__,
                       "file tree setting xml file was not found.", Logger::LogLevel::Warn);
    }
}

void FileTreeSettingWidget::saveXmlSetting()
{
    using namespace boost::property_tree;

    ptree pt;

    pt.add("root.previousPath", _previousFolderPath.toUtf8().constData());

    ptree& childFilter = pt.add("root.filterList", "");
    for(int i = 0; i < filterList->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *item = filterList->topLevelItem(i);
        const std::string filter = item->text(0).toStdString();

        childFilter.add("filter", filter);
    }


    ptree& childScriptExt = pt.add("root.scriptExtList", "");
    for(int i = 0; i < scriptExtensionList->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *item = scriptExtensionList->topLevelItem(i);
        const std::string ext = item->text(0).toStdString();
        const int  readType = item->text(2).toInt();

        ptree& childItem = childScriptExt.add("item", "");
        childItem.add("ext", ext);
        childItem.add("readType", readType);
    }


    ptree& childSheetExt = pt.add("root.sheetExtList", "");
    for(int i = 0; i < sheetExtensionList->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *item = sheetExtensionList->topLevelItem(i);
        const std::string ext = item->text(0).toStdString();
        const int readType = item->text(2).toInt();

        ptree& childItem = childSheetExt.add("item", "");
        childItem.add("ext", ext);
        childItem.add("readType", readType);
    }


    constexpr int indent = 4;

    //保存用のフォルダがなければ作成
    QDir dir(settingFolderPath);
    if(!dir.exists())
    {
        QDir dir;
        const bool success = dir.mkdir(settingFolderPath);
        if(!success)
        {
            logger->output(__FILE__, __LINE__, __FUNCTION__,
                           "failed to make dir " + settingFolderPath + "."
                           "could not save the filetree setting.", Logger::LogLevel::Error);
            return;
        }
    }

    logger->output(__FILE__, __LINE__, __FUNCTION__,
                   "save filetree setting as xml file.", Logger::LogLevel::Info);

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

void InputDialog::setData(const QString &lineEdit, const int index)
{
    if(this->lineEdit) this->lineEdit->setText(lineEdit);
    if(combo) this->combo->setCurrentIndex(index);
}










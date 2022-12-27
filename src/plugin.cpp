/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "plugin.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QToolButton>
#include <QFormLayout>
#include <QLineEdit>
#include <QPalette>
#include <QFileInfo>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>
#include <QLabel>
#include <QSpinBox>


#include "utility.h"
#include "../plugin/editor_plugin.h"




QHash<int, Plugin<editorplugin::EditorPlugin>*> PluginCollection::editorPlugins = {};


PluginListWidget::PluginListWidget(QWidget *parent)
    : QWidget(parent)
    , tabWidget(new QTabWidget(this))
    , editorPluginPage(new EditorPluginPage(tabWidget))
{
    setWindowFlag(Qt::Dialog);

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    setLayout(hLayout);

    hLayout->addWidget(tabWidget);
    tabWidget->addTab(editorPluginPage, "Editor");

    hLayout->setSpacing(0);
    hLayout->setContentsMargins(0, 0, 0, 0);
}

QString PluginListWidget::pluginSettingPath()
{
    return QApplication::applicationDirPath() + "/setting/plugin-list.xml";
}









PluginListWidget::PluginOptionDialog::PluginOptionDialog(QWidget *parent)
    : QDialog(parent)
    , dllPathEdit(new QLineEdit(this))
    , symbolNameEdit(new QLineEdit(this))
    , fileDialog(new QFileDialog(this))
{
    QFormLayout *fLayout = new QFormLayout(this);
    setLayout(fLayout);

    QWidget *inputPathWidget = new QWidget(this);
    QHBoxLayout *inputPathLayout = new QHBoxLayout(inputPathWidget);
    QToolButton *fileDialogButton = new QToolButton(inputPathWidget);

    QWidget *buttonWidget = new QWidget(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);

    inputPathWidget->setLayout(inputPathLayout);
    inputPathLayout->addWidget(dllPathEdit);
    inputPathLayout->addWidget(fileDialogButton);

    buttonWidget->setLayout(buttonLayout);
    buttonLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    fLayout->addRow("DLL Path", inputPathWidget);
    fLayout->addRow("Symbol Name", symbolNameEdit);
    fLayout->addRow("", buttonWidget);

    inputPathWidget->setContentsMargins(0, 0, 0, 0);
    inputPathLayout->setSpacing(0);
    inputPathLayout->setContentsMargins(0, 0, 0, 0);
    buttonWidget->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(0);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    dllPathEdit->setAutoFillBackground(true);
    fileDialogButton->setText("...");
    fileDialogButton->setToolTip("Open file dialog");
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    fileDialog->setNameFilter("Dll (*.dll)");

    connect(dllPathEdit, &QLineEdit::textChanged, this, &PluginOptionDialog::checkFileExists);
    connect(fileDialogButton, &QToolButton::released, fileDialog, &QFileDialog::exec);
    connect(fileDialog, &QFileDialog::fileSelected, dllPathEdit, &QLineEdit::setText);
    connect(okButton, &QPushButton::released, this, &PluginOptionDialog::accept);
    connect(cancelButton, &QPushButton::released, this, &PluginOptionDialog::reject);
}

void PluginListWidget::PluginOptionDialog::setDllPathText(const QString &text)
{
    dllPathEdit->setText(text);
}

void PluginListWidget::PluginOptionDialog::setSymbolNameText(const QString &text)
{
    symbolNameEdit->setText(text);
}

QString PluginListWidget::PluginOptionDialog::dllPath() const
{
    return dllPathEdit->text();
}

QString PluginListWidget::PluginOptionDialog::symbolName() const
{
    return symbolNameEdit->text();
}

void PluginListWidget::PluginOptionDialog::checkFileExists(const QString& path)
{
    QFileInfo info(path);

    static QPalette p(dllPathEdit->palette());

    if(info.exists() && info.isFile())
    {
        p.setColor(dllPathEdit->foregroundRole(), Qt::black);
    }
    else
    {
        p.setColor(dllPathEdit->foregroundRole(), Qt::red);
    }

    dllPathEdit->setPalette(p);
}















PluginListWidget::EditorPluginPage::EditorPluginPage(QWidget *parent)
    : QWidget(parent)
    , listWidget(new QTreeWidget(this))
    , settingButton(new QPushButton("Setting", this))
{
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    QVBoxLayout *vLayout = new QVBoxLayout;
    QPushButton *addButton = new QPushButton("Add", this);
    QPushButton *removeButton = new QPushButton("Remove", this);
    QPushButton *editButton = new QPushButton("Edit", this);

    setLayout(hLayout);
    hLayout->addWidget(listWidget);
    hLayout->addLayout(vLayout);
    vLayout->addWidget(addButton);
    vLayout->addWidget(removeButton);
    vLayout->addWidget(editButton);
    vLayout->addWidget(settingButton);
    vLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));

    hLayout->setSpacing(0);
    hLayout->setContentsMargins(0, 0, 0, 0);

    listWidget->setColumnCount(mutility::getEnumCount(ItemIndex(0)));
    listWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    listWidget->setHeaderLabels(mutility::enumToStrings(ItemIndex(0)));
    settingButton->setEnabled(false);

    connect(listWidget, &QTreeWidget::itemSelectionChanged, this, &EditorPluginPage::setButtonEnable);
    connect(listWidget->model(), &QAbstractItemModel::dataChanged, this, &EditorPluginPage::changePlugin);
    connect(addButton, &QPushButton::released, this, QOverload<void>::of(&EditorPluginPage::addPluginFromDialog));
    connect(removeButton, &QPushButton::released, this, &EditorPluginPage::removePlugin);
    connect(editButton, &QPushButton::released, this, &EditorPluginPage::editPlugin);
    connect(settingButton, &QPushButton::released, this, &EditorPluginPage::openSettingWidget);

    loadFromXml();
}

PluginListWidget::EditorPluginPage::~EditorPluginPage()
{
    saveAsXml();
}

void PluginListWidget::EditorPluginPage::addPluginFromDialog()
{
    PluginOptionDialog dialog(this);
    const int code = dialog.exec();

    switch(QDialog::DialogCode(code))
    {
    case QDialog::DialogCode::Rejected:
        return;
    case QDialog::DialogCode::Accepted:
        break;
    default:
        return;
    }

    addPlugin(dialog.dllPath(), dialog.symbolName());
}

void PluginListWidget::EditorPluginPage::addPlugin(const QString &dllPath, const QString &symbolName)
{
    Plugin<editorplugin::EditorPlugin> *plugin = new Plugin<editorplugin::EditorPlugin>;
    PluginCollection::editorPlugins.insert(plugin->id(), plugin);

    QTreeWidgetItem *item = new QTreeWidgetItem(listWidget);

    /* 順番に注意
     * addPlugin()でPlugin::load()が呼ばれるのはdllPathが設定されてからである必要がある．
     * そうでないとQLibrary::libraryPath()が空文字である場合にloadしてエラーが起きる．
     * ここでテキストを設定するごとにchangePlugin()が呼ばれる．*/
    item->setText((int)ItemIndex::DLLPath, dllPath);
    item->setText((int)ItemIndex::SymbolName, "createPluginInstance");
    item->setCheckState((int)ItemIndex::Enable, Qt::CheckState::Checked);
    item->setText((int)ItemIndex::ID, QString::number(plugin->id()));
    item->setText((int)ItemIndex::Name, QFileInfo(dllPath).baseName());

    listWidget->addTopLevelItem(item);
}

void PluginListWidget::EditorPluginPage::removePlugin()
{
    if(listWidget->selectedItems().count() < 1)
    {
        __LOGOUT__("item is not selected.", Logger::LogLevel::Warn);
        return;
    }

    QTreeWidgetItem *item = listWidget->selectedItems().first();

    const QMessageBox::StandardButton button
            = QMessageBox::question(this, "Remove Plugin",
                                    "Do you want to remove this DLL \"" + item->text((int)ItemIndex::Name) + "\"");

    if(button != QMessageBox::StandardButton::Yes) return;

    if(Plugin<editorplugin::EditorPlugin> *p = PluginCollection::editorPlugins.take(item->text((int)ItemIndex::ID).toInt()))
    {
        listWidget->takeTopLevelItem(listWidget->indexOfTopLevelItem(item));

        if(settingWidgets.contains(p->id()))
        {
            PluginSettingWidget *w = settingWidgets.value(p->id());
            settingWidgets.remove(p->id());
            if(w) w->deleteLater();
        }

        delete item;
        delete p;
        item = nullptr;
        p = nullptr;
    }
    else
    {
        __LOGOUT__("failed to remove the dll \"" + item->text((int)ItemIndex::Name) + "\".", Logger::LogLevel::Error);
        return;
    }
}

void PluginListWidget::EditorPluginPage::editPlugin()
{
    if(listWidget->selectedItems().count() < 1)
    {
        __LOGOUT__("item is not selected.", Logger::LogLevel::Warn);
        return;
    }

    QTreeWidgetItem *item = listWidget->selectedItems().first();

    PluginOptionDialog dialog(this);
    dialog.setDllPathText(item->text((int)ItemIndex::DLLPath));
    dialog.setSymbolNameText(item->text((int)ItemIndex::SymbolName));
    const int code = dialog.exec();

    switch(code)
    {
    case QDialog::Accepted:
        break;
    case QDialog::Rejected:
        return;
    default:
        return;
    }

    item->setText((int)ItemIndex::DLLPath, dialog.dllPath());
    item->setText((int)ItemIndex::SymbolName, dialog.symbolName());
}

void PluginListWidget::EditorPluginPage::setButtonEnable()
{
    if(listWidget->selectedItems().count() < 1)
    {
        settingButton->setEnabled(false);
        return;
    }

    QTreeWidgetItem *item = listWidget->selectedItems().at(0);

    if(!item)
    {
        __LOGOUT__("selected item is nullptr.", Logger::LogLevel::Warn);
        settingButton->setEnabled(false);
        return;
    }

    const int id = item->text((int)ItemIndex::ID).toInt();

    if(settingWidgets.contains(id))
        settingButton->setEnabled(true);
    else
        settingButton->setEnabled(false);
}

void PluginListWidget::EditorPluginPage::openSettingWidget()
{
    if(listWidget->selectedItems().count() < 1)
    {
        __LOGOUT__("no selected items", Logger::LogLevel::Warn);
        return;
    }

    QTreeWidgetItem *item = listWidget->selectedItems().at(0);

    if(!item)
    {
        __LOGOUT__("selected item is nullptr.", Logger::LogLevel::Warn);
        return;
    }

    const int id = item->text((int)ItemIndex::ID).toInt();

    if(QWidget *w = settingWidgets.value(id))
    {
        w->show();
    }
}

void PluginListWidget::EditorPluginPage::changePlugin(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
{
    if(roles.count() == 1 && roles.at(0) == (int)Qt::ItemDataRole::BackgroundRole)
        return;

    if(topLeft != bottomRight)
    {
        __LOGOUT__("no supported model index", Logger::LogLevel::Warn);
        return;
    }

    QTreeWidgetItem *item = listWidget->itemFromIndex(topLeft);

    if(!item)
    {
        __LOGOUT__("changed item is nullptr", Logger::LogLevel::Warn);
        return;
    }


    Plugin<editorplugin::EditorPlugin> *plugin = PluginCollection::editorPlugins.value(item->text((int)ItemIndex::ID).toInt());

    if(!plugin)
    {
        __LOGOUT__("changed plugin is nullptr", Logger::LogLevel::Warn);
        return;
    }

    switch((ItemIndex)topLeft.column())
    {
    case ItemIndex::Enable: //Enable/Disable
    {
        switch(item->checkState((int)ItemIndex::Enable))
        {
        case Qt::CheckState::Checked:
        case Qt::CheckState::PartiallyChecked:
        {
            item->setBackground((int)ItemIndex::Name, QBrush(Qt::white));
            plugin->setEnable(true);
            plugin->load();
            break;
        }
        case Qt::CheckState::Unchecked:
        {
            item->setBackground((int)ItemIndex::Name, QBrush(Qt::red));
            plugin->setEnable(false);
            plugin->unload();
            break;
        }
        default:
            break;
        }

        checkLoadState(item);

        return;
    }
    case ItemIndex::ID: //ID
    case ItemIndex::Name: //Name
    case ItemIndex::Version:
        return;
    case ItemIndex::DLLPath: //DLL Path
    {
        if(plugin->isLoaded()) plugin->unload();

        plugin->setDllPath(item->text((int)ItemIndex::DLLPath));

        QFileInfo info(item->text((int)ItemIndex::DLLPath));

        if(info.exists())
        {
            plugin->load();
            checkLoadState(item);
        }
        else
        {
            item->setBackground((int)ItemIndex::DLLPath, QBrush(Qt::red));
            item->setBackground((int)ItemIndex::SymbolName, QBrush(Qt::red));
        }

        return;
    }
    case ItemIndex::SymbolName: //Symbol Name
    {
        plugin->setSymbol(item->text((int)ItemIndex::SymbolName));
        plugin->resolve();
        checkResolveState(item);
        return;
    }
    default:
        return;
    }
}

void PluginListWidget::EditorPluginPage::checkLoadState(QTreeWidgetItem *item)
{
    if(!item) return;

    Plugin<editorplugin::EditorPlugin> *p = PluginCollection::editorPlugins.value(item->text((int)ItemIndex::ID).toInt());

    if(!p)
    {
        /* 無効になったプラグイン */
        return;
    }

    if(p->isLoaded())
    {
        item->setBackground((int)ItemIndex::DLLPath, QBrush(Qt::white));

        checkResolveState(item);
    }
    else
    {
        item->setBackground((int)ItemIndex::DLLPath, QBrush(Qt::red));
        item->setBackground((int)ItemIndex::SymbolName, QBrush(Qt::red));
    }
}

void PluginListWidget::EditorPluginPage::checkResolveState(QTreeWidgetItem *item)
{
    if(!item) return;

    Plugin<editorplugin::EditorPlugin> *p = PluginCollection::editorPlugins.value(item->text((int)ItemIndex::ID).toInt());

    if(!p)
    {
        /* 無効になったプラグイン */
        return;
    }

    if(p->instance())
    {
        item->setBackground((int)ItemIndex::SymbolName, QBrush(Qt::white));

        PluginInfo info;
        p->instance()->info(info);
        item->setText((int)ItemIndex::Name, QString::fromStdString(info.name));
        item->setText((int)ItemIndex::Version, QString::fromStdString(info.version));

        if(!settingWidgets.contains(p->id()))
        {
            p->instance()->setup();
            std::vector<AbstractPlugin::SettingItem> *items = nullptr;
            p->instance()->settingItems(items);
            PluginSettingWidget *w = new PluginSettingWidget(this, items);
            settingWidgets.insert(p->id(), w);
        }
    }
    else
    {
        item->setBackground((int)ItemIndex::SymbolName, QBrush(Qt::red));

        if(settingWidgets.contains(p->id()))
        {
            PluginSettingWidget *w = settingWidgets.value(p->id());
            settingWidgets.remove(p->id());
            if(w) w->deleteLater();
        }
    }
}


/*          Copyright Joe Coder 2004 - 2006.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://www.boost.org/LICENSE_1_0.txt)
 */
#include "boost/property_tree/xml_parser.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/foreach.hpp"

void PluginListWidget::EditorPluginPage::loadFromXml()
{
    using namespace boost::property_tree;

    const QString filePath = PluginListWidget::pluginSettingPath();

    if(QFile::exists(filePath))
    {
        ptree pt;
        read_xml(filePath.toUtf8().constData(), pt);

        __LOGOUT__("read editor plugin list file \"" + filePath + "\".", Logger::LogLevel::Info);

        boost::optional<std::string> child_editorplugin = pt.get_optional<std::string>("root.editorplugin");
        if(!child_editorplugin)
        {
            qDebug() << "editorplugin not found";
            return;
        }
        boost::optional<std::string> child_dllinfo = pt.get_optional<std::string>("root.editorplugin.dllinfo");
        if(!child_dllinfo)
        {
            qDebug() << "editorplugin.dllinfo not found";
            return;
        }

        BOOST_FOREACH(const ptree::value_type& child, pt.get_child("root.editorplugin")) //root.editorplugin.dllinfoを捜査
        {   //childはroot.editorplugin.dllinfo
            if(const boost::optional<std::string>& dllPath = child.second.get_optional<std::string>("dllpath"))
                if(const boost::optional<std::string>& symbolName = child.second.get_optional<std::string>("symbolname"))
                    addPlugin(QString::fromStdString(dllPath.value()), QString::fromStdString(symbolName.value()));
        }
    }
    else
    {
        __LOGOUT__("dll list file was not found. \"" + filePath + "\".", Logger::LogLevel::Warn);
    }
}

void PluginListWidget::EditorPluginPage::saveAsXml()
{
    /*
     * root --- editorplugin --- dllinfo --- dllpath
     *       |                |           |- symbolname
     *       |                |
     *       |                | - dllinfo --- dllpath
     *       |                             |- symbolname
     *       |- otherplugin
     */


    using namespace boost::property_tree;

    ptree pt;

    ptree& dllList = pt.add("root.editorplugin", "");
    for(int i = 0; i < listWidget->topLevelItemCount(); ++i)
    {
        if(QTreeWidgetItem *item = listWidget->topLevelItem(i))
        {
            ptree& dllInfo = dllList.add("dllinfo", "");
            dllInfo.add("dllpath", item->text((int)ItemIndex::DLLPath).toUtf8().constData());
            dllInfo.add("symbolname", item->text((int)ItemIndex::SymbolName).toUtf8().constData());
        }
    }

    const QString filePath = pluginSettingPath();
    const QString folderPath = QFileInfo(filePath).absolutePath();

    QDir dir(folderPath);
    if(!dir.exists())
    {   //フォルダが存在しない場合は作成する
        if(dir.mkdir(folderPath))
        {
             __LOGOUT__("make a directory \"" + folderPath + "\".", Logger::LogLevel::Info);
        }
        else
        {
             __LOGOUT__("failed to make a directory \"" + folderPath + "\". could not save the editor-plugin list.", Logger::LogLevel::Error);
                return;
        }
    }

    constexpr int indent = 4;
    write_xml(filePath.toUtf8().constData(), pt, std::locale(), xml_writer_make_settings<std::string>(' ', indent, "utf-8"));

    __LOGOUT__("save the editor plugin list.", Logger::LogLevel::Info);
}










PluginSettingWidget::PluginSettingWidget(QWidget *parent, std::vector<AbstractPlugin::SettingItem> *&items)
    : QWidget(parent)
    , items(items)
{
    setWindowFlag(Qt::Dialog);

    if(!items)
    {
        __LOGOUT__("setting items are nullptr.", Logger::LogLevel::Error);
    }
    else
    {
        setupForm();
    }
}

void PluginSettingWidget::setupForm()
{
    QFormLayout *fLayout = new QFormLayout(this);
    setLayout(fLayout);

    for(AbstractPlugin::SettingItem& item : *items)
    {
        QLabel *label = new QLabel(QString::fromStdString(item.name), this);
        label->setToolTip(QString::fromStdString(item.detail));

        switch(item.variant.index())
        {
        case (size_t)AbstractPlugin::SettingItem::VariantType::Int:
        {
            QSpinBox *spinBox = new QSpinBox(this);
            spinBox->setMinimum(-100000);
            spinBox->setMaximum(100000);
            spinBox->setValue(std::get<(size_t)AbstractPlugin::SettingItem::VariantType::Int>(item.variant));


            connect(spinBox, &QSpinBox::valueChanged, [&item](const int value){
                item.variant = value;
            });

            fLayout->addRow(label, spinBox);
            continue;
        }
        case (size_t)AbstractPlugin::SettingItem::VariantType::Double:
        {
            QDoubleSpinBox *spinBox = new QDoubleSpinBox(this);
            spinBox->setMinimum(-100000);
            spinBox->setMaximum(100000);
            spinBox->setValue(std::get<(size_t)AbstractPlugin::SettingItem::VariantType::Double>(item.variant));

            connect(spinBox, &QDoubleSpinBox::valueChanged, [&item](const double& value){
                item.variant = value;
            });

            fLayout->addRow(label, spinBox);
            continue;
        }
        case (size_t)AbstractPlugin::SettingItem::VariantType::StdString:
        {
            QLineEdit *lineEdit = new QLineEdit(this);
            lineEdit->setText(QString::fromStdString(std::get<(size_t)AbstractPlugin::SettingItem::VariantType::StdString>(item.variant)));

            connect(lineEdit, &QLineEdit::textChanged, [&item](const QString& text){
                item.variant = text.toStdString();
            });

            fLayout->addRow(label, lineEdit);
            continue;
        }
        default:
        {
            __LOGOUT__("invalid variant index.", Logger::LogLevel::Warn);

            label->deleteLater();
            continue;
        }
        }
    }
}
















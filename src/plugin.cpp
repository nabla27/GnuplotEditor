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


#include "utility.h"
#include "../plugin/editor_plugin.h"




QHash<int, Plugin<editorplugin::EditorPlugin>*> PluginCollection::editorPlugins = {};


PluginSettingWidget::PluginSettingWidget(QWidget *parent)
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









PluginSettingWidget::PluginOptionDialog::PluginOptionDialog(QWidget *parent)
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

void PluginSettingWidget::PluginOptionDialog::setDllPathText(const QString &text)
{
    dllPathEdit->setText(text);
}

void PluginSettingWidget::PluginOptionDialog::setSymbolNameText(const QString &text)
{
    symbolNameEdit->setText(text);
}

QString PluginSettingWidget::PluginOptionDialog::dllPath() const
{
    return dllPathEdit->text();
}

QString PluginSettingWidget::PluginOptionDialog::symbolName() const
{
    return symbolNameEdit->text();
}

void PluginSettingWidget::PluginOptionDialog::checkFileExists(const QString& path)
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















PluginSettingWidget::EditorPluginPage::EditorPluginPage(QWidget *parent)
    : QWidget(parent)
    , listWidget(new QTreeWidget(this))
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
    vLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));

    hLayout->setSpacing(0);
    hLayout->setContentsMargins(0, 0, 0, 0);

    listWidget->setColumnCount(getEnumCount(ItemIndex(0)));
    listWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    listWidget->setHeaderLabels(enumToStrings(ItemIndex(0)));

    connect(listWidget->model(), &QAbstractItemModel::dataChanged, this, &EditorPluginPage::changePlugin);
    connect(addButton, &QPushButton::released, this, &EditorPluginPage::addPlugin);
    connect(removeButton, &QPushButton::released, this, &EditorPluginPage::removePlugin);
    connect(editButton, &QPushButton::released, this, &EditorPluginPage::editPlugin);
}

void PluginSettingWidget::EditorPluginPage::addPlugin()
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

    Plugin<editorplugin::EditorPlugin> *plugin = new Plugin<editorplugin::EditorPlugin>;
    PluginCollection::editorPlugins.insert(plugin->id(), plugin);

    QTreeWidgetItem *item = new QTreeWidgetItem(listWidget);

    /* 順番に注意
     * addPlugin()でPlugin::load()が呼ばれるのはdllPathが設定されてからである必要がある
     * そうでないとQLibrary::libraryPath()が空文字である場合にloadしてエラーが起きる */
    item->setText((int)ItemIndex::DLLPath, dialog.dllPath());
    item->setText((int)ItemIndex::SymbolName, dialog.symbolName());
    item->setCheckState((int)ItemIndex::Enable, Qt::CheckState::Checked);
    item->setText((int)ItemIndex::ID, QString::number(plugin->id()));
    item->setText((int)ItemIndex::Name, QFileInfo(dialog.dllPath()).baseName());

    listWidget->addTopLevelItem(item);
}

void PluginSettingWidget::EditorPluginPage::removePlugin()
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

void PluginSettingWidget::EditorPluginPage::editPlugin()
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

void PluginSettingWidget::EditorPluginPage::changePlugin(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
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

void PluginSettingWidget::EditorPluginPage::checkLoadState(QTreeWidgetItem *item)
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

void PluginSettingWidget::EditorPluginPage::checkResolveState(QTreeWidgetItem *item)
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
    }
    else
    {
        item->setBackground((int)ItemIndex::SymbolName, QBrush(Qt::red));
    }
}





















/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "editormanager.h"

#include <QStandardItemModel>
#include <QApplication>
#include <QThreadPool>

#include "plugin.h"
#include "../plugin/editor_plugin.h"


EditorManager::EditorManager(QObject *parent)
    : QObject(parent)
{
    for(Plugin<editorplugin::EditorPlugin> *&plugin : PluginCollection::editorPlugins.values())
    {
        if(!plugin) continue;
        if(!plugin->isLoaded()) continue;
        if(!plugin->instance()) continue;

        managers.insert(plugin->id(), plugin->instance()->createInstance());
    }
}

EditorManager::~EditorManager()
{
    for(editorplugin::EditorManager *&manager : managers.values())
    {
        delete manager;
        manager = nullptr;
    }
}

void EditorManager::requestToolTip(const QString &text)
{
    auto i = managers.constBegin();
    auto e = managers.constEnd();

    editorplugin::ToolTip toolTip;

    while(i != e)
    {
        if(const Plugin<editorplugin::EditorPlugin> *const plugin = PluginCollection::editorPlugins.value(i.key()))
        {
            if(plugin->isEnable())
            {
                i.value()->toolTip(text.toStdString(), toolTip);
            }
        }

        ++i;
    }

    if(toolTip.text.size() != 0)
        emit toolTipSet(QString::fromStdString(toolTip.text));
}

void EditorManager::requestModel(const QString &text)
{
    auto i = managers.constBegin();
    auto e = managers.constEnd();

    std::unordered_set<editorplugin::Completion> completions;

    while(i != e)
    {
        if(const Plugin<editorplugin::EditorPlugin> *const plugin = PluginCollection::editorPlugins.value(i.key()))
        {
            if(plugin->isEnable())
            {
                i.value()->completions(text.toStdString(), completions);
            }
        }
    }

    QList<QStandardItem*> items;
    items.reserve(completions.size());

    for(const editorplugin::Completion& c : completions)
    {
        if(c.text.size() != 0)
            items.append(new QStandardItem(QString::fromStdString(c.text)));
    }

    if(items.size() != 0)
    {
        QStandardItemModel *model = new QStandardItemModel(nullptr);
        model->appendRow(items);
        model->moveToThread(QThreadPool::globalInstance()->thread());
        qDebug() << __LINE__;
        emit modelSet(model);
    }
}

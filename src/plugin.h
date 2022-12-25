/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef PLUGIN_H
#define PLUGIN_H

#include <QLibrary>
#include <QWidget>
#include <QDialog>
#include "logger.h"
#include "../plugin/plugin_base.h"



namespace editorplugin { class EditorPlugin; }
class QTreeWidget;
class QTabWidget;
class QLineEdit;
class QFileDialog;
class QTreeWidgetItem;
class PluginSettingWidget;




template <typename InstanceType>
class Plugin
{
public:
    Plugin();

    using InstanceGeneratorType = void(*)(InstanceType*&);

public:
    void setDllPath(const QString& path);
    void setSymbol(const QString& symbol);
    void setEnable(const bool enable) { enableFlag = enable; }
    bool isEnable() const { return enableFlag; }
    bool load();
    bool unload();
    bool resolve();

    bool isLoaded() { return lib.isLoaded(); }
    int id() const { return _id; }
    InstanceType *const instance() const { return _instance; }

private:
    inline static int count = 0;
    const int _id;

    QString dllPath;
    QString symbol;
    QLibrary lib;
    bool enableFlag = true;

    InstanceType* _instance;
};






template <typename T>
Plugin<T>::Plugin()
    : _id(count++)
    , _instance(nullptr)
{
}

template <typename T>
void Plugin<T>::setDllPath(const QString &path)
{
    dllPath = path;
}

template <typename T>
void Plugin<T>::setSymbol(const QString &symbol)
{
    this->symbol = symbol;
}

template <typename T>
bool Plugin<T>::load()
{
    lib.setFileName(dllPath);

    if(lib.load())
    {
        __LOGOUT__("load the dll \"" + dllPath + "\".", Logger::LogLevel::Info);

        return true;
    }
    else
    {
        __LOGOUT__("failed to load the dll \"" + dllPath + "\".", Logger::LogLevel::Error);

        return false;
    }
}

template <typename T>
bool Plugin<T>::unload()
{
    if(lib.unload())
    {
        __LOGOUT__("unload the dll \"" + dllPath + "\".", Logger::LogLevel::Info);

        return true;
    }
    else
    {
        __LOGOUT__("failed to unload the dll \"" + dllPath + "\".", Logger::LogLevel::Error);

        return false;
    }
}

template <typename T>
bool Plugin<T>::resolve()
{
    __LOGOUT__("resolve the symbol ... \"" + symbol + "\".", Logger::LogLevel::Info);

    if(InstanceGeneratorType generate = (InstanceGeneratorType)lib.resolve(symbol.toUtf8().constData()))
    {

        __LOGOUT__("resolve the symbol name \"" + symbol + "\". and call the symbol ...", Logger::LogLevel::Info);

        generate(_instance);

        __LOGOUT__("the symbol name \"" + symbol + "\" is called.", Logger::LogLevel::Info);

        if(_instance)
        {
            return true;
        }
        else
        {
            __LOGOUT__("the symbol \"" + symbol + "\" was resolved. but that symbol has an invalid argument.", Logger::LogLevel::Error);

            return false;
        }
    }

    __LOGOUT__("failed to resolve the symbol \"" + symbol + "\".", Logger::LogLevel::Error);

    return false;
}






struct PluginCollection
{
    static QHash<int, Plugin<editorplugin::EditorPlugin>*> editorPlugins;
};






class PluginListWidget : public QWidget
{
    Q_OBJECT
public:
    PluginListWidget(QWidget *parent);

    enum class ItemIndex { Enable, ID, Name, Version, DLLPath, SymbolName };
    Q_ENUM(ItemIndex)

    QSize sizeHint() const { return QSize(700, QWidget::sizeHint().height()); }

    static QString pluginSettingPath();

private:
    QTabWidget *tabWidget;

    class PluginOptionDialog;

    class EditorPluginPage;
    EditorPluginPage *editorPluginPage;
};






class PluginListWidget::PluginOptionDialog : public QDialog
{
    Q_OBJECT
public:
    PluginOptionDialog(QWidget *parent);

    QSize sizeHint() const override { return QSize(500, QDialog::sizeHint().height()); }

public:
    void setDllPathText(const QString& text);
    void setSymbolNameText(const QString& text);
    QString dllPath() const;
    QString symbolName() const;

private slots:
    void checkFileExists(const QString& path);

private:
    QLineEdit *dllPathEdit;
    QLineEdit *symbolNameEdit;
    QFileDialog *fileDialog;
};







class PluginListWidget::EditorPluginPage : public QWidget
{
    Q_OBJECT

public:
    EditorPluginPage(QWidget *parent);
    ~EditorPluginPage();

private slots:
    void addPluginFromDialog();
    void removePlugin();
    void editPlugin();
    void changePlugin(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles);
    void setButtonEnable();
    void openSettingWidget();

private:
    void addPlugin(const QString& dllPath, const QString& symbolName);
    void checkLoadState(QTreeWidgetItem *item);
    void checkResolveState(QTreeWidgetItem *item);

    void loadFromXml();
    void saveAsXml();

private:
    QTreeWidget *listWidget;
    QPushButton *settingButton;

    QHash<int, PluginSettingWidget*> settingWidgets;
};








class PluginSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PluginSettingWidget(QWidget *parent, std::vector<AbstractPlugin::SettingItem>*& items);

private:
    void setupForm();

private:
    std::vector<AbstractPlugin::SettingItem> *const items;
};









#endif // PLUGIN_H

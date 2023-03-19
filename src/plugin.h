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
#include <QTreeWidgetItem>
#include "logger.h"
#include "../plugin/plugin_base.h"



namespace editorplugin { class EditorPlugin; }
namespace mlayout { class LoopProgressDialog; }
class QTreeWidget;
class QTabWidget;
class QLineEdit;
class QFileDialog;
class PluginSettingWidget;
class QProcess;
class AbstractPlugin;
class QHBoxLayout;










class PluginLoader : public QThread
{
    Q_OBJECT
public:
    explicit PluginLoader(QObject *parent);
    ~PluginLoader();

public:
    int id() const noexcept { return _id; }
    AbstractPlugin *const plugin() { return _plugin; }
    bool isEnable() const { return _isEnable; }
    int libState() const { return _libState; }
    QString libraryPath() const { return libPath; }

public:
    void setLibraryPath(const QString& path);
    void setSymbolName(const QString& symbolName);
    void setEnable(bool enable);

    static QSet<PluginLoader*> pluginLoaders;

protected:
    void run() override;

private slots:
    static void startLoopProgress();
    static void stopLoopProgress();

private:
    inline static int count = 0;

    QString static pluginCheckerPath();

    QLibrary lib;
    AbstractPlugin *_plugin;
    int _libState;

    QString libPath;
    QString symbolName;
    const int _id;
    bool _isEnable = true;

    inline static mlayout::LoopProgressDialog *lpDialog = nullptr;
    inline static int lpCount = 0;

signals:
    void checked(int code);
    void loaded(const PluginInfo& info);
};











class PluginListWidgetItem;
class PluginListWidget : public QWidget
{
    Q_OBJECT
public:
    PluginListWidget(QWidget *parent);
    ~PluginListWidget();

    QSize sizeHint() const override { return QSize(qMax(700, QWidget::sizeHint().width()), QWidget::sizeHint().height()); }

private slots:
    void addPluginFromDialog();
    void removePlugin();
    void editPlugin();
    void openSettingWidget();
    void setButtonEnable();
    void changePluginState(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles);

private:
    static QString pluginListFilePath();

    PluginListWidgetItem* checkSelectedItem() const;

    void addPlugin(const QString& dllPath, const QString& symbolName);

    void loadPluginListFromXml();
    void loadDefaultPlugin();
    void savePluginListAsXml();

private:
    QTreeWidget *treeWidget;
    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *editButton;
    QPushButton *optionButton;

    class PluginOptionDialog;
};










class PluginListWidgetItem : public QObject, public QTreeWidgetItem
{
    Q_OBJECT

public:
    explicit PluginListWidgetItem(QTreeWidget *parent, PluginLoader *p);

    enum class ItemIndex { Enable, ID, Type, Name, Version, LibraryPath, SymbolName };
    Q_ENUM(ItemIndex)

public:
    void setLibNameText(const QString& name);
    void setLibPathText(const QString& path);
    void setSymbolNameText(const QString& symbol);

    PluginLoader *const pluginLoader() { return _pluginLoader; }
    PluginSettingWidget *const settingWidget() { return _settingWidget; }

    QString libNameText() const;
    QString libPathText() const;
    QString symbolNameText() const;

private slots:
    void reflectDllState(const int code);
    void setDllInfo(const PluginInfo& info);

private:
    PluginLoader *const _pluginLoader;
    PluginSettingWidget *const _settingWidget;
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











class PluginSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PluginSettingWidget(QWidget *parent);

public:
    void setupForm(std::vector<AbstractPlugin::SettingItem> *items);

private:
    QHBoxLayout *contentsLayout;
    QWidget *contentsWidget;
    std::vector<AbstractPlugin::SettingItem> *items;
};









#endif // PLUGIN_H

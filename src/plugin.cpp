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
#include <QProcess>
#include <QTimer>


#include "utility.h"
#include "layoutparts.h"
#include "../plugin/plugin_base.h"
#include "../plugin/pluginchecker/checker.h"










QSet<PluginLoader*> PluginLoader::pluginLoaders = {};

PluginLoader::PluginLoader(QObject *parent)
    : QThread(parent)
    , _plugin(nullptr)
    , _libState((int)PluginChecker::ExitCode::NotFound)
    , _id(count++)
{
    if(!lpDialog)
    {
        lpDialog = new mlayout::LoopProgressDialog(nullptr);
        lpDialog->setAutoDisplay(true);
        lpDialog->setMessage("now loading library ...");
        lpDialog->setWindowFlags(lpDialog->windowFlags() | Qt::WindowStaysOnTopHint);
    }

    connect(this, &PluginLoader::started, this, &PluginLoader::startLoopProgress);
    connect(this, &PluginLoader::finished, this, &PluginLoader::stopLoopProgress);

    pluginLoaders.insert(this);
}

PluginLoader::~PluginLoader()
{
    quit();

    if(lib.isLoaded()) lib.unload();

    pluginLoaders.remove(this);

    if(_plugin) delete _plugin;
}

void PluginLoader::setLibraryPath(const QString &path)
{
    if(isRunning())
    {
        __LOGOUT__("is running. could not set the dll path.", Logger::LogLevel::Warn);
        return;
    }

    libPath = path;
}

void PluginLoader::setSymbolName(const QString &symbolName)
{
    if(isRunning())
    {
        __LOGOUT__("is runnig. could not set the symbol name.", Logger::LogLevel::Warn);
        return;
    }

    this->symbolName = symbolName;
}

void PluginLoader::setEnable(bool enable)
{
    if(_isEnable == enable) return;

    _isEnable = enable;

    if(enable)
    {
        start();
    }
    else
    {
        if(isRunning())
        {
            quit();
        }

        lib.unload();
    }
}

QString PluginLoader::pluginCheckerPath()
{
    return "E:/repos/qt_project/LabQ/GnuplotEditor/plugin/pluginchecker/bin/release/pluginchecker.exe";
}

void PluginLoader::run()
{
    if(_plugin)
    {
        delete _plugin;
        _plugin = nullptr;
    }
    if(lib.isLoaded())
    {
        lib.unload();
    }

    QProcess *checkProcess = new QProcess(nullptr);
    connect(this, &PluginLoader::destroyed, checkProcess, &QProcess::close);
    connect(this, &PluginLoader::destroyed, checkProcess, &QProcess::deleteLater);

    QFile file(pluginCheckerPath());
    if(!file.exists())
    {
        /* plugin checker が無かった場合，QProcess::exitCode() はゼロであり，
         * これは PluginChecker::ExitCode::ValidLibrary 相当し，チェックできないまま，
         * ロードすることになる．*/
        __LOGOUT__("plugin checker does not exists. \"" + pluginCheckerPath() + "\".", Logger::LogLevel::Warn);
    }

    {
        QElapsedTimer timer;
        timer.start();
        static constexpr int waitTime = 2 * 1000;
        checkProcess->start(pluginCheckerPath(), QStringList() << libPath << symbolName);
        checkProcess->waitForFinished(waitTime);
        if(timer.elapsed() > waitTime - 500)
        {
            __LOGOUT__("plugin checker time out.", Logger::LogLevel::Info);

            _libState = (int)PluginChecker::ExitCode::InvalidLibrary;
            emit checked((int)PluginChecker::ExitCode::InvalidLibrary);
            return;
        }
    }


    _libState = checkProcess->exitCode();
    const QProcess::ExitStatus exitStatus = checkProcess->exitStatus();

    checkProcess->disconnect(this);
    checkProcess->close();
    checkProcess->deleteLater();

    switch(exitStatus)
    {
    case QProcess::ExitStatus::NormalExit:
    {
        const PluginChecker::ExitCode exitCode = PluginChecker::ExitCode(_libState);

        switch(PluginChecker::ExitCode(exitCode))
        {
        case PluginChecker::ExitCode::InvalidArguments:
        {
            __LOGOUT__("plugin checker received invalid arguments", Logger::LogLevel::Debug);
            break;
        }
        case PluginChecker::ExitCode::NotFound:
        case PluginChecker::ExitCode::FailedToLoad:
        case PluginChecker::ExitCode::FailedToResolve:
        case PluginChecker::ExitCode::FailedToCreateInstance:
        case PluginChecker::ExitCode::InvalidLibrary:
        {
            __LOGOUT__("failed to load the plugin \"" + libPath + "\", "
                       "the symbol name \"" + symbolName + "\". "
                       "[" + mutility::enumToString(exitCode) + "]", Logger::LogLevel::Warn);
            break;
        }
        case PluginChecker::ExitCode::ValidLibrary:
        {
            if(!_isEnable) return;

            lib.setFileName(libPath);

            CreatePluginInstanceFuncType func = (CreatePluginInstanceFuncType)lib.resolve(symbolName.toUtf8().constData());

            if(func)
            {
                _plugin = func();

                if(_plugin)
                {
                    /* すべてが正常に読み込めた場合．
                     * plugin checkerが無く，チェック出来なかった場合で，不正なライブラリであれば
                     * ここでクラッシュする */
                    emit loaded(_plugin->info);
                }
                else
                {
                    /* plugin checker で ValidLibraryであった，またはplugin checkerが存在しなかった場合で，
                     * インスタンスの取得に失敗した場合 */
                    __LOGOUT__("failed to create plugin instance.", Logger::LogLevel::Warn);
                    _libState = (int)PluginChecker::ExitCode::FailedToCreateInstance;
                    emit checked((int)PluginChecker::ExitCode::FailedToCreateInstance);
                    return;
                }
            }
            else
            {
                /* plugin checker で ValidLibraryであった．またはplugin checker が存在しなかった場合で，
                 * シンボルが解決されなかった時 */
                __LOGOUT__("the symbol function is nullptr.", Logger::LogLevel::Warn);
                _libState = (int)PluginChecker::ExitCode::FailedToResolve;
                emit checked((int)PluginChecker::ExitCode::FailedToResolve);
                return;
            }

            break;
        }
        default:
        {
            __LOGOUT__("invalid exit code.", Logger::LogLevel::Debug);
            return;
        }
        }

        emit checked((int)exitCode);
        break;
    }
    case QProcess::ExitStatus::CrashExit:
    {
        emit checked((int)PluginChecker::ExitCode::InvalidLibrary);
        break;
    }
    default:
    {
        __LOGOUT__("invalid exit status", Logger::LogLevel::Debug);
        return;
    }
    }
}

void PluginLoader::startLoopProgress()
{
    lpCount++;
    lpDialog->start();
}

void PluginLoader::stopLoopProgress()
{
    lpCount--;

    if(lpCount == 0) lpDialog->stop();
}














PluginListWidget::PluginListWidget(QWidget *parent)
    : QWidget(parent)
    , treeWidget(new QTreeWidget(this))
    , addButton(new QPushButton("Add", this))
    , removeButton(new QPushButton("Remove", this))
    , editButton(new QPushButton("Edit", this))
    , optionButton(new QPushButton("Option", this))
{
    setWindowFlag(Qt::Dialog);

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    QVBoxLayout *vLayout = new QVBoxLayout;

    setLayout(hLayout);

    hLayout->addWidget(treeWidget);
    hLayout->addLayout(vLayout);
    vLayout->addWidget(addButton);
    vLayout->addWidget(removeButton);
    vLayout->addWidget(editButton);
    vLayout->addWidget(optionButton);
    vLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));

    hLayout->setSpacing(0);
    hLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);

    treeWidget->setColumnCount(mutility::getEnumCount(PluginListWidgetItem::ItemIndex(0)));
    treeWidget->setHeaderLabels(mutility::enumToStrings(PluginListWidgetItem::ItemIndex(0)));
    treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    removeButton->setEnabled(false);
    editButton->setEnabled(false);
    optionButton->setEnabled(false);

    connect(addButton, &QPushButton::released, this, &PluginListWidget::addPluginFromDialog);
    connect(removeButton, &QPushButton::released, this, &PluginListWidget::removePlugin);
    connect(editButton, &QPushButton::released, this, &PluginListWidget::editPlugin);
    connect(optionButton, &QPushButton::released, this, &PluginListWidget::openSettingWidget);

    connect(treeWidget, &QTreeWidget::itemSelectionChanged, this, &PluginListWidget::setButtonEnable);
    connect(treeWidget->model(), &QAbstractItemModel::dataChanged, this, &PluginListWidget::changePluginState);

    loadPluginListFromXml();
}

PluginListWidget::~PluginListWidget()
{
    savePluginListAsXml();
}

QString PluginListWidget::pluginListFilePath()
{
    return QApplication::applicationDirPath() + "/setting/plugin-list.xml";
}

void PluginListWidget::addPluginFromDialog()
{
    PluginOptionDialog dialog(this);

    switch(QDialog::DialogCode(dialog.exec()))
    {
    case QDialog::DialogCode::Rejected: return;
    case QDialog::DialogCode::Accepted: break;
    default: return;
    }

    addPlugin(dialog.dllPath(), dialog.symbolName());
}

void PluginListWidget::addPlugin(const QString &dllPath, const QString &symbolName)
{
    PluginLoader *ploader = new PluginLoader(this);
    PluginListWidgetItem *item = new PluginListWidgetItem(treeWidget, ploader);

    ploader->setLibraryPath(dllPath);
    ploader->setSymbolName(symbolName);

    item->setLibNameText(QFileInfo(dllPath).baseName());
    item->setLibPathText(dllPath);
    item->setSymbolNameText(symbolName);

    treeWidget->addTopLevelItem(item);

    ploader->start();
}

void PluginListWidget::removePlugin()
{
    PluginListWidgetItem *item = checkSelectedItem();

    if(!item) return;

    const QMessageBox::StandardButton button
            = QMessageBox::question(this, "Remove Plugin",
                                    "Do you want to remove this Library \"" + item->libPathText() + "\" ??");

    if(button != QMessageBox::StandardButton::Yes) return;

    treeWidget->takeTopLevelItem(treeWidget->indexOfTopLevelItem(item));

    /* PluginListWidgetItem::destloyed -> PluginLoader::deleteLater()          -> delete AbstractPlugin
     *                                                                         -> QSet<PluginLoader*>::remove()
     *                                 -> PluginSettingWidget::deleteLater()
     */

    item->deleteLater();
}

void PluginListWidget::editPlugin()
{
    PluginListWidgetItem *item = checkSelectedItem();

    if(!item) return;

    PluginOptionDialog dialog(this);
    dialog.setDllPathText(item->libPathText());
    dialog.setSymbolNameText(item->symbolNameText());

    switch(dialog.exec())
    {
    case QDialog::DialogCode::Accepted:
        break;
    case QDialog::DialogCode::Rejected:
        return;
    default:
    {
        __LOGOUT__("invalid dialog code.", Logger::LogLevel::Debug);
        return;
    }
    }

    PluginLoader *ploader = item->pluginLoader();

    if(!ploader)
    {
        __LOGOUT__("selected plugin loader is nullptr.", Logger::LogLevel::Warn);
        return;
    }

    item->setLibPathText(dialog.dllPath());
    item->setSymbolNameText(dialog.symbolName());

    ploader->setLibraryPath(dialog.dllPath());
    ploader->setSymbolName(dialog.symbolName());

    ploader->start();
}

void PluginListWidget::openSettingWidget()
{
    PluginListWidgetItem *item = checkSelectedItem();

    if(!item) return;

    if(PluginSettingWidget *w =  item->settingWidget())
    {
        w->show();
    }
    else
    {
        __LOGOUT__("plugin setting widget is nullptr.", Logger::LogLevel::Warn);
        return;
    }
}

void PluginListWidget::setButtonEnable()
{
    if(treeWidget->selectedItems().count() < 1)
    {
        removeButton->setEnabled(false);
        editButton->setEnabled(false);
        optionButton->setEnabled(false);

        return;
    }

    removeButton->setEnabled(true);
    editButton->setEnabled(true);

    PluginListWidgetItem *item = static_cast<PluginListWidgetItem*>(treeWidget->selectedItems().at(0));

    if(!item)
    {
        __LOGOUT__("selected item is nullptr.", Logger::LogLevel::Warn);
        optionButton->setEnabled(false);
        return;
    }

    PluginLoader *ploader = item->pluginLoader();

    if(!ploader)
    {
        __LOGOUT__("selected plugin loader is nullptr.", Logger::LogLevel::Warn);
        optionButton->setEnabled(false);
        return;
    }

    if(ploader->libState() == (int)PluginChecker::ExitCode::ValidLibrary)
        optionButton->setEnabled(true);
    else
        optionButton->setEnabled(false);
}

void PluginListWidget::changePluginState(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
{
    if(topLeft != bottomRight)
    {
        __LOGOUT__("different model index. some model indexes are changed.", Logger::LogLevel::Debug);
        return;
    }

    PluginListWidgetItem *item = static_cast<PluginListWidgetItem*>(treeWidget->topLevelItem(topLeft.row()));

    if(!item)
    {
        __LOGOUT__("changed item is nullptr.", Logger::LogLevel::Warn);
        return;
    }

    if(roles.contains((int)Qt::ItemDataRole::CheckStateRole))
    {
        switch(item->checkState((int)PluginListWidgetItem::ItemIndex::Enable))
        {
        case Qt::CheckState::Unchecked:
        {
            item->pluginLoader()->setEnable(false);
            break;
        }
        case Qt::CheckState::PartiallyChecked:
        case Qt::CheckState::Checked:
        {
            item->pluginLoader()->setEnable(true);
            break;
        }
        default:
        {
            __LOGOUT__("invalid check state.", Logger::LogLevel::Debug);
            return;
        }
        }
    }
}

PluginListWidgetItem* PluginListWidget::checkSelectedItem() const
{
    if(treeWidget->selectedItems().count() < 1)
    {
        __LOGOUT__("no item selected.", Logger::LogLevel::Warn);
        return nullptr;
    }

    PluginListWidgetItem *item = static_cast<PluginListWidgetItem*>(treeWidget->selectedItems().at(0));

    if(!item)
    {
        __LOGOUT__("selected item is nullptr.", Logger::LogLevel::Warn);
    }

    return item;
}


/*          Copyright Joe Coder 2004 - 2006.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://www.boost.org/LICENSE_1_0.txt)
 */
#include "boost/property_tree/xml_parser.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/foreach.hpp"

void PluginListWidget::loadPluginListFromXml()
{
    using namespace boost::property_tree;

    const QString filePath = pluginListFilePath();

    if(!QFile::exists(filePath))
    {
        __LOGOUT__("dll list file was not fount. \"" + filePath + "\".", Logger::LogLevel::Warn);
        return;
    }

    ptree pt;

    __LOGOUT__("start to load the plugin list file \"" + filePath + "\".", Logger::LogLevel::Info);

    read_xml(filePath.toUtf8().constData(), pt);

    __LOGOUT__("start to parse the plugin list file \"" + filePath + "\".", Logger::LogLevel::Info);

    boost::optional<std::string> child_libinfo = pt.get_optional<std::string>("root.libinfo");
    if(!child_libinfo)
    {
        __LOGOUT__("xml child option \"root.libinfo\" was not found.", Logger::LogLevel::Info);
        loadDefaultPlugin();
        return;
    }

    int count = 0;

    BOOST_FOREACH(const ptree::value_type& child, pt.get_child("root")) //root.libinfo
    {
        if(const boost::optional<std::string>& libPath = child.second.get_optional<std::string>("libpath"))
            if(const boost::optional<std::string>& symbolName = child.second.get_optional<std::string>("symbolname"))
            {
                addPlugin(QString::fromStdString(libPath.value()), QString::fromStdString(symbolName.value()));
                count++;
            }
    }

    if(count == 0)
        loadDefaultPlugin();

    __LOGOUT__("finished loading plugin list file \"" + filePath + "\".", Logger::LogLevel::Info);
}

void PluginListWidget::loadDefaultPlugin()
{
    __LOGOUT__("load default plugins.", Logger::LogLevel::Info);

    static QString pluginsFolder = QApplication::applicationDirPath() + "/m_plugins/";

    static auto isLoaded = [this](const QString& path) -> bool
    {
        for(auto ploader : qAsConst(PluginLoader::pluginLoaders))
        {
            if(ploader->libraryPath() == path)
                return true;
        }

        return false;
    };

    {
        static QString libPath = pluginsFolder + "gnuplot-completion.dll";
        if(!isLoaded(libPath))
        {
            addPlugin(libPath, "createPluginInstance");
        }
    }
}

void PluginListWidget::savePluginListAsXml()
{
    /*
     * root --|-- libinfo --|-- libpath
     *        |             |-- symbolname
     *        |
     *        |-- libinfo --|-- libpath
     *        |             |-- symbolname
     *        |
     */

    __LOGOUT__("start to save the plugin list as xml file.", Logger::LogLevel::Info);

    using namespace boost::property_tree;

    ptree pt;

    for(int i = 0; i < treeWidget->topLevelItemCount(); ++i)
    {
        if(PluginListWidgetItem *item = static_cast<PluginListWidgetItem*>(treeWidget->topLevelItem(i)))
        {
            ptree& libinfo = pt.add("root.libinfo", "");
            libinfo.add("libpath", item->libPathText().toUtf8().constData());
            libinfo.add("symbolname", item->symbolNameText().toUtf8().constData());
        }
        else
        {
            __LOGOUT__("the item is nullptr. failed to save the plugin info.", Logger::LogLevel::Warn);
        }
    }

    const QString filePath = pluginListFilePath();
    const QString folderPath = QFileInfo(filePath).absolutePath();

    QDir dir(folderPath);
    if(!dir.exists())
    {   //フォルダが存在しない場合は作成する．親ディレクトリが存在しないままwrite_xml()を実行するとクラッシュ
        if(dir.mkdir(folderPath))
        {
            __LOGOUT__("make a directory \"" + folderPath + "\".", Logger::LogLevel::Info);
        }
        else
        {
            __LOGOUT__("failed to make a directory \"" + folderPath + "\". could not save the plugin list info.", Logger::LogLevel::Error);
            return;
        }
    }

    constexpr int indent = 4;
    write_xml(filePath.toUtf8().constData(), pt, std::locale(), xml_writer_make_settings<std::string>(' ', indent, "utf-8"));

    __LOGOUT__("finished saving the plugin list info.", Logger::LogLevel::Info);
}














PluginListWidgetItem::PluginListWidgetItem(QTreeWidget *parent, PluginLoader *p)
    : QObject(parent)
    , QTreeWidgetItem(parent)
    , _pluginLoader(p)
    , _settingWidget(new PluginSettingWidget(nullptr))
{
    connect(p, &PluginLoader::checked, this, &PluginListWidgetItem::reflectDllState);
    connect(p, &PluginLoader::loaded, this, &PluginListWidgetItem::setDllInfo);
    connect(this, &PluginListWidgetItem::destroyed, _settingWidget, &PluginSettingWidget::deleteLater);
    connect(this, &PluginListWidgetItem::destroyed, _pluginLoader, &PluginLoader::deleteLater);

    setCheckState((int)ItemIndex::Enable, Qt::CheckState::Checked);
    setText((int)ItemIndex::ID, QString::number(p->id()));
}

void PluginListWidgetItem::setLibNameText(const QString& name)
{
    setText((int)ItemIndex::Name, name);
}

void PluginListWidgetItem::setLibPathText(const QString &path)
{
    setText((int)ItemIndex::LibraryPath, path);
}

void PluginListWidgetItem::setSymbolNameText(const QString &symbol)
{
    setText((int)ItemIndex::SymbolName, symbol);
}

QString PluginListWidgetItem::libNameText() const
{
    return text((int)ItemIndex::Name);
}

QString PluginListWidgetItem::libPathText() const
{
    return text((int)ItemIndex::LibraryPath);
}

QString PluginListWidgetItem::symbolNameText() const
{
    return text((int)ItemIndex::SymbolName);
}

void PluginListWidgetItem::reflectDllState(const int code)
{
    static QColor nameColor = Qt::white;
    static QColor pathColor = Qt::white;
    static QColor symbolColor = Qt::white;

    switch(PluginChecker::ExitCode(code))
    {
    case PluginChecker::ExitCode::ValidLibrary:
    {
        nameColor = Qt::white;
        pathColor = Qt::white;
        symbolColor = Qt::white;

        _settingWidget->setupForm(&_pluginLoader->plugin()->items);

        break;
    }
    case PluginChecker::ExitCode::InvalidArguments:
    {
        __LOGOUT__("plugin checker received invalid arguments", Logger::LogLevel::Debug);
    }
    case PluginChecker::ExitCode::NotFound:
    case PluginChecker::ExitCode::FailedToLoad:
    {
        nameColor = Qt::red;
        pathColor = Qt::red;
        symbolColor = Qt::red;
        break;
    }
    case PluginChecker::ExitCode::FailedToResolve:
    case PluginChecker::ExitCode::FailedToCreateInstance:
    case PluginChecker::ExitCode::InvalidLibrary:
    {
        nameColor = Qt::red;
        pathColor = Qt::white;
        symbolColor = Qt::red;
        break;
    }
    default:
    {
        nameColor = Qt::red;
        pathColor = Qt::white;
        symbolColor = Qt::red;

        __LOGOUT__("invalid exit code.", Logger::LogLevel::Debug);
        break;
    }
    }

    setToolTip((int)ItemIndex::Name, mutility::enumToString(PluginChecker::ExitCode(code)));

    setBackground((int)ItemIndex::Name, QBrush(nameColor));
    setBackground((int)ItemIndex::LibraryPath, QBrush(pathColor));
    setBackground((int)ItemIndex::SymbolName, QBrush(symbolColor));
}

void PluginListWidgetItem::setDllInfo(const PluginInfo &info)
{
    setText((int)ItemIndex::Type, QString::number((int)info.type));
    setText((int)ItemIndex::Name, QString::fromStdString(info.name));
    setText((int)ItemIndex::Version, QString::fromStdString(info.version));
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













PluginSettingWidget::PluginSettingWidget(QWidget *parent)
    : QWidget(parent)
    , contentsLayout(new QHBoxLayout(this))
    , contentsWidget(nullptr)
{
    setWindowFlag(Qt::Dialog);

    setLayout(contentsLayout);
}

void PluginSettingWidget::setupForm(std::vector<AbstractPlugin::SettingItem> *items)
{
    this->items = items;

    if(contentsWidget) contentsWidget->deleteLater();
    contentsWidget = new QWidget(this);
    contentsLayout->addWidget(contentsWidget);

    QFormLayout *fLayout = new QFormLayout(contentsWidget);
    contentsWidget->setLayout(fLayout);

    for(AbstractPlugin::SettingItem& item : *items)
    {
        QLabel *label = new QLabel(QString::fromStdString(item.name), contentsWidget);
        label->setToolTip(QString::fromStdString(item.detail));

        switch(item.variant.index())
        {
        case (size_t)AbstractPlugin::SettingItem::VariantType::Int:
        {
            QSpinBox *spinBox = new QSpinBox(contentsWidget);
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
            QDoubleSpinBox *spinBox = new QDoubleSpinBox(contentsWidget);
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
            QLineEdit *lineEdit = new QLineEdit(contentsWidget);
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
















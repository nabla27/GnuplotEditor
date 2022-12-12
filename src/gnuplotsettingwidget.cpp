/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "gnuplotsettingwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QFileDialog>
#include <QScrollBar>
#include <QLineEdit>
#include <QToolButton>
#include <QApplication>
#include "utility.h"
#include "texteditor.h"
#include "logger.h"

GnuplotSettingWidget::GnuplotSettingWidget(QWidget *parent)
    : QWidget(parent)
    , browser(new LogBrowserWidget(this))
    , pathEdit(new QLineEdit(this))
    , pathTool(new QToolButton(this))
    , initializeCmd(new TextEdit(this))
    , preCmd(new TextEdit(this))
    , settingFolderPath(QApplication::applicationDirPath() + "/setting")
    , settingFileName("gnuplot-setting.xml")
{
    initializeLayout();

    /* 設定の反映 */
    connect(pathEdit, &QLineEdit::editingFinished, this, &GnuplotSettingWidget::setGnuplotPath);
    connect(pathTool, &QToolButton::released, this, &GnuplotSettingWidget::selectGnuplotPath);
    connect(initializeCmd, &TextEdit::textChanged, this, &GnuplotSettingWidget::setGnuplotInitCmd);
    connect(preCmd, &TextEdit::textChanged, this, &GnuplotSettingWidget::setGnuplotPreCmd);

    browser->addFilter(Logger::LogLevel::GnuplotInfo);
    connect(logger, &Logger::logPushed, browser, &LogBrowserWidget::appendLog);
}

GnuplotSettingWidget::~GnuplotSettingWidget()
{
    saveXmlSetting();
}

void GnuplotSettingWidget::setGnuplotPath()
{
    emit exePathSet(pathEdit->text());
}

void GnuplotSettingWidget::setGnuplotInitCmd()
{
    emit initCmdSet(initializeCmd->toPlainText());
}

void GnuplotSettingWidget::setGnuplotPreCmd()
{
    emit preCmdSet(preCmd->toPlainText());
}

void GnuplotSettingWidget::initializeLayout()
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QHBoxLayout *pathLayout = new QHBoxLayout;
    QLabel *pathLabel = new QLabel("Path", this);
    QHBoxLayout *initCmdLayout = new QHBoxLayout;
    QLabel *initCmdLabel = new QLabel("Initialize Cmd", this);
    QHBoxLayout *preCmdLayout = new QHBoxLayout;
    QLabel *preCmdLabel = new QLabel("Pre Cmd", this);
    QHBoxLayout *autoCompileMsecLayout = new QHBoxLayout;
    QLabel *autoCompileMsecLabel = new QLabel("Auto compile", this);
    QSpinBox *autoCompileMsec = new QSpinBox(this);

    setLayout(vLayout);
    vLayout->addWidget(browser);
    vLayout->addLayout(pathLayout);
    pathLayout->addWidget(pathLabel);
    pathLayout->addWidget(pathEdit);
    pathLayout->addWidget(pathTool);
    vLayout->addLayout(initCmdLayout);
    initCmdLayout->addWidget(initCmdLabel);
    initCmdLayout->addWidget(initializeCmd);
    vLayout->addLayout(preCmdLayout);
    preCmdLayout->addWidget(preCmdLabel);
    preCmdLayout->addWidget(preCmd);
    vLayout->addLayout(autoCompileMsecLayout);
    autoCompileMsecLayout->addWidget(autoCompileMsecLabel);
    autoCompileMsecLayout->addWidget(autoCompileMsec);

    constexpr int label_width = 80;
    constexpr int editor_height = 80;
    pathLabel->setFixedWidth(label_width);
    pathTool->setText("...");
    initCmdLabel->setFixedWidth(label_width);
    initializeCmd->setFixedHeight(editor_height);
    preCmdLabel->setFixedWidth(label_width);
    preCmd->setFixedHeight(editor_height);
    autoCompileMsecLabel->setFixedWidth(label_width);

    pathTool->setText("...");
    autoCompileMsec->setMaximum(10000);

    pathLabel->setToolTip("Execution path of gnuplot.");
    initCmdLabel->setToolTip("Command to be executed in advance.\nThis will be kept event if you close the app.");
    preCmdLabel->setToolTip("Command to be executed in advance.\nThis will be removed if you close the app.");
    autoCompileMsecLabel->setToolTip("Auto compile msec time for sheet changes.");

    setGeometry(getRectFromScreenRatio(screen()->size(), 0.3f, 0.3f));
    browser->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(autoCompileMsec, &QSpinBox::valueChanged, this, &GnuplotSettingWidget::autoCompileMsecSet);
}

void GnuplotSettingWidget::selectGnuplotPath()
{
    const QString exePath = QFileDialog::getOpenFileName(this, "", "", "*.exe");

    if(exePath.isEmpty()) return;

    pathEdit->setText(exePath);
    setGnuplotPath();
}

void GnuplotSettingWidget::addLogToBrowser(const QString& text)
{
    if(text.isEmpty()) return;

    browser->append(text); //改行含む!!

    /* 一番下までスクロール */
    browser->verticalScrollBar()->setValue(browser->verticalScrollBar()->maximum());
}











/*          Copyright Joe Coder 2004 - 2006.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://www.boost.org/LICENSE_1_0.txt)
 */
#include "boost/property_tree/xml_parser.hpp"
#include "boost/lexical_cast.hpp"



void GnuplotSettingWidget::loadXmlSetting()
{
    using namespace boost::property_tree;

    if(QFile::exists(settingFolderPath + "/" + settingFileName))
    {
        logger->output(__FILE__, __LINE__, __FUNCTION__,
                       "load gnuplot setting xml file.", Logger::LogLevel::Info);

        ptree pt;
        read_xml((settingFolderPath + "/" + settingFileName).toUtf8().constData(), pt);

        if(boost::optional<std::string> path = pt.get_optional<std::string>("root.path"))
            pathEdit->setText(QString::fromStdString(path.value()));

        if(boost::optional<std::string> initCmd = pt.get_optional<std::string>("root.initCmd"))
            initializeCmd->insertPlainText(QString::fromStdString(initCmd.value()));

        setGnuplotPath();
        setGnuplotInitCmd();
    }
    else
    {
        logger->output(__FILE__, __LINE__, __FUNCTION__,
                       "gnuplot setting xml file was not found.", Logger::LogLevel::Warn);

        pathEdit->setText("gnuplot.exe");
        initializeCmd->insertPlainText("set datafile separator ','");
    }
}

void GnuplotSettingWidget::saveXmlSetting()
{
    using namespace boost::property_tree;

    ptree pt;

    pt.add("root.path", pathEdit->text().toUtf8().constData());
    pt.add("root.initCmd", initializeCmd->toPlainText().toUtf8().constData());

    //保存用のフォルダがなければ作成
    QDir dir(settingFolderPath);
    if(!dir.exists())
    {
        QDir dir;
        const bool success = dir.mkdir(settingFolderPath);
        if(!success)
        {
            logger->output(__FILE__, __LINE__, __FUNCTION__,
                           "failed to make dir " + settingFolderPath + ". "
                           "could not save the gnuplot setting.", Logger::LogLevel::Error);
            return;
        }
    }

    logger->output(__FILE__, __LINE__, __FUNCTION__,
                   "save gnuplot setting as xml file.", Logger::LogLevel::Info);

    //存在しないフォルダーを含むパスを指定した場合はクラッシュする
    //存在しないファイルは指定しても大丈夫
    constexpr int indent = 4;
    write_xml((settingFolderPath + "/" + settingFileName).toUtf8().constData(),
              pt, std::locale(), xml_writer_make_settings<std::string>(' ', indent, "utf-8"));
}

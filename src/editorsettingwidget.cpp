/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "editorsettingwidget.h"
#include <QFormLayout>
#include <QPushButton>
#include <QMessageBox>



EditorSetting::EditorSetting(QWidget *parent)
    : QWidget(parent)
    , backgroundColorDialog(new QColorDialog(this))
    , textColorDialog(new QColorDialog(this))
    , textFontDialog(new QFontDialog(this))
    , mainCmdColorDialog(new QColorDialog(this))
    , commentCmdColorDialog(new QColorDialog(this))
    , doubleQuoteColorDialog(new QColorDialog(this))
    , cursorLineColorDialog(new QColorDialog(this))
    , setDefaultButton(new QPushButton("Default", this))
{
    initializeLayout();

    loadXmlSetting();

    setMinimumWidth(250);
}

EditorSetting::~EditorSetting()
{
    saveXmlSetting();
}

void EditorSetting::initializeLayout()
{
    if(!layout()) delete layout();

    QFormLayout *fLayout = new QFormLayout(this);
    setLayout(fLayout);

    fLayout->addRow("Background color", backgroundColorButton = new QPushButton(this));
    fLayout->addRow("Text color", textColorButton = new QPushButton(this));
    fLayout->addRow("Text font", textFontButton = new QPushButton("sample", this));
    fLayout->addRow("Tab space", tabSpaceSpin = new QDoubleSpinBox(this));
    fLayout->addRow("Wrap line", wrapCheckBox = new QCheckBox(this));
    fLayout->addRow("Main cmd color", mainCmdColorButton = new QPushButton(this));
    fLayout->addRow("Comment cmd color", commentCmdColorButton = new QPushButton(this));
    fLayout->addRow("double quote color", doubleQuoteColorButton = new QPushButton(this));
    fLayout->addRow("Cursor line color", cursorLineColorButton = new QPushButton(this));
    fLayout->addRow(setDefaultButton);

    connect(backgroundColorButton, &QPushButton::released, backgroundColorDialog, &QColorDialog::show);
    connect(textColorButton, &QPushButton::released, textColorDialog, &QColorDialog::show);
    connect(textFontButton, &QPushButton::released, textFontDialog, &QFontDialog::show);
    connect(mainCmdColorButton, &QPushButton::released, mainCmdColorDialog, &QColorDialog::show);
    connect(commentCmdColorButton, &QPushButton::released, commentCmdColorDialog, &QColorDialog::show);
    connect(doubleQuoteColorButton, &QPushButton::released, doubleQuoteColorDialog, &QColorDialog::show);
    connect(cursorLineColorButton, &QPushButton::released, cursorLineColorDialog, &QColorDialog::show);

    connect(backgroundColorDialog, &QColorDialog::currentColorChanged, this, &EditorSetting::setBackgroundColor);
    connect(textColorDialog, &QColorDialog::currentColorChanged, this, &EditorSetting::setTextColor);
    connect(textFontDialog, &QFontDialog::currentFontChanged, this, &EditorSetting::setTextFont);
    connect(tabSpaceSpin, &QDoubleSpinBox::valueChanged, this, &EditorSetting::setTabSpace);
    connect(wrapCheckBox, &QCheckBox::toggled, this, &EditorSetting::setWrap);
    connect(mainCmdColorDialog, &QColorDialog::currentColorChanged, this, &EditorSetting::setMainCmdColor);
    connect(commentCmdColorDialog, &QColorDialog::currentColorChanged, this, &EditorSetting::setCommentCmdColor);
    connect(doubleQuoteColorDialog, &QColorDialog::currentColorChanged, this, &EditorSetting::setDoubleQuoteColor);
    connect(cursorLineColorDialog, &QColorDialog::currentColorChanged, this, &EditorSetting::setCursorLineColor);

    connect(setDefaultButton, &QPushButton::released, this, &EditorSetting::setDefaultSetting);
}

void EditorSetting::setDefaultSetting()
{
    const QMessageBox::StandardButton result = QMessageBox::question(this, "Reset", "Would you like to return to the default settings?");

    if(result == QMessageBox::Yes)
        loadDefaultSetting();
}

void EditorSetting::setBackgroundColor(const QColor& color)
{
    backgroundColorButton->setStyleSheet(QString("QPushButton { background: %1 }").arg(color.name()));
    if(editor)
        editor->setBackgroundColor(color);
}

void EditorSetting::setTextColor(const QColor& color)
{
    textColorButton->setStyleSheet(QString("QPushButton { background: %1 }").arg(color.name()));
    if(editor)
        editor->setTextColor(color);
}

void EditorSetting::setTextFont(const QFont& font)
{
    textFontButton->setFont(font);
    if(editor)
        editor->setFont(font);
}

void EditorSetting::setTabSpace(const double& tabSpace)
{
    if(editor)
        editor->setTabSpace(tabSpace);
}

void EditorSetting::setWrap(const bool wrap)
{
    if(editor)
        editor->setWrap(wrap);
}

void EditorSetting::setMainCmdColor(const QColor& color)
{
    mainCmdColorButton->setStyleSheet(QString("QPushButton { background: %1 }").arg(color.name()));

    if(editor)
        editor->setMainCmdColor(color);
}

void EditorSetting::setCommentCmdColor(const QColor& color)
{
    commentCmdColorButton->setStyleSheet(QString("QPushButton { background: %1 }").arg(color.name()));

    if(editor)
        editor->setCommentColor(color);
}

void EditorSetting::setDoubleQuoteColor(const QColor& color)
{
    doubleQuoteColorButton->setStyleSheet(QString("QPushButton { background: %1 }").arg(color.name()));

    if(editor)
        editor->setStringColor(color);
}

void EditorSetting::setCursorLineColor(const QColor& color)
{
    cursorLineColorButton->setStyleSheet(QString("QPushButton { background: %1 }").arg(color.name()));

    if(editor)
        editor->setCursorLineColor(color);
}

void EditorSetting::setTextSize(const int ps)
{
    QFont font = textFontDialog->currentFont();
    font.setPointSize(ps);
    textFontDialog->setCurrentFont(font);
}

void EditorSetting::setEditor(TextEdit *editor)
{
    if(!editor) return;

    this->editor = editor;

    editor->setBackgroundColor(backgroundColorDialog->currentColor());
    editor->setTextColor(textColorDialog->currentColor());
    editor->setFont(textFontDialog->currentFont());
    editor->setTabSpace(tabSpaceSpin->value());
    editor->setWrap(wrapCheckBox->isChecked());
    editor->setMainCmdColor(mainCmdColorDialog->currentColor());
    editor->setCommentColor(commentCmdColorDialog->currentColor());
    editor->setStringColor(doubleQuoteColorDialog->currentColor());
    editor->setCursorLineColor(cursorLineColorDialog->currentColor());
}









/*          Copyright Joe Coder 2004 - 2006.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://www.boost.org/LICENSE_1_0.txt)
 */
#include "boost/property_tree/xml_parser.hpp"
#include "boost/lexical_cast.hpp"




/* dialogを設定した後、コンストラクタ内でつなげたシグナルスロットによって、ボタンのスタイルも変更されるが、
 * dialogに設定した値がdialogのデフォルト値と等しい場合、シグナルは発せられないので、それぞれ明示的に
 * set~()を呼び出してある。*/
void EditorSetting::loadXmlSetting()
{
    using namespace boost::property_tree;

    const QString settingFile = settingFolderPath + '/' + settingFileName;

    if(QFile::exists(settingFile))
    {
        ptree pt;
        read_xml(settingFile.toUtf8().constData(), pt); //存在しないファイルやフォルダーを指定するとエラー(落ちる)

        {
            int r = 0, g = 0, b = 0;
            if(boost::optional<int> red = pt.get_optional<int>("root.backgroundColor.r"))
                r = red.value();
            if(boost::optional<int> green = pt.get_optional<int>("root.backgroundColor.g"))
                g = green.value();
            if(boost::optional<int> blue = pt.get_optional<int>("root.backgroundColor.b"))
                b = blue.value();

            backgroundColorDialog->setCurrentColor(QColor(r, g, b));
            setBackgroundColor(QColor(r, g, b));
        }
        {
            int r = 0, g = 0, b = 0;
            if(boost::optional<int> red = pt.get_optional<int>("root.textColor.r"))
                r = red.value();
            if(boost::optional<int> green = pt.get_optional<int>("root.textColor.g"))
                g = green.value();
            if(boost::optional<int> blue = pt.get_optional<int>("root.textColor.b"))
                b = blue.value();

            textColorDialog->setCurrentColor(QColor(r, g, b));
            setTextColor(QColor(r, g, b));
        }
        {
            QString family = "";
            int pointSize = 0;
            QFont::Weight weight = QFont::Weight::Normal;
            bool itaric = false;
            if(boost::optional<std::string> f = pt.get_optional<std::string>("root.textFont.family"))
                family = QString::fromStdString(f.value());
            if(boost::optional<int> p = pt.get_optional<int>("root.textFont.pointSize"))
                pointSize = p.value();
            if(boost::optional<int> w = pt.get_optional<int>("root.textFont.weight"))
                weight = QFont::Weight(w.value());
            if(boost::optional<bool> i = pt.get_optional<bool>("root.textFont.italic"))
                itaric = i.value();

            const QFont font(family, pointSize, weight, itaric);
            textFontDialog->setCurrentFont(font);
            setTextFont(font);
        }
        {
            if(boost::optional<double> s = pt.get_optional<double>("root.tabSpace"))
                tabSpaceSpin->setValue(s.value());
        }
        {
            if(boost::optional<bool> wrap = pt.get_optional<bool>("root.wrap"))
                wrapCheckBox->setChecked(wrap.value());
        }
        {
            int r = 0, g = 0, b = 0;
            if(boost::optional<int> red = pt.get_optional<int>("root.mainCmdColor.r"))
                r = red.value();
            if(boost::optional<int> green = pt.get_optional<int>("root.mainCmdColor.g"))
                g = green.value();
            if(boost::optional<int> blue = pt.get_optional<int>("root.mainCmdColor.b"))
                b = blue.value();

            mainCmdColorDialog->setCurrentColor(QColor(r, g, b));
            setMainCmdColor(QColor(r, g, b));
        }
        {
            int r = 0, g = 0, b = 0;
            if(boost::optional<int> red = pt.get_optional<int>("root.commentCmdColor.r"))
                r = red.value();
            if(boost::optional<int> green = pt.get_optional<int>("root.commentCmdColor.g"))
                g = green.value();
            if(boost::optional<int> blue = pt.get_optional<int>("root.commentCmdColor.b"))
                b = blue.value();

            commentCmdColorDialog->setCurrentColor(QColor(r, g, b));
            setCommentCmdColor(QColor(r, g, b));
        }
        {
            int r = 0, g = 0, b = 0;
            if(boost::optional<int> red = pt.get_optional<int>("root.doubleQuoteColor.r"))
                r = red.value();
            if(boost::optional<int> green = pt.get_optional<int>("root.doubleQuoteColor.g"))
                g = green.value();
            if(boost::optional<int> blue = pt.get_optional<int>("root.doubleQuoteColor.b"))
                b = blue.value();

            doubleQuoteColorDialog->setCurrentColor(QColor(r, g, b));
            setDoubleQuoteColor(QColor(r, g, b));
        }
        {
            int r = 0, g = 0, b = 0;
            if(boost::optional<int> red = pt.get_optional<int>("root.cursorLineColor.r"))
                r = red.value();
            if(boost::optional<int> green = pt.get_optional<int>("root.cursorLineColor.g"))
                g = green.value();
            if(boost::optional<int> blue = pt.get_optional<int>("root.cursorLineColor.b"))
                b = blue.value();

            cursorLineColorDialog->setCurrentColor(QColor(r, g, b));
            setCursorLineColor(QColor(r, g, b));
        }
    }
    else
    {
        loadDefaultSetting();
    }
}

void EditorSetting::loadDefaultSetting()
{
    backgroundColorDialog->setCurrentColor(Qt::GlobalColor::black);
    setBackgroundColor(Qt::GlobalColor::black);

    textColorDialog->setCurrentColor(Qt::GlobalColor::white);
    setTextColor(Qt::GlobalColor::white);

    const QFont font("Yu Gothic UI", 14, QFont::Normal, false);
    textFontDialog->setCurrentFont(font);
    setTextFont(font);

    tabSpaceSpin->setValue(20.00);

    wrapCheckBox->setChecked(true);

    mainCmdColorDialog->setCurrentColor(Qt::GlobalColor::cyan);
    setMainCmdColor(Qt::GlobalColor::cyan);

    commentCmdColorDialog->setCurrentColor(Qt::GlobalColor::lightGray);
    setCommentCmdColor(Qt::GlobalColor::lightGray);

    doubleQuoteColorDialog->setCurrentColor(QColor(255, 200, 0));
    setDoubleQuoteColor(QColor(255, 200, 0));

    cursorLineColorDialog->setCurrentColor(QColor(50, 50, 50));
    setCursorLineColor(QColor(50, 50, 50));
}

void EditorSetting::saveXmlSetting()
{
    using namespace boost::property_tree;

    ptree pt;

    ptree& backgroundColor = pt.add("root.backgroundColor", "");
    backgroundColor.add("r", backgroundColorDialog->currentColor().red());
    backgroundColor.add("g", backgroundColorDialog->currentColor().green());
    backgroundColor.add("b", backgroundColorDialog->currentColor().blue());

    ptree& textColor = pt.add("root.textColor", "");
    textColor.add("r", textColorDialog->currentColor().red());
    textColor.add("g", textColorDialog->currentColor().green());
    textColor.add("b", textColorDialog->currentColor().blue());

    ptree& textFont = pt.add("root.textFont", "");
    textFont.add("family", textFontDialog->currentFont().family().toStdString());
    textFont.add("pointSize", textFontDialog->currentFont().pointSize());
    textFont.add("weight", (int)textFontDialog->currentFont().weight());
    textFont.add("italic", textFontDialog->currentFont().italic());
    pt.add("root.tabSpace", tabSpaceSpin->value());

    pt.add("root.wrap", wrapCheckBox->isChecked());

    ptree& mainCmdColor = pt.add("root.mainCmdColor", "");
    mainCmdColor.add("r", mainCmdColorDialog->currentColor().red());
    mainCmdColor.add("g", mainCmdColorDialog->currentColor().green());
    mainCmdColor.add("b", mainCmdColorDialog->currentColor().blue());

    ptree& commentCmdColor = pt.add("root.commentCmdColor", "");
    commentCmdColor.add("r", commentCmdColorDialog->currentColor().red());
    commentCmdColor.add("g", commentCmdColorDialog->currentColor().green());
    commentCmdColor.add("b", commentCmdColorDialog->currentColor().blue());

    ptree& doubleQuoteColor = pt.add("root.doubleQuoteColor", "");
    doubleQuoteColor.add("r", doubleQuoteColorDialog->currentColor().red());
    doubleQuoteColor.add("g", doubleQuoteColorDialog->currentColor().green());
    doubleQuoteColor.add("b", doubleQuoteColorDialog->currentColor().blue());

    ptree& cursorLineColor = pt.add("root.cursorLineColor", "");
    cursorLineColor.add("r", cursorLineColorDialog->currentColor().red());
    cursorLineColor.add("g", cursorLineColorDialog->currentColor().green());
    cursorLineColor.add("b", cursorLineColorDialog->currentColor().blue());


    const int indent = 4;

    //保存用のフォルダが無ければ作成
    QDir dir(settingFolderPath);
    if(!dir.exists()) {
        QDir dir;
        const bool success = dir.mkdir(settingFolderPath);
        if(!success) {
            emit errorCaused("failed to create the folder \"" + settingFolderPath + "\", so it could not save settings.", BrowserWidget::MessageType::FileSystemErr);
            return;
        }
    }

    //存在しないフォルダーを含むパスを指定した場合はクラッシュする
    //存在しないファイルは指定しても大丈夫
    write_xml((settingFolderPath + "/" + settingFileName).toUtf8().constData()
              , pt, std::locale(), xml_writer_make_settings<std::string>(' ', indent, "utf-8"));
}





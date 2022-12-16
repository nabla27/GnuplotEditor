/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef EDITORSETTINGWIDGET_H
#define EDITORSETTINGWIDGET_H

#include <QWidget>
#include <QFile>
#include <QDir>
#include <QColorDialog>
#include <QFontDialog>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include "texteditor.h"
#include <QApplication>

class EditorSetting : public QWidget
{
    Q_OBJECT
public:
    explicit EditorSetting(QWidget *parent = nullptr);
    ~EditorSetting();

    void loadXmlSetting();
    void saveXmlSetting();
    void addEditor(TextEdit *const editor);

private slots:
    void setTextSize(const int ps);
    void setBackgroundColor(const QColor& color);
    void setTextColor(const QColor& color);
    void setTextFont(const QFont& font);
    //void setTabSpace(const double& tabSpace);
    //void setWrap(const bool wrap);
    void setMainCmdColor(const QColor& color);
    void setCommentCmdColor(const QColor& color);
    void setDoubleQuoteColor(const QColor& color);
    void setCursorLineColor(const QColor& color);

    void setDefaultSetting();

private:
    void initializeLayout();
    void closeEvent(QCloseEvent* event) override { saveXmlSetting(); QWidget::closeEvent(event); }
    void loadDefaultSetting();

    const QString settingFolderPath = QApplication::applicationDirPath() + "/setting";
    const QString settingFileName = "editor-setting.xml";

private:
    QColorDialog *backgroundColorDialog;
    QPushButton *backgroundColorButton;
    QColorDialog *textColorDialog;
    QPushButton *textColorButton;
    QFontDialog *textFontDialog;
    QPushButton *textFontButton;
    QDoubleSpinBox *tabSpaceSpin;
    QCheckBox *wrapCheckBox;
    QColorDialog *mainCmdColorDialog;
    QPushButton *mainCmdColorButton;
    QColorDialog *commentCmdColorDialog;
    QPushButton *commentCmdColorButton;
    QColorDialog *doubleQuoteColorDialog;
    QPushButton *doubleQuoteColorButton;
    QColorDialog *cursorLineColorDialog;
    QPushButton *cursorLineColorButton;

    QPushButton *setDefaultButton;

    QSet<TextEdit*> editorList;
};

#endif // EDITORSETTINGWIDGET_H

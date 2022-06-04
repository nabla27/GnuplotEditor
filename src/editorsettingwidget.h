#ifndef EDITORSETTINGWIDGET_H
#define EDITORSETTINGWIDGET_H

#include <QWidget>
#include <QFile>
#include <QDir>
#include <QColorDialog>
#include <QFontDialog>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include "browserwidget.h"
#include "texteditor.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <QApplication>

class EditorSetting : public QWidget
{
    Q_OBJECT
public:
    explicit EditorSetting(QWidget *parent = nullptr);
    ~EditorSetting();

    void loadXmlSetting();
    void saveXmlSetting();
    void setTextSize(const int ps);
    void setEditor(TextEdit *const editor);

private slots:
    void setBackgroundColor(const QColor& color);
    void setTextColor(const QColor& color);
    void setTextFont(const QFont& font);
    void setTabSpace(const double& tabSpace);
    void setWrap(const bool wrap);
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

    TextEdit *editor = nullptr;

signals:
    void errorCaused(const QString& text, const BrowserWidget::MessageType& type);
};

#endif // EDITORSETTINGWIDGET_H

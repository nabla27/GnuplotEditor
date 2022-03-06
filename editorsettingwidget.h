#ifndef EDITORSETTINGWIDGET_H
#define EDITORSETTINGWIDGET_H

#include <QWidget>
#include <QFile>
#include <QDir>
#include "layoutparts.h"
#include "utility.h"
#include "browserwidget.h"
#include "texteditor.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>

class EditorSettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditorSettingWidget(QWidget *parent = nullptr);
    ~EditorSettingWidget();
    void loadXmlSetting();
    void saveXmlSetting();
    void set(TextEdit *const editor);

private:
    void initializeLayout();
    void showEvent(QShowEvent*) override { loadXmlSetting(); }
    void closeEvent(QCloseEvent*) override { saveXmlSetting(); }
    void loadDefaultSetting();
    static const QString settingFolderPath;

private:
    ComboEditLayout *backgroundColorCombo;
    RGBEditLayout *backgroundColorRgb;
    ComboEditLayout *textColorCombo;
    RGBEditLayout *textColorRgb;
    SpinBoxEditLayout *textSizeSpin;
    LineEditLayout *tabSpaceEdit;
    CheckBoxLayout *checkWrap;
    CheckBoxLayout *checkItaric;
    CheckBoxLayout *checkBold;
    ComboEditLayout *mainCmdColorCombo;
    RGBEditLayout *mainCmdColorRgb;
    ComboEditLayout *commentColorCombo;
    RGBEditLayout *commentColorRgb;
    ComboEditLayout *cursorLineColorCombo;
    RGBEditLayout *cursorLineColorRgb;
    ComboEditLayout *stringColorCombo;
    RGBEditLayout *stringColorRgb;

signals:
    void backgroundColorSet(const QColor& color);
    void textColorSet(const QColor& color);
    void textSizeSet(const int ps);
    void tabSpaceSet(const double& space);
    void checkWrapSet(const bool wrap);
    void checkItaricSet(const bool iraric);
    void checkBoldSet(const bool bold);
    void mainCmdColorSet(const QColor& color);
    void commentColorSet(const QColor& color);
    void cursorLineColorSet(const QColor& color);
    void stringColorSet(const QColor& color);
    void errorCaused(const QString& text, const BrowserWidget::MessageType type);
};

#endif // EDITORSETTINGWIDGET_H

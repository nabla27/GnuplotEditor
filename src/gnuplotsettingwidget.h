/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef GNUPLOTSETTINGWIDGET_H
#define GNUPLOTSETTINGWIDGET_H

#include <QWidget>
#include <QTextBrowser>
#include <QLineEdit>
#include <QToolButton>
#include <QApplication>
#include "gnuplot.h"
#include "texteditor.h"

class GnuplotSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GnuplotSettingWidget(QWidget *parent);
    ~GnuplotSettingWidget();
    void loadXmlSetting();

public slots:
    void addLogToBrowser(const QString& text);

private slots:
    void selectGnuplotPath();
    void setGnuplotPath() { emit exePathSet(pathEdit->text()); }
    void setGnuplotInitCmd() { emit initCmdSet(initializeCmd->toPlainText()); }
    void setGnuplotPreCmd() { emit preCmdSet(preCmd->toPlainText()); }

private:
    void initializeLayout();
    void saveXmlSetting();

private:
    QTextBrowser *browser;
    QLineEdit *pathEdit;
    QToolButton *pathTool;
    TextEdit *initializeCmd;
    TextEdit *preCmd;

    const QString settingFolderPath = QApplication::applicationDirPath() + "/setting";
    const QString settingFileName = "gnuplot-setting.xml";

signals:
    void autoCompileMsecSet(const int msec);
    void exePathSet(const QString& path);
    void initCmdSet(const QString& initCmd);
    void preCmdSet(const QString& preCmd);
};

#endif // GNUPLOTSETTINGWIDGET_H

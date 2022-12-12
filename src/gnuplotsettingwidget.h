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


class LogBrowserWidget;
class QLineEdit;
class QToolButton;
class TextEdit;



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
    void setGnuplotPath();
    void setGnuplotInitCmd();
    void setGnuplotPreCmd();

private:
    void initializeLayout();
    void saveXmlSetting();

private:
    LogBrowserWidget *browser;
    QLineEdit *pathEdit;
    QToolButton *pathTool;
    TextEdit *initializeCmd;
    TextEdit *preCmd;

    const QString settingFolderPath;
    const QString settingFileName;

signals:
    void autoCompileMsecSet(const int msec);
    void exePathSet(const QString& path);
    void initCmdSet(const QString& initCmd);
    void preCmdSet(const QString& preCmd);
};

#endif // GNUPLOTSETTINGWIDGET_H

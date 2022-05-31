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
    explicit GnuplotSettingWidget(Gnuplot *gnuplot, QWidget *parent);
    ~GnuplotSettingWidget();

private slots:
    void addLogToBrowser(const QString& text);
    void selectGnuplotPath();
    void setGnuplotPath() { gnuplot->setExePath(pathEdit->text()); }
    void setGnuplotInitCmd() { gnuplot->setInitCmd(initializeCmd->toPlainText()); }
    void setGnuplotPreCmd() { gnuplot->setPreCmd(preCmd->toPlainText()); };

private:
    void initializeLayout();
    void loadXmlSetting();
    void saveXmlSetting();

private:
    Gnuplot *gnuplot;
    QTextBrowser *browser;
    QLineEdit *pathEdit;
    QToolButton *pathTool;
    TextEdit *initializeCmd;
    TextEdit *preCmd;

    const QString settingFolderPath = QApplication::applicationDirPath() + "/setting";
    const QString settingFileName = "gnuplot-setting.xml";

signals:
    void autoCompileMsecSet(const int msec);
};

#endif // GNUPLOTSETTINGWIDGET_H

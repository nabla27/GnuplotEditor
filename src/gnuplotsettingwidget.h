#ifndef GNUPLOTSETTINGWIDGET_H
#define GNUPLOTSETTINGWIDGET_H

#include <QWidget>
#include <QTextBrowser>
#include <QLineEdit>
#include <QToolButton>
#include "gnuplot.h"
#include "texteditor.h"

class GnuplotSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GnuplotSettingWidget(Gnuplot *gnuplot, QWidget *parent = nullptr);
    ~GnuplotSettingWidget();

private slots:
    void addLogToBrowser(const QString& text);
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

    const QString settingFolderPath = "./setting/";
    const QString settingFileName = "gnuplot-setting.xml";

signals:

};

#endif // GNUPLOTSETTINGWIDGET_H

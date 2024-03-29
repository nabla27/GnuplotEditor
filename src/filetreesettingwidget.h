/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef FILETREESETTINGWIDGET_H
#define FILETREESETTINGWIDGET_H
#include <QWidget>
#include <QApplication>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include "filetreewidget.h"

/* フィルター
 * 拡張子
 *
 */

class FileTreeSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileTreeSettingWidget(QWidget *parent = nullptr);
    ~FileTreeSettingWidget();

    QString previousFolderPath() const { return _previousFolderPath; }

public slots:
    void setPreviousFolderPath(const QString& path) { _previousFolderPath = path; }

private:
    void setupLayout();
    void loadXmlSetting();
    void saveXmlSetting();

private slots:
    void addItem();
    void removeItem();
    void editItem();
    void addScriptExt(const QString& ext, const int index);
    void addSheetExt(const QString& ext, const int index);

    void requestReload();

private:
    const QString settingFolderPath = QApplication::applicationDirPath() + "/setting";
    const QString settingFileName = "filetree-setting.xml";
    QString _previousFolderPath = "";

    QStackedWidget *settingPage;
    QTreeWidget *filterList;
    QTreeWidget *scriptExtensionList;
    QTreeWidget *sheetExtensionList;

signals:
    void reloadRequested();
};




class InputDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InputDialog(QWidget *parent, const QString& lineEditLabel, const QString& comboLabel);

public:
    void addComboItems(const QStringList& list) { if(combo) combo->addItems(list); }
    QString lineEditText() const { return (lineEdit) ? lineEdit->text() : ""; }
    int comboIndex() const { return (combo) ? combo->currentIndex() : -1; }
    void setData(const QString& lineEdit, const int index);

private:
    QLineEdit *lineEdit;
    QComboBox *combo;
};

#endif // FILETREESETTINGWIDGET_H

#ifndef FILETREESETTINGWIDGET_H
#define FILETREESETTINGWIDGET_H
#include <QWidget>
#include <QApplication>

/* フィルター
 * 拡張子
 *
 */

class FileTreeSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileTreeSettingWidget(QWidget *parent = nullptr);

private:
    void setupLayout();
    void loadXmlSetting();
    void saveXmlSetting();
    void closeEvent(QCloseEvent *event) override { saveXmlSetting(); QWidget::closeEvent(event); }

private:
    const QString settingFolderPath = QApplication::applicationDirPath() + "/setting";
    const QString settingFileName = "filetree-setting.xml";

signals:

};

#endif // FILETREESETTINGWIDGET_H

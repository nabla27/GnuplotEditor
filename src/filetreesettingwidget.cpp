#include "filetreesettingwidget.h"
#include "boost/property_tree/xml_parser.hpp"
#include "boost/lexical_cast.hpp"
#include <QFile>
#include <QDir>

FileTreeSettingWidget::FileTreeSettingWidget(QWidget *parent)
    : QWidget{parent}
{

}

void FileTreeSettingWidget::setupLayout()
{

}

void FileTreeSettingWidget::loadXmlSetting()
{
    using namespace boost::property_tree;

    const QString settingFile = settingFolderPath + '/' + settingFileName;

    if(QFile::exists(settingFile))
    {
        ptree pt;
        read_xml(settingFile.toUtf8().constData(), pt); //存在しないファイルやフォルダーを指定するとエラー(落ちる)

    }
}

void FileTreeSettingWidget::saveXmlSetting()
{
    using namespace boost::property_tree;

    ptree pt;



    const int indent = 4;

    //保存用のフォルダがなければ作成
    QDir dir(settingFolderPath);
    if(!dir.exists())
    {
        QDir dir;
        const bool success = dir.mkdir(settingFolderPath);
        if(!success)
        {
            /* エラー処理を追加する */ qDebug() << __FILE__ << __LINE__;
            return;
        }
    }

    //存在しないフォルダーを含むパスを指定した場合はクラッシュする
    //存在しないファイルは指定しても大丈夫
    write_xml((settingFolderPath + "/" + settingFileName).toUtf8().constData(),
              pt, std::locale(), xml_writer_make_settings<std::string>(' ', indent, "utf-8"));
}

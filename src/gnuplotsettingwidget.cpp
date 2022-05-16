#include "gnuplotsettingwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include "utility.h"
#include "boost/property_tree/xml_parser.hpp"
#include "boost/lexical_cast.hpp"

GnuplotSettingWidget::GnuplotSettingWidget(Gnuplot *gnuplot, QWidget *parent)
    : QWidget(parent)
    , gnuplot(gnuplot)
    , browser(new QTextBrowser(this))
    , pathEdit(new QLineEdit(this))
    , pathTool(new QToolButton(this))
    , initializeCmd(new TextEdit(this))
    , preCmd(new TextEdit(this))
{
    connect(gnuplot, &Gnuplot::cmdPushed, this, &GnuplotSettingWidget::addLogToBrowser);

    initializeLayout();

    /* 設定の反映 */
    connect(pathEdit, &QLineEdit::editingFinished, this, &GnuplotSettingWidget::setGnuplotPath);
    connect(initializeCmd, &TextEdit::textChanged, this, &GnuplotSettingWidget::setGnuplotInitCmd);
    connect(preCmd, &TextEdit::textChanged, this, &GnuplotSettingWidget::setGnuplotPreCmd);

    /* ファイルから設定の読み込み */
    loadXmlSetting();
}

GnuplotSettingWidget::~GnuplotSettingWidget()
{
    saveXmlSetting();
}

void GnuplotSettingWidget::initializeLayout()
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QHBoxLayout *pathLayout = new QHBoxLayout;
    QLabel *pathLabel = new QLabel("Path", this);
    QHBoxLayout *initCmdLayout = new QHBoxLayout;
    QLabel *initCmdLabel = new QLabel("Initialize Cmd", this);
    QHBoxLayout *preCmdLayout = new QHBoxLayout;
    QLabel *preCmdLabel = new QLabel("Pre Cmd", this);
    QHBoxLayout *autoCompileMsecLayout = new QHBoxLayout;
    QLabel *autoCompileMsecLabel = new QLabel("Auto compile sec", this);
    QSpinBox *autoCompileMsec = new QSpinBox(this);
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    setLayout(vLayout);
    vLayout->addWidget(browser);
    vLayout->addLayout(pathLayout);
    pathLayout->addWidget(pathLabel);
    pathLayout->addWidget(pathEdit);
    pathLayout->addWidget(pathTool);
    vLayout->addLayout(initCmdLayout);
    initCmdLayout->addWidget(initCmdLabel);
    initCmdLayout->addWidget(initializeCmd);
    vLayout->addLayout(preCmdLayout);
    preCmdLayout->addWidget(preCmdLabel);
    preCmdLayout->addWidget(preCmd);
    vLayout->addLayout(autoCompileMsecLayout);
    autoCompileMsecLayout->addWidget(autoCompileMsecLabel);
    autoCompileMsecLayout->addWidget(autoCompileMsec);
    vLayout->addItem(spacer);

    constexpr int label_width = 80;
    constexpr int editor_height = 80;
    browser->setFixedHeight(editor_height);
    pathLabel->setFixedWidth(label_width);
    initCmdLabel->setFixedWidth(label_width);
    initializeCmd->setFixedHeight(editor_height);
    preCmdLabel->setFixedWidth(label_width);
    preCmd->setFixedHeight(editor_height);
    autoCompileMsecLabel->setFixedWidth(label_width);
    autoCompileMsec->setMaximum(10000);

    setGeometry(getRectFromScreenRatio(screen()->size(), 0.3f, 0.3f));
    setFixedHeight(editor_height * 3 +
                   vLayout->spacing() * 2 +
                   vLayout->contentsMargins().bottom() +
                   vLayout->contentsMargins().top() +
                   pathEdit->height() +
                   autoCompileMsec->height());

    connect(autoCompileMsec, &QSpinBox::valueChanged, this, &GnuplotSettingWidget::autoCompileMsecSet);
}

void GnuplotSettingWidget::addLogToBrowser(const QString& text)
{
    if(text.isEmpty()) return;

    browser->append(text); //改行含む!!

    /* 一番下までスクロール */
    browser->verticalScrollBar()->setValue(browser->verticalScrollBar()->maximum());
}

void GnuplotSettingWidget::loadXmlSetting()
{
    using namespace boost::property_tree;

    if(QFile::exists(settingFolderPath + "/" + settingFileName))
    {
        ptree pt;
        read_xml((settingFolderPath + "/" + settingFileName).toUtf8().constData(), pt);

        if(boost::optional<std::string> path = pt.get_optional<std::string>("root.path"))
            pathEdit->setText(QString::fromStdString(path.value()));

        if(boost::optional<std::string> initCmd = pt.get_optional<std::string>("root.initCmd"))
            initializeCmd->insertPlainText(QString::fromStdString(initCmd.value()));
    }
    else
    {
        pathEdit->setText("gnuplot.exe");
        initializeCmd->insertPlainText("set datafile separator ','");
    }
}

void GnuplotSettingWidget::saveXmlSetting()
{
    using namespace boost::property_tree;

    ptree pt;

    pt.add("root.path", pathEdit->text().toUtf8().constData());
    pt.add("root.initCmd", initializeCmd->toPlainText().toUtf8().constData());

    //保存用のフォルダがなければ作成
    QDir dir(settingFolderPath);
    if(!dir.exists())
    {
        QDir dir;
        const bool success = dir.mkdir(settingFolderPath);
        if(!success)
        {
            //エラー処理
            return;
        }
    }

    //存在しないフォルダーを含むパスを指定した場合はクラッシュする
    //存在しないファイルは指定しても大丈夫
    constexpr int indent = 4;
    write_xml((settingFolderPath + "/" + settingFileName).toUtf8().constData(),
              pt, std::locale(), xml_writer_make_settings<std::string>(' ', indent, "utf-8"));
}

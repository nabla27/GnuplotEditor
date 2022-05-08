#include "updatemanager.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QDialogButtonBox>

#include <QFileDialog>

#include "utility.h"
#include "boost/property_tree/xml_parser.hpp"
#include "boost/lexical_cast.hpp"

const QString UpdateManager::defaultFileName = "pltEditor.zip";
const QString UpdateManager::downloadUrl = "https://github.com/nabla27/GnuplotEditor/archive/refs/heads/master.zip";
const QString UpdateManager::unzipName = "GnuplotEditor-develop"; //DEBUG


ProgressDialog::ProgressDialog(const QUrl& url, QWidget *parent)
    : QProgressDialog(parent)
{
    setWindowTitle("Download Progress");
    setLabelText(QString("Downloading %1.").arg(url.toDisplayString()));
    setMinimum(0);
    setValue(0);
    setMinimumDuration(0);
    setMinimumSize(QSize(400, 76));
}

void ProgressDialog::replyProgress(qint64 bytesRead, qint64 totalBytes)
{
    if(totalBytes == -1)
        setMaximum(20000000);
    else
        setMaximum(totalBytes);

    setValue(bytesRead);
}






Unzip::Unzip(QObject *parent)
    : QObject(parent)
    , unzipExePath("unzip.exe")
    , process(new QProcess(this))
{
    /* unzipの実行ファイルのパスを取得 */
    QString dir = QDir::currentPath();
    for(;;)
    {
        if(QDir(dir).entryList().contains("unzip.exe"))
        {
            unzipExePath = dir + '/' + "unzip.exe";
            break;
        }

        const qsizetype n = dir.lastIndexOf('/');

        if(n == qsizetype(-1))
            break;
        else
            dir = dir.first(n);
    }

    connect(process, &QProcess::readyReadStandardOutput, this, &Unzip::receiveStdOut);
    connect(process, &QProcess::readyReadStandardError, this, &Unzip::receiveStdErr);
    connect(process, &QProcess::finished, this, &Unzip::finished);
}

void Unzip::unzip(const QString& zipName, const QString& dir)
{
    if(!QFile::exists(unzipExePath))
        emit stdErrReceived("Could not find the unzip.exe");

    process->start(unzipExePath,
                   QStringList() << zipName
                                 << "-d"
                                 << dir);

}

void Unzip::receiveStdOut()
{
    emit stdOutReceived(QString::fromLatin1(process->readAllStandardOutput()));
}

void Unzip::receiveStdErr()
{
    emit stdErrReceived(QString::fromLatin1(process->readAllStandardError()));
}








UpdateManager::UpdateManager(QWidget *parent)
    : QDialog(parent)
    , urlLineEdit(new QLineEdit(downloadUrl, this))
    , directoryLineEdit(new QLineEdit("", this))
    , versionLineEdit(new QLineEdit("", this))
    , messageLabel(new QLabel(this))
    , updateButton(new QPushButton("Update", this))
    , quitButton(new QPushButton("Quit", this))
{
    setMinimumWidth(500);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QHBoxLayout *urlLayout = new QHBoxLayout;
    QHBoxLayout *directoryLayout = new QHBoxLayout;
    QHBoxLayout *versionLayout = new QHBoxLayout;
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    setLayout(vLayout);
    vLayout->addLayout(urlLayout);
    vLayout->addLayout(directoryLayout);
    vLayout->addLayout(versionLayout);
    vLayout->addWidget(messageLabel);
    vLayout->addItem(spacer);

    QLabel *urlLabel = new QLabel("URL", this);
    QLabel *directoryLabel = new QLabel("Directory", this);
    QToolButton *selectDirButton = new QToolButton(this);
    QLabel *versionLabel = new QLabel("Version", this);
    urlLayout->addWidget(urlLabel);
    urlLayout->addWidget(urlLineEdit);
    directoryLayout->addWidget(directoryLabel);
    directoryLayout->addWidget(directoryLineEdit);
    directoryLayout->addWidget(selectDirButton);
    versionLayout->addWidget(versionLabel);
    versionLayout->addWidget(versionLineEdit);
    connect(selectDirButton, &QToolButton::released, this, &UpdateManager::selectDirectory);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    vLayout->addWidget(buttonBox);
    buttonBox->addButton(updateButton, QDialogButtonBox::ButtonRole::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::ButtonRole::RejectRole);
    connect(updateButton, &QPushButton::released, this, &UpdateManager::requestUpdate);
    connect(quitButton, &QPushButton::released, this, &UpdateManager::close);

    const int label_width = 60;
    urlLabel->setFixedWidth(label_width);
    directoryLabel->setFixedWidth(label_width);
    versionLabel->setFixedWidth(label_width);

    //urlLineEdit->setReadOnly(true);  //DEBUG
    versionLineEdit->setReadOnly(true);
    selectDirButton->setText("...");

    /* カレントディレクトリからさかのぼって、binフォルダのあるパス(=アプリケーションのパス)を特定する */
    const QString currentDirectory = QDir::currentPath();
    const qsizetype sp = currentDirectory.count('/');
    for(qsizetype i = 0; i < sp; ++i)
    {
        const QString dir = currentDirectory.section('/', 0, -i); //'/'でセクションに区切る。一番左0から右からのセクションi番目を取り出す
        if(QDir(dir).entryList().contains("bin"))
        {
            oldFolderPath = dir;
            break;
        }
    }

    directoryLineEdit->setText(oldFolderPath.first(oldFolderPath.lastIndexOf('/')));

    getVersion();
}

UpdateManager::~UpdateManager()
{

}

void UpdateManager::getVersion()
{
    using namespace boost::property_tree;

    const QString versionFile = oldFolderPath + "/bin/release/setting/version.xml";

    QString oldVersion;
    QString newVersion;
    QString oldDate;
    QString newDate;

    if(QFile::exists(versionFile))
    {
        ptree pt;
        read_xml(versionFile.toUtf8().constData(), pt);

        if(boost::optional<std::string> version = pt.get_optional<std::string>("root.version"))
            oldVersion = QString::fromStdString(version.value());
        if(boost::optional<std::string> date = pt.get_optional<std::string>("root.date"))
            oldDate = QString::fromStdString(date.value());
    }
}

void UpdateManager::requestUpdate()
{
    /* フォームに入力されたURL */
    const QString urlText = urlLineEdit->text().trimmed();
    if(urlText.isEmpty())
    {
        outErrorMessage("Invalid URL.");
        return;
    }

    /* URL ダウンロードリンク */
    const QUrl url = QUrl::fromUserInput(urlText);
    if(!url.isValid())
    {
        outErrorMessage("Invalid URL.");
        return;
    }

    /* ダウンロードリンク先のzipファイルのファイル名 */
    QString fileName = url.fileName();
    if(fileName.isEmpty()) fileName = defaultFileName;

    /* ダウンロードするディレクトリとダウンロードするファイル名 */
    QDir dlDirectory(directoryLineEdit->text().trimmed());
    if(dlDirectory.exists())
    {
        /* 選択したディレクトリに展開後と同じフォルダー名があれば無効 */
        if(QDir(dlDirectory.path() + '/' + unzipName).exists())
        {
            outErrorMessage("Already exists : " + dlDirectory.path() + '/' + unzipName);
            return;
        }

        newParentFolder = dlDirectory.path();

        file = std::make_unique<QFile>(newParentFolder + '/' + fileName);

        if(!file->open(QIODevice::WriteOnly)) return;
    }
    else
    {   //指定されたダウンロード先のディレクトリが存在しない
        outErrorMessage("Invalid directory.");
        return;
    }

    startDownload(url);
}

void UpdateManager::startDownload(const QUrl& url)
{
    networkCanceledFlag = false;

    outNormalMessage("Start downloading ...");

    reply.reset(networkAccessManager.get(QNetworkRequest(url)));

    connect(reply.get(), &QNetworkReply::readyRead, this, &UpdateManager::readData);
    connect(reply.get(), &QNetworkReply::errorOccurred, this, &UpdateManager::receiveNetworkError);
    connect(reply.get(), &QNetworkReply::finished, this, &UpdateManager::unzipFile); //ダウンロードをキャンセルした場合もfinishedが送られる

    ProgressDialog *progressDialog = new ProgressDialog(url, this);
    progressDialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(reply.get(), &QNetworkReply::downloadProgress, progressDialog, &ProgressDialog::replyProgress);
    connect(reply.get(), &QNetworkReply::finished, progressDialog, &ProgressDialog::deleteLater);
    connect(progressDialog, &ProgressDialog::canceled, this, &UpdateManager::cancelUpdate);

    progressDialog->show();
    updateButton->setEnabled(false);
}

void UpdateManager::readData()
{
    if(file)
        file->write(reply->readAll());
}

void UpdateManager::unzipFile()
{
    if(networkCanceledFlag) return; //ダウンロードされていない場合は無効



    outNormalMessage("start unzip...");

    file->close();       //closeしないとunzipできない(アクセス拒否される)
    unzipThread.start();

    Unzip *unzip = new Unzip(nullptr);
    unzip->moveToThread(&unzipThread);

    connect(unzip, &Unzip::stdOutReceived, this, &UpdateManager::outNormalMessage);
    connect(unzip, &Unzip::stdErrReceived, this, &UpdateManager::outErrorMessage);
    connect(unzip, &Unzip::finished, this, &UpdateManager::updateApp);
    connect(unzip, &Unzip::finished, unzip, &Unzip::deleteLater);
    connect(this, &UpdateManager::unzipRequested, unzip, &Unzip::unzip);

    emit unzipRequested(file->fileName(), newParentFolder);
}

void UpdateManager::updateApp()
{
    file->remove(); //zipファイル削除
    unzipThread.quit();
    unzipThread.wait();

    outNormalMessage("Finished unzipping all files.");

    if(QProcess::startDetached(newParentFolder + '/' + unzipName + "/bin/release/GnuplotEditor.exe"))
        emit closeApplicationRequested();
    else
        outErrorMessage("Failed to start updated application.");
}

void UpdateManager::cancelUpdate()
{
    networkCanceledFlag = true;
    outNormalMessage("Download canceled.");

    reply->abort();
    updateButton->setEnabled(true);
    file->remove();
}



void UpdateManager::selectDirectory()
{
    const QString dir = QFileDialog::getExistingDirectory(this);

    if(dir.isEmpty()) return;

    directoryLineEdit->setText(dir);
}

void UpdateManager::outNormalMessage(const QString& message)
{
    QPalette p = messageLabel->palette();
    p.setColor(messageLabel->foregroundRole(), Qt::blue);
    messageLabel->setPalette(p);

    outMessage(message);
}

void UpdateManager::outErrorMessage(const QString& message)
{
    QPalette p = messageLabel->palette();
    p.setColor(messageLabel->foregroundRole(), Qt::red);
    messageLabel->setPalette(p);

    outMessage(message);
}

void UpdateManager::receiveNetworkError(const QNetworkReply::NetworkError& err)
{
    networkCanceledFlag = true;

    outErrorMessage("Network error caused : " + enumToString(err));
}

void UpdateManager::outMessage(const QString &message)
{
    if(message.isEmpty()) return;

    for(const QString& text : message.split('\n'))
    {
        if(text.isEmpty()) continue;
        messageLabel->setText(text);
    }
}




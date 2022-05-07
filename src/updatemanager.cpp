#include "updatemanager.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QDialogButtonBox>

#include <QFileDialog>

#include <QtWidgets>
#include <QtNetwork>

#include "zipper/unzipper.h"


const QString UpdateManager::defaultFileName = "pltEditor.zip";
const QString UpdateManager::downloadUrl = "https://github.com/nabla27/GnuplotEditor/archive/refs/heads/master.zip";


ProgressDialog::ProgressDialog(const QUrl& url, QWidget *parent)
    : QProgressDialog(parent)
{
    setWindowTitle("Download Progress");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
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
    const QString currentDirectory = QDir::currentPath();
    const qsizetype sp = currentDirectory.count('/');
    for(qsizetype i = 0; i < sp; ++i)
    {
        const QString dir = currentDirectory.section('/', 0, -i); //'/'でセクションに区切る。一番左0から右からのセクションi番目を取り出す
        if(QDir(dir).entryList().contains("unzip.exe"))
        {
            unzipExePath = dir + '/' + "unzip.exe";
            break;
        }
    }

    connect(process, &QProcess::readyReadStandardOutput, this, &Unzip::receiveStdOut);
    connect(process, &QProcess::readyReadStandardError, this, &Unzip::receiveStdErr);
}

void Unzip::unzip(const QString& zipName, const QString& dir)
{
    const int exitCode = process->execute(unzipExePath,
                                          QStringList() << "-o"     //上書き確認なし
                                                        << zipName  //解凍するzipファイル名
                                                        << "-d"     //ディレクトリ指定
                                                        << dir);    //展開するディレクトリ

    emit finished(exitCode);
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
    , messageLabel(new QLabel("", this))
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

    urlLineEdit->setReadOnly(true);
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
}

UpdateManager::~UpdateManager()
{

}

void UpdateManager::selectDirectory()
{
    const QString dir = QFileDialog::getExistingDirectory(this);

    if(dir.isEmpty()) return;

    directoryLineEdit->setText(dir);
}

void UpdateManager::requestUpdate()
{
    /* フォームに入力されたURL */
    const QString urlText = urlLineEdit->text().trimmed();
    if(urlText.isEmpty()) return;

    /* URL ダウンロードリンク */
    const QUrl url = QUrl::fromUserInput(urlText);
    if(!url.isValid()) return;

    /* ダウンロードリンク先のzipファイルのファイル名 */
    QString fileName = url.fileName();
    if(fileName.isEmpty()) fileName = defaultFileName;

    /* ダウンロードするディレクトリとダウンロードするファイル名 */
    QDir dlDirectory(directoryLineEdit->text().trimmed());
    if(dlDirectory.exists())
    {
        newParentFolder = dlDirectory.path();

        file = std::make_unique<QFile>(newParentFolder + '/' + fileName);

        if(!file->open(QIODevice::WriteOnly)) return;
    }
    else
    {   //指定されたダウンロード先のディレクトリが存在しない
        return;
    }

    startDownload(url);
}

void UpdateManager::startDownload(const QUrl& url)
{
    reply.reset(networkAccessManager.get(QNetworkRequest(url)));

    connect(reply.get(), &QNetworkReply::readyRead, this, &UpdateManager::readData);
    connect(reply.get(), &QNetworkReply::finished, this, &UpdateManager::updateFiles);

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

void UpdateManager::updateFiles()
{
    file->close();
    unzipThread.start();

    Unzip *unzip = new Unzip(nullptr);
    unzip->moveToThread(&unzipThread);

    connect(unzip, &Unzip::stdOutReceived, [](const QString& out){
        qDebug() << out;
    });
    connect(unzip, &Unzip::finished, unzip, &Unzip::deleteLater);
    connect(this, &UpdateManager::unzipRequested, unzip, &Unzip::unzip);
    connect(unzip, &Unzip::finished, [this](){
        qDebug() << "finished";
        file->remove();
        unzipThread.quit();
        unzipThread.wait();
    });

    emit unzipRequested(file->fileName(), newParentFolder);



    //file->remove();
}

void UpdateManager::cancelUpdate()
{
    reply->abort();
    messageLabel->setText("Download canceled");
    updateButton->setEnabled(true);
    file->remove();
}



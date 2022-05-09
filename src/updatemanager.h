#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressDialog>
#include <QFile>

#include <memory>

#include <QProcess>
#include <QThread>

class ProgressDialog : public QProgressDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(const QUrl& url, QWidget *parent = nullptr);

public slots:
    void replyProgress(qint64 bytesRead, qint64 totalBytes);
};





class Unzip : public QObject
{
    Q_OBJECT
public:
    Unzip(QObject *parent);

public slots:
    void unzip(const QString& zipName, const QString& dir);

private:
    QString unzipExePath;
    QProcess *process;

private slots:
    void receiveStdOut();
    void receiveStdErr();

signals:
    void stdOutReceived(const QString& stdOut);
    void stdErrReceived(const QString& stdErr);
    void finished();
};






class UpdateManager : public QDialog
{
    Q_OBJECT
public:
    explicit UpdateManager(QWidget *parent);
    ~UpdateManager();

private slots:
    void fetchRemoteVersion();
    void readRemoteVersionXml();
    void getVersionFromXml();

    void requestUpdate();
    void writeZipFile();
    void unzipFile();
    void updateApp();
    void cancelUpdate();

    void selectDirectory();
    void outNormalMessage(const QString& message);
    void outErrorMessage(const QString& message);
    void receiveNetworkError(const QNetworkReply::NetworkError& err);

private:
    void startDownload(const QUrl& url);
    void outMessage(const QString& message);

private:
    static const QString defaultFileName;     //ダウンロードするzipファイルの名前
    static const QString downloadUrl;         //ダウンロード先のURL
    static const QString unzipName;           //unzip後に展開されるフォルダー名
    static const QString localVersionPath;    //バージョンが書かれたファイルのアプリケーションフォルダーからの相対パス
    static const QString remoteVersionUrl;    //リモート上のバージョンが書かれたファイルのURL
    static const QString localExePath;        //実行ファイルのアプリケーションっフォルダーからの相対パス

    QLineEdit *urlLineEdit;
    QLineEdit *directoryLineEdit;
    QLineEdit *versionLineEdit;
    QLabel *messageLabel;
    QPushButton *updateButton;
    QPushButton *quitButton;

    QString newParentFolder;
    QString oldFolderPath;
    QString newVersion;

    QNetworkAccessManager networkAccessManager;
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply;
    std::unique_ptr<QFile> zipFile;         //リモートからのコピー先
    std::unique_ptr<QFile> xmlVersionFile;  //リモートからのコピー先
    bool networkCanceledFlag = false;  //ネットワークエラーや実行中にキャンセルされればTrueとする

    QThread unzipThread;

signals:
    void unzipRequested(const QString& zipName, const QString& dir);
    void closeApplicationRequested();
};

#endif // UPDATEMANAGER_H

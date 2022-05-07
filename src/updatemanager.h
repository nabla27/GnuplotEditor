#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>

#include <QUrl>
#include <QNetworkAccessManager>
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
    void finished(const int exitCode);
};


class UpdateManager : public QDialog
{
    Q_OBJECT
public:
    explicit UpdateManager(QWidget *parent);
    ~UpdateManager();

    void startRequest(const QUrl& requestedUrl);

private slots:
    void selectDirectory();
    void requestUpdate();
    void readData();
    void updateFiles();
    void cancelUpdate();

private:
    void startDownload(const QUrl& url);

private:
    static const QString defaultFileName;     //ダウンロードするzipファイルの名前
    static const QString downloadUrl;         //ダウンロード先のURL

    QLineEdit *urlLineEdit;
    QLineEdit *directoryLineEdit;
    QLineEdit *versionLineEdit;
    QLabel *messageLabel;
    QPushButton *updateButton;
    QPushButton *quitButton;

    QString newParentFolder;
    QString oldFolderPath;

    QNetworkAccessManager networkAccessManager;
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply;
    std::unique_ptr<QFile> file;
    bool httpsRequestAborted;

    QThread unzipThread;

signals:
    void unzipRequested(const QString& zipName, const QString& dir);
};

#endif // UPDATEMANAGER_H

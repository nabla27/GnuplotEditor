#ifndef Gnuplot_H
#define Gnuplot_H

/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

//#include <QString>
//#include <QScrollBar>
//#include <QObject>
//#include <QRegularExpressionMatchIterator>
//#include <QDir>
//#include "browserwidget.h"
//#include "utility.h"


//class Gnuplot : public QObject
//{
//    Q_OBJECT
//
//public:
//    Gnuplot(QObject *parent);
//
//public slots:
//    void exc(QProcess *process, const QList<QString>& cmdlist, const bool preHandling = true);
//    void setExePath(const QString& path) { this->path = path; }
//    void setInitCmd(const QString& initCmd) { this->initCmdList = initCmd.split('\n'); }
//    void setPreCmd(const QString& preCmd) { this->preCmdList = preCmd.split('\n'); }
//    void setWorkingDirectory(const QString& path) { this->workingDirectory = path; }
//
//private:
//    int getErrorLineNumber(const QString& err);
//
//private slots:
//    void readStandardOutput();
//    void readStandardError();
//    void receiveProcessError(const QProcess::ProcessError& error);
//
//private:
//    QString path;
//    QString workingDirectory;
//    QList<QString> initCmdList;
//    QList<QString> preCmdList;
//    QProcess *currentProcess;
//
//signals:
//    void standardOutputPassed(const QString& out);
//    void standardErrorPassed(const QString& out, const int line);
//    void errorCaused(const QString& err, const BrowserWidget::MessageType type);
//    void cmdPushed(const QString& cmd);
//};

#include <QObject>
#include <QProcess>
#include "logger.h"
#include "textcodec.h"


class Gnuplot;
class QThread;
class GnuplotProcess;


class GnuplotExecutor : public QObject
{
    Q_OBJECT
public:
    explicit GnuplotExecutor(QObject *parent);
    ~GnuplotExecutor();

public:
    void execGnuplot(GnuplotProcess *process, const QList<QString>& cmd, bool enablePreCmd);
    void execGnuplot(const QList<QString>& cmd, bool enablePreCmd);
    void setExePath(const QString& path);
    void setInitializeCmd(const QString& cmd);
    void setPreProcessingCmd(const QString& cmd);
    void setWorkingFolderPath(const QString& path);

    QThread* gnuplotThread() const { return _gnuplotThread; }
    GnuplotProcess *const defaultProcess() const { return _defaultProcess; }

private:
    QThread *_gnuplotThread;
    GnuplotProcess *_defaultProcess;

    class Gnuplot;
    Gnuplot *gnuplot;

signals:
    void executeRequested(GnuplotProcess *process, const QList<QString>& cmd, bool enablePreCmd);
    void setExePathRequested(const QString& path);
    void setInitializeCmdRequested(const QString& cmd);
    void setPreProcessingCmdRequested(const QString& cmd);
    void setWorkingFolderPathRequested(const QString& path);
};

extern GnuplotExecutor *gnuplotExecutor;




class GnuplotExecutor::Gnuplot : public QObject
{
    Q_OBJECT
public:
    explicit Gnuplot(QObject *parent);

public slots:
    void execute(GnuplotProcess *process, const QList<QString>& cmd, bool enablePreCmd);
    void setExePath(const QString& path) { this->exePath = path; }
    void setInitializeCmd(const QString& cmd) { this->initCmd = cmd.split("\n"); }
    void setPreProcessingCmd(const QString& cmd) { this->preCmd = cmd.split("\n"); }
    void setWorkingFoderPath(const QString& path) { this->workingPath = path; }

private:
    QString exePath;
    QList<QString> initCmd;
    QList<QString> preCmd;
    QString workingPath;
};








class GnuplotProcess : public QProcess
{
    Q_OBJECT
public:
    explicit GnuplotProcess(QObject *parent);

public:
    QString stdOut() const { return _stdOut; }
    QString stdErr() const { return _stdErr; }

    static void setCharCode(const TextCodec::CharCode& code);

private:
    void readStdOut();
    void readStdErr();

private:
    inline static TextCodec::CharCode charCode = TextCodec::CharCode::Shift_JIS;
    QString _stdOut;
    QString _stdErr;

signals:
    void standardOutputRead(const QString& out, const Logger::LogLevel& level);
    void errorCaused(const int errorLine);
    void aboutToExecute();
    void readyReadStdOut();
    void readyReadStdErr();
};








#endif // Gnuplot_H

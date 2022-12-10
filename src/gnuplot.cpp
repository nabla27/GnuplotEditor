/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "gnuplot.h"

//Gnuplot::Gnuplot(QObject *parent)
//    : QObject(parent)
//{
//}
//
//void Gnuplot::exc(QProcess *process, const QList<QString>& cmdlist, const bool preHandling)
//{
//    if(!process)  return;
//
//    currentProcess = process;
//
//    /* 標準出力 */
//    QObject::connect(process, &QProcess::readyReadStandardOutput, this, &Gnuplot::readStandardOutput);
//
//    /* 標準エラー */
//    QObject::connect(process, &QProcess::readyReadStandardError, this, &Gnuplot::readStandardError);
//
//    /* プロセスエラー */
//    QObject::connect(process, &QProcess::errorOccurred, this, &Gnuplot::receiveProcessError);
//    QObject::connect(process, &QProcess::errorOccurred, process, &QProcess::close);
//
//    /* プロセスの開始 */
//    if(process->state() == QProcess::ProcessState::NotRunning)
//    {
//        process->start(path, QStringList() << "-persist");
//        if(process->error() == QProcess::ProcessError::FailedToStart){
//            process->close();
//            emit errorCaused("failed to start the gnuplot process.", BrowserWidget::MessageType::ProcessErr);
//            return;
//        }
//    }
//
//    emit cmdPushed("\n[ " + QDateTime::currentDateTime().toString() + " ]     ProcessID(" + QString::number(process->processId()) + ")");
//
//    /* workingDirectoryに移動 */
//    if(!workingDirectory.isEmpty())
//    {
//        const QString moveDirCmd = "cd '" + workingDirectory + "'";
//        process->write((moveDirCmd + "\n").toUtf8().constData());
//        emit cmdPushed(moveDirCmd);
//    }
//
//    if(preHandling)
//    {
//        /* 初期コマンドの実行 */
//        for(const QString& initCmd : initCmdList)
//        {
//            process->write((initCmd + "\n").toUtf8().constData());
//            emit cmdPushed(initCmd);
//        }
//
//        /* 事前コマンドの実行 */
//        for(const QString& preCmd : preCmdList)
//        {
//            process->write((preCmd + "\n").toUtf8().constData());
//            emit cmdPushed(preCmd);
//        }
//    }
//
//    /* コマンドの実行 */
//    for(const QString& cmd : cmdlist)
//    {
//        process->write((cmd + "\n").toUtf8().constData());
//        emit cmdPushed(cmd);
//    }
//}
//
//int Gnuplot::getErrorLineNumber(const QString &err)
//{
//    /* エラー行の取得 */
//    QList<int> list;  //エラー行を格納するリスト
//    QRegularExpressionMatchIterator iter = QRegularExpression("line \\d+:").globalMatch(err);
//    while(iter.hasNext()){
//        QRegularExpressionMatch match = iter.next();
//        list << match.captured(0).sliced(5, match.captured(0).size() - 6).toInt();
//    }
//
//    return (list.size() < 1) ? -1 : list.at(0);
//}
//
//void Gnuplot::readStandardOutput()
//{
//    const QString output = currentProcess->readAllStandardOutput();
//
//    if(output.isEmpty()) return;
//
//    emit standardOutputPassed(output);
//}
//
//void Gnuplot::readStandardError()
//{
//    const QString output = currentProcess->readAllStandardError();
//
//    if(output.isEmpty()) return;
//
//    const int errLine = getErrorLineNumber(output); //エラー行を示す表現が見つけられなかったら-1を返す
//
//    if(errLine == -1)
//        emit standardOutputPassed(output);          //エラーじゃない標準出力でもreadAllStandardError()で拾われてしまう
//    else
//        emit standardErrorPassed(output, errLine);
//}
//
//void Gnuplot::receiveProcessError(const QProcess::ProcessError& error)
//{
//    emit errorCaused("Process error has occurred [" + enumToString(error) + "]. The process is closed.", BrowserWidget::MessageType::ProcessErr);
//}

#include <QThread>
#include <QProcess>
#include <QDebug>



GnuplotExecutor::GnuplotExecutor(QObject *parent)
    : QObject(parent)
    , _gnuplotThread(new QThread(this))
    , defaultProcess(new QProcess())
    , gnuplot(new Gnuplot(nullptr))
{
    defaultProcess->moveToThread(_gnuplotThread);
    gnuplot->moveToThread(_gnuplotThread);
    _gnuplotThread->start();

    /* ランタイムエラーの回避
     * qt.core.qobject.connect: QObject::connect: Cannot queue arguments of type 'QProcess*'
     * (Make sure 'QProcess*' is registered using qRegisterMetaType().)
     */
    qRegisterMetaType<QProcess*>();

    connect(this, &GnuplotExecutor::executeRequested, gnuplot, &GnuplotExecutor::Gnuplot::execute);
    connect(this, &GnuplotExecutor::setExePathRequested, gnuplot, &GnuplotExecutor::Gnuplot::setExePath);
    connect(this, &GnuplotExecutor::setInitializeCmdRequested, gnuplot, &GnuplotExecutor::Gnuplot::setInitializeCmd);
    connect(this, &GnuplotExecutor::setPreProcessingCmdRequested, gnuplot, &GnuplotExecutor::Gnuplot::setPreProcessingCmd);
    connect(this, &GnuplotExecutor::setWorkingFolderPathRequested, gnuplot, &GnuplotExecutor::Gnuplot::setWorkingFoderPath);
}

GnuplotExecutor::~GnuplotExecutor()
{
    _gnuplotThread->quit();
    _gnuplotThread->wait();
}

void GnuplotExecutor::execGnuplot(QProcess *process, const QList<QString>&cmd, bool enablePreCmd)
{
    emit executeRequested(process, cmd, enablePreCmd);
}

void GnuplotExecutor::execGnuplot(const QList<QString>& cmd, bool enablePreCmd)
{
    execGnuplot(defaultProcess, cmd, enablePreCmd);
}

void GnuplotExecutor::setExePath(const QString &path)
{
    emit setExePathRequested(path);
}

void GnuplotExecutor::setInitializeCmd(const QString &cmd)
{
    emit setInitializeCmdRequested(cmd);
}

void GnuplotExecutor::setPreProcessingCmd(const QString &cmd)
{
    emit setPreProcessingCmdRequested(cmd);
}

void GnuplotExecutor::setWorkingFolderPath(const QString &path)
{
    emit setWorkingFolderPathRequested(path);
}




GnuplotExecutor *gnuplotExecutor = new GnuplotExecutor(nullptr);

GnuplotExecutor::Gnuplot::Gnuplot(QObject *parent)
    : QObject(parent)
{

}

void GnuplotExecutor::Gnuplot::execute(QProcess *process, const QList<QString>& cmdlist, bool enablePreCmd)
{
    if(!process)
    {
        /* メッセージ */
        return;
    }

    if(process->state() == QProcess::ProcessState::NotRunning)
    {
        process->start(exePath, QStringList() << "-persist");

        if(process->error() == QProcess::ProcessError::FailedToStart)
        {
            process->close();
            /* エラーメッセージを送る */
            return;
        }
    }

    /* workingFolderPath に移動 */
    {
        const QString moveDirCmd = "cd '" + workingPath + "'";
        process->write((moveDirCmd + "\n").toUtf8().constData());
        /* メッセージ */
    }

    if(enablePreCmd)
    {
        for(const QString& initCmd : this->initCmd)
        {
            process->write((initCmd + "\n").toUtf8().constData());
            /* メッセージ */
        }

        for(const QString& preCmd : this->preCmd)
        {
            process->write((preCmd + "\n").toUtf8().constData());
            /* メッセージ */
        }
    }
    qDebug() << __LINE__;
    for(const QString& cmd : cmdlist)
    {
        process->write((cmd + "\n").toUtf8().constData());
        /* メッセージ */
    }
}

























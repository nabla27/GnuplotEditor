#include "gnuplot.h"

Gnuplot::Gnuplot(QObject *parent)
    : QObject(parent)
{
}

void Gnuplot::exc(QProcess *process, const QList<QString>& cmdlist)
{
    if(!process)  return;

    currentProcess = process;

    emit cmdPushed("\n[ " + QDateTime::currentDateTime().toString() + " ]     ProcessID(" + QString::number(process->processId()) + ")");

    /* 標準出力 */
    QObject::connect(process, &QProcess::readyReadStandardOutput, this, &Gnuplot::readStandardOutput);

    /* 標準エラー */
    QObject::connect(process, &QProcess::readyReadStandardError, this, &Gnuplot::readStandardError);

    /* プロセスの開始 */
    if(process->state() == QProcess::ProcessState::NotRunning)
    {
        process->start(path, QStringList() << "-persist");
        if(process->error() == QProcess::ProcessError::FailedToStart){
            process->close();
            emit errorCaused("failed to start the gnuplot process.", BrowserWidget::MessageType::ProcessErr);
        }
    }

    /* workingDirectoryに移動 */
    const QString moveDirCmd = "cd '" + workingDirectory + "'";
    process->write((moveDirCmd + "\n").toUtf8().constData());
    emit cmdPushed(moveDirCmd);

    /* 初期コマンドの実行 */
    for(const QString& initCmd : initCmdList)
    {
        process->write((initCmd + "\n").toUtf8().constData());
        emit cmdPushed(initCmd);
    }

    /* 事前コマンドの実行 */
    for(const QString& preCmd : preCmdList)
    {
        process->write((preCmd + "\n").toUtf8().constData());
        emit cmdPushed(preCmd);
    }

    /* コマンドの実行 */
    for(const QString& cmd : cmdlist)
    {
        process->write((cmd + "\n").toUtf8().constData());
        emit cmdPushed(cmd);
    }
}

int Gnuplot::getErrorLineNumber(const QString &err)
{
    /* エラー行の取得 */
    QList<int> list;  //エラー行を格納するリスト
    QRegularExpressionMatchIterator iter = QRegularExpression("line \\d+:").globalMatch(err);
    while(iter.hasNext()){
        QRegularExpressionMatch match = iter.next();
        list << match.captured(0).sliced(5, match.captured(0).size() - 6).toInt();
    }

    return (list.size() < 1) ? -1 : list.at(0);
}

void Gnuplot::readStandardOutput()
{
    const QString output = currentProcess->readAllStandardOutput();

    if(output.isEmpty()) return;

    emit standardOutputPassed(output);
}

void Gnuplot::readStandardError()
{
    const QString output = currentProcess->readAllStandardError();

    if(output.isEmpty()) return;

    const int errLine = getErrorLineNumber(output); //エラー行を示す表現が見つけられなかったら-1を返す

    if(errLine == -1)
        emit standardOutputPassed(output);          //エラーじゃない標準出力でもreadAllStandardError()で拾われてしまう
    else
        emit standardErrorPassed(output, errLine);
}



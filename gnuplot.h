#ifndef Gnuplot_H
#define Gnuplot_H

#include <QProcess>
#include <QList>
#include <QString>
#include <QScrollBar>
#include <QObject>
#include <QRegularExpressionMatchIterator>
#include <QDir>
#include "browserwidget.h"
#include "utility.h"


class Gnuplot : public QObject
{
    Q_OBJECT

public:
    Gnuplot(QObject *parent);

public:
    void exc(QProcess *process, const QList<QString>& cmdlist);
    void setExePath(const QString& path) { this->path = path; }
    void setPreCmd(const QString& preCmd) { this->preCmdList = preCmd.split('\n'); }
    void setWorkingPath(const QString& workingPath) { this->workingPath = workingPath; }

private:
    int getErrorLineNumber(const QString& err);

private slots:
    void readStandardOutput();
    void readStandardError();

private:
    QString path = "gnuplot.exe";
    QString workingPath = QDir::currentPath() + "/" + BasicSet::tmpDirectory;
    QList<QString> initCmdList;
    QList<QString> preCmdList;
    QProcess *currentProcess;

signals:
    void standardOutputPassed(const QString& out);
    void standardErrorPassed(const QString& out, const int line);
    void errorCaused(const QString& err, const BrowserWidget::MessageType type);
};
#endif // Gnuplot_H

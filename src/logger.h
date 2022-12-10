/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QSet>
#include <QFile>
#include <QThread>
#include <QTextStream>
#include <QWidget>


class QTextBrowser;
class QFileSystemWatcher;


class Logger : public QObject
{
    Q_OBJECT
public:
    Logger(QObject *parent);

    enum class LogLevel { Debug, Info, Warn, Error, Fatal, GnuplotInfo, GnuplotStdOut, GnuplotStdErr };
    Q_ENUM(LogLevel)

public slots:
    void output(const QString& message, const Logger::LogLevel& level);
    void output(const QString& file, const int line, const QString& func, const QString& message, const Logger::LogLevel& level);
    void setLogFilePath(const QString& path);
    void showLogViwer();

private:
    void setupViwer();

private:
    QThread ioThread;

    class LogWriter;
    class LogViwer;
    LogWriter *writer;
    LogViwer *viwer;

signals:
    void writeRequested(const QString& message, const Logger::LogLevel& level);
    void logPathChanged(const QString& path);
};
extern Logger *logger;








class Logger::LogWriter : public QObject
{
    Q_OBJECT
public:
    explicit LogWriter(QObject *parent);
    ~LogWriter();

public slots:
    void write(const QString& message, const Logger::LogLevel& level);
    void setLogFilePath(const QString& path);

private:
    QString logFilePath;
};






class Logger::LogViwer : public QWidget
{
    Q_OBJECT
public:
    explicit LogViwer(QWidget *parent);

public:
    void load(const QString& path);
    void setLogFilePath(const QString& path);

private:
    QTextBrowser *browser;
    QFileSystemWatcher *fileWatcher;

signals:
    void logPushed(const QString& file, const int line, const QString& func, const QString& message, const Logger::LogLevel& level);
};













#endif // LOGGER_H

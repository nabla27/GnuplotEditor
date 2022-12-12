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
#include <QTextBrowser>


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

private:
    QThread ioThread;

    class LogWriter;
    LogWriter *writer;

signals:
    /* public signal */
    void logPushed(const QString& message, const Logger::LogLevel& level);
    /* private signal */
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








class LogBrowserWidget : public QTextBrowser
{
    Q_OBJECT
public:
    explicit LogBrowserWidget(QWidget *parent);

public:
    void addFilter(const Logger::LogLevel& level);
    void addAllFilter();
    void removeFilter(const Logger::LogLevel& level);
    void clearFilter();

public slots:
    void appendLog(const QString& message, const Logger::LogLevel& level);
    void setAutoScroll(bool enable);
    void grayOutAll();

private:
    QSet<Logger::LogLevel> logFilter;
    static QHash<Logger::LogLevel, QColor> logLevelColor;

    bool isAutoScroll = false;
};













#endif // LOGGER_H

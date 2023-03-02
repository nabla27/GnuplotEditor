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
class QLineEdit;


class Logger : public QObject
{
    Q_OBJECT
public:
    Logger(QObject *parent);

    enum class LogLevel { Debug, Info, Warn, Error, Fatal, GnuplotInfo, GnuplotStdOut, GnuplotStdErr };
    Q_ENUM(LogLevel)

    QString logFilePath() const;

public slots:
    void output(const QString& message, const Logger::LogLevel& level);
    void output(const QString& file, const int line, const QString& func, const QString& message, const Logger::LogLevel& level);
    void setLogFilePath(const QString& path);

private:
    QThread ioThread;

    class LogWriter;
    LogWriter *writer;

    QSet<LogLevel> dialogFilter;

signals:
    /* public signal */
    void logPushed(const QString& message, const Logger::LogLevel& level);
    void logPathChanged(const QString& path);
    /* request output */
    void outputRequested(const QString& file, const int line, const QString& func, const QString& message, const Logger::LogLevel& level);
};
extern Logger *logger;

/* cannot output the message with below macro from other thread which is not main gui thread.
 * It's because that the Logger::output() function call the QMessageBox widget. */
//#define __LOGOUT__(comment, type) logger->output(__FILE__, __LINE__, __FUNCTION__, comment, type)
/* output message from a below signal to call other thread. */
#define __LOGOUT__(comment, type) Q_EMIT logger->outputRequested(__FILE__, __LINE__, __FUNCTION__, comment, type)






class Logger::LogWriter : public QObject
{
    Q_OBJECT
public:
    explicit LogWriter(QObject *parent);
    ~LogWriter();

    QString logFilePath() const { return _logFilePath; }

public slots:
    void write(const QString& message, const Logger::LogLevel& level);
    void setLogFilePath(const QString& path);

private:
    QString _logFilePath;
};








class LogBrowserWidget : public QTextBrowser
{
    Q_OBJECT
public:
    explicit LogBrowserWidget(QWidget *parent);
    QSize sizeHint() const override { return QSize(0, 0); }

public:
    void addFilter(const Logger::LogLevel& level);
    void addAllFilter();
    void removeFilter(const Logger::LogLevel& level);
    void clearFilter();
    void setLogColor(const Logger::LogLevel& level, const QColor& color);

    bool hasFilter(const Logger::LogLevel& level) const { return logFilter.contains(level); }
    QColor logColor(const Logger::LogLevel& level) const { return logLevelColor.value(level); }
    bool isAutoScroll() const { return _isAutoScroll; }

public slots:
    void appendLog(const QString& message, const Logger::LogLevel& level);
    void setAutoScroll(bool enable);
    void grayOutAll();
    void setDefaultLogColor();

private:
    QSet<Logger::LogLevel> logFilter;
    QHash<Logger::LogLevel, QColor> logLevelColor;
    bool _isAutoScroll = false;

signals:
    void filterChanged(const Logger::LogLevel& level, bool enable);
    void logColorChanged(const Logger::LogLevel& level, const QColor& color);
    void autoScrollChanged(const bool enable);
};


class LogBrowserSettingWidget : public QWidget
{
    Q_OBJECT
public:
    LogBrowserSettingWidget(LogBrowserWidget *browser, QWidget *parent);

private:
    LogBrowserWidget *browser;
};








class LogSettingWidget : public QWidget
{
    Q_OBJECT
public:
    LogSettingWidget(QWidget *parent);

private slots:
    void openLogFileDialog();
    void openLogFile();

private:
    LogBrowserWidget *logBrowser;
    LogBrowserSettingWidget *browserSetting;
    QLineEdit *logPathEdit;
};












#endif // LOGGER_H

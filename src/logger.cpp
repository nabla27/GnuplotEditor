/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "logger.h"

#include "iofile.h"
#include "utility.h"
#include <QFileSystemWatcher>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QTimer>



Logger::Logger(QObject *parent)
    : QObject(parent)
    , writer(new LogWriter(nullptr))
{
    writer->moveToThread(&ioThread);
    ioThread.start();

    connect(this, &Logger::logPushed, writer, &Logger::LogWriter::write);
    connect(this, &Logger::logPathChanged, writer, &Logger::LogWriter::setLogFilePath);
}

Logger *logger = new Logger(nullptr);

void Logger::output(const QString &message, const LogLevel &level)
{
    emit logPushed(message, level);
}

void Logger::output(const QString &file, const int line, const QString& func, const QString &message, const LogLevel &level)
{
    output("FILE(" + file + ") LINE(" + QString::number(line) + ") FUNC(" + func + ")\n" + message, level);
}

void Logger::setLogFilePath(const QString &path)
{
    emit logPathChanged(path);
}








Logger::LogWriter::LogWriter(QObject *parent)
    : QObject(parent)
{
}

Logger::LogWriter::~LogWriter()
{
}

void Logger::LogWriter::write(const QString &message, const LogLevel &level)
{
    QFile file(logFilePath);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QTextStream out(&file);

        out << "[" + enumToString(level) + "] " + QDateTime::currentDateTime().toString() + "\n";
        out << message + "\n";
        out << "\n";

        file.close();
    }
}

void Logger::LogWriter::setLogFilePath(const QString &path)
{
    logFilePath = path;
}












QHash<Logger::LogLevel, QColor> LogBrowserWidget::logLevelColor
= { { Logger::LogLevel::Debug, Qt::black },
    { Logger::LogLevel::Info, Qt::black},
    { Logger::LogLevel::Warn, Qt::yellow},
    { Logger::LogLevel::Error, Qt::red},
    { Logger::LogLevel::Fatal, Qt::red},
    { Logger::LogLevel::GnuplotInfo, Qt::black},
    { Logger::LogLevel::GnuplotStdOut, Qt::blue},
    { Logger::LogLevel::GnuplotStdErr, Qt::red}
  };

LogBrowserWidget::LogBrowserWidget(QWidget *parent)
    : QTextBrowser(parent)
{
    setFontFamily("Consolas");
}

void LogBrowserWidget::addFilter(const Logger::LogLevel &level)
{
    logFilter.insert(level);
}

void LogBrowserWidget::addAllFilter()
{
    for(int i = 0; i < QMetaEnum::fromType<Logger::LogLevel>().keyCount(); ++i)
    {
        logFilter.insert(Logger::LogLevel(i));
    }
}

void LogBrowserWidget::removeFilter(const Logger::LogLevel &level)
{
    logFilter.remove(level);
}

void LogBrowserWidget::clearFilter()
{
    logFilter.clear();
}

void LogBrowserWidget::appendLog(const QString &message, const Logger::LogLevel &level)
{
    if(!logFilter.contains(level)) return;

    QTextCursor tc = textCursor();
    tc.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    const int previousEndPosition = tc.position();

    append("");
    append("[" + enumToString(level) + "] " + QDateTime::currentDateTime().toString());
    append(message);

    tc.setPosition(previousEndPosition);
    tc.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);

    QTextCharFormat fm = tc.charFormat();
    fm.setForeground(logLevelColor.value(level));
    tc.setCharFormat(fm);

    if(isAutoScroll)
        verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

void LogBrowserWidget::setAutoScroll(bool enable)
{
    isAutoScroll = enable;
}

void LogBrowserWidget::grayOutAll()
{
    QTextCursor tc = textCursor();
    tc.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    tc.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);

    QTextCharFormat fm = tc.charFormat();
    fm.setForeground(Qt::lightGray);
    tc.setCharFormat(fm);
}









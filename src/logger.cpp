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


Logger::Logger(QObject *parent)
    : QObject(parent)
    , writer(new LogWriter(nullptr))
    , viwer(nullptr)
{
    writer->moveToThread(&ioThread);
    ioThread.start();

    connect(this, &Logger::writeRequested, writer, &Logger::LogWriter::write);
    connect(this, &Logger::logPathChanged, writer, &Logger::LogWriter::setLogFilePath);
}

Logger *logger = new Logger(nullptr);

void Logger::output(const QString &message, const LogLevel &level)
{
    emit writeRequested(message, level);
}

void Logger::output(const QString &file, const int line, const QString& func, const QString &message, const LogLevel &level)
{
    emit writeRequested("File(" + file + ") LINE(" + QString::number(line) + ") FUNC(" + func + ")\n" + message, level);
}

void Logger::setLogFilePath(const QString &path)
{
    if(!viwer)
    {
        setupViwer();
        viwer->load(path);
    }


    viwer->setLogFilePath(path);
    emit logPathChanged(path);
}

void Logger::showLogViwer()
{
    if(!viwer) setupViwer();

    viwer->show();
}

void Logger::setupViwer()
{
    if(viwer) delete viwer;

    viwer = new Logger::LogViwer(nullptr);
    connect(this, &Logger::destroyed, viwer, &Logger::LogViwer::deleteLater);
    connect(viwer, &Logger::LogViwer::logPushed, this, QOverload<const QString&, const int, const QString&, const QString&, const Logger::LogLevel&>::of(&Logger::output));
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









Logger::LogViwer::LogViwer(QWidget *parent)
    : QWidget(parent)
    , browser(new QTextBrowser(this))
    , fileWatcher(new QFileSystemWatcher(this))
{
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, &Logger::LogViwer::load);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    setLayout(vLayout);
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);

    vLayout->addWidget(browser);

    setWindowFlag(Qt::WindowType::Window);
}

void Logger::LogViwer::load(const QString &path)
{
    QFile file(path);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        browser->setPlainText(in.readAll());
        file.close();
    }
    else
    {
        emit logPushed(__FILE__, __LINE__, __FUNCTION__, "failed to read log file.", LogLevel::Error);
    }
}

void Logger::LogViwer::setLogFilePath(const QString &path)
{
    fileWatcher->removePaths(fileWatcher->files());
    fileWatcher->addPath(path);
}








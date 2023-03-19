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
#include "layoutparts.h"
#include "standardpixmap.h"

#include <QFileSystemWatcher>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QTimer>
#include <QMessageBox>
#include <QDateTime>
#include <QCheckBox>
#include <QScreen>
#include <QLineEdit>
#include <QToolButton>
#include <QDesktopServices>
#include <QFileDialog>



Logger::Logger(QObject *parent)
    : QObject(parent)
    , writer(new LogWriter(nullptr))
{
    writer->moveToThread(&ioThread);
    ioThread.start();

    dialogFilter.insert(Logger::LogLevel::Error);
    dialogFilter.insert(Logger::LogLevel::Fatal);
    //dialogFilter.insert(Logger::LogLevel::Warn);

    connect(this, &Logger::logPushed, writer, &Logger::LogWriter::write);
    connect(this, &Logger::logPathChanged, writer, &Logger::LogWriter::setLogFilePath);
    connect(this, &Logger::outputRequested,
            this, QOverload<const QString&, const int, const QString&, const QString&, const LogLevel&>::of(&Logger::output));
}

Logger *logger = new Logger(nullptr);

QString Logger::logFilePath() const
{
    return writer->logFilePath();
}

void Logger::output(const QString &message, const LogLevel &level)
{
    emit logPushed(message, level);
}

void Logger::output(const QString &file, const int line, const QString& func, const QString &message, const LogLevel &level)
{
    output("FILE(" + file + ") LINE(" + QString::number(line) + ") FUNC(" + func + ")\n" + message, level);

    if(dialogFilter.contains(level))
    {
        QMessageBox messageBox;
        messageBox.setWindowFlags(messageBox.windowFlags() | Qt::WindowStaysOnTopHint);
        messageBox.setWindowTitle(file + " " + QString::number(line));
        messageBox.setText(message);

        switch(level)
        {
        case LogLevel::Fatal:
        case LogLevel::Error:
        {
            messageBox.setIconPixmap(QMessageBox::standardIcon(QMessageBox::Icon::Critical));
            break;
        }
        case LogLevel::Warn:
        {
            messageBox.setIconPixmap(QMessageBox::standardIcon(QMessageBox::Icon::Warning));
            break;
        }
        case LogLevel::Info:
        case LogLevel::Debug:
        {
            messageBox.setIconPixmap(QMessageBox::standardIcon(QMessageBox::Icon::Information));
            break;
        }
        default:
            return;
        }

        messageBox.exec();
    }
}

void Logger::setLogFilePath(const QString &path)
{
    if(logFilePath() != path)
    {
        emit logPathChanged(path);
    }
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
    QFile file(_logFilePath);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QTextStream out(&file);

        out << "[" + mutility::enumToString(level) + "] " + QDateTime::currentDateTime().toString() + "\n";
        out << message + "\n";
        out << "\n";

        file.close();
    }
}

void Logger::LogWriter::setLogFilePath(const QString &path)
{
    _logFilePath = path;
}












LogBrowserWidget::LogBrowserWidget(QWidget *parent)
    : QTextBrowser(parent)
{
    setFontFamily("Consolas");
    setDefaultLogColor();
}

void LogBrowserWidget::addFilter(const Logger::LogLevel &level)
{
    logFilter.insert(level);
    emit filterChanged(level, true);
}

void LogBrowserWidget::addAllFilter()
{
    for(int i = 0; i < QMetaEnum::fromType<Logger::LogLevel>().keyCount(); ++i)
    {
        logFilter.insert(Logger::LogLevel(i));
        emit filterChanged(Logger::LogLevel(i), true);
    }
}

void LogBrowserWidget::removeFilter(const Logger::LogLevel &level)
{
    logFilter.remove(level);
    emit filterChanged(level, false);
}

void LogBrowserWidget::clearFilter()
{
    for(auto&& level: logFilter)
    {
        logFilter.remove(level);
        emit filterChanged(level, false);
    }
}

void LogBrowserWidget::setLogColor(const Logger::LogLevel &level, const QColor &color)
{
    if(logLevelColor.value(level) != color)
    {
        logLevelColor.insert(level, color);
        emit logColorChanged(level, color);
    }
}

void LogBrowserWidget::setDefaultLogColor()
{
    static const QHash<Logger::LogLevel, QColor> defaultColor = { { Logger::LogLevel::Debug, Qt::darkBlue },
                                                                { Logger::LogLevel::Info, Qt::black},
                                                                { Logger::LogLevel::Warn, Qt::darkRed},
                                                                { Logger::LogLevel::Error, Qt::red},
                                                                { Logger::LogLevel::Fatal, Qt::red},
                                                                { Logger::LogLevel::GnuplotInfo, Qt::darkCyan},
                                                                { Logger::LogLevel::GnuplotStdOut, Qt::blue},
                                                                { Logger::LogLevel::GnuplotStdErr, Qt::red} };

    for(auto i = defaultColor.constBegin(); i != defaultColor.constEnd(); ++i)
    {
        if(!logLevelColor.contains(i.key()) || logLevelColor.value(i.key()) != i.value())
        {
            logLevelColor.insert(i.key(), i.value());
            emit logColorChanged(i.key(), i.value());
        }
    }
}


void LogBrowserWidget::appendLog(const QString &message, const Logger::LogLevel &level)
{
    if(!logFilter.contains(level)) return;

    QTextCursor tc = textCursor();
    tc.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    const int previousEndPosition = tc.position();

    append("");
    append("[" + mutility::enumToString(level) + "] " + QDateTime::currentDateTime().toString());
    append(message);

    tc.setPosition(previousEndPosition);
    tc.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);

    QTextCharFormat fm = tc.charFormat();
    fm.setForeground(logLevelColor.value(level));
    tc.setCharFormat(fm);

    if(_isAutoScroll)
        verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

void LogBrowserWidget::setAutoScroll(bool enable)
{
    if(_isAutoScroll != enable)
    {
        _isAutoScroll = enable;
        emit autoScrollChanged(enable);
    }
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


LogBrowserSettingWidget::LogBrowserSettingWidget(LogBrowserWidget *browser, QWidget *parent)
    : QWidget(parent)
    , browser(browser)
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QGridLayout *gLayout = new QGridLayout;
    setLayout(vLayout);
    vLayout->addLayout(gLayout);
    vLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));

    const QStringList logLevels = mutility::enumToStrings(Logger::LogLevel(0));

    for(int i = 0; i < logLevels.size(); ++i)
    {
        QCheckBox *enableCheck = new QCheckBox(mutility::enumToString(Logger::LogLevel(i)), this);
        mlayout::ColorButton *messageColor = new mlayout::ColorButton("", QPalette::ColorRole::ButtonText, this);

        enableCheck->setChecked(browser->hasFilter(Logger::LogLevel(i)));
        messageColor->setFillBackGround(true);
        messageColor->setFixedHeight(15);
        messageColor->setDialogColor(browser->logColor(Logger::LogLevel(i)));

        gLayout->addWidget(enableCheck, i, 0);
        gLayout->addWidget(messageColor, i, 1);

        auto setCheckBoxFromBrowser = [=](const Logger::LogLevel& level, bool enable)
        {
            if((int)level == i) enableCheck->setChecked(enable);
        };
        auto setFilterFromCheckBox = [=](bool checked)
        {
            if(checked) browser->addFilter(Logger::LogLevel(i));
            else        browser->removeFilter(Logger::LogLevel(i));
        };
        auto setDialogColroFromBrowser = [=](const Logger::LogLevel& level, const QColor& color)
        {
            if((int)level == i) messageColor->setDialogColor(color);
        };
        auto setLogColorFromDialog = [=](const QColor& color)
        {
            browser->setLogColor(Logger::LogLevel(i), color);
        };

        connect(browser, &LogBrowserWidget::filterChanged, setCheckBoxFromBrowser);
        connect(enableCheck, &QCheckBox::toggled, setFilterFromCheckBox);
        connect(browser, &LogBrowserWidget::logColorChanged, setDialogColroFromBrowser);
        connect(messageColor, &mlayout::ColorButton::colorChanged, setLogColorFromDialog);
    }

    QPushButton *defaultColorButton = new QPushButton("Default Color", this);
    gLayout->addWidget(defaultColorButton, logLevels.size(), 0, 1, 2);
    connect(defaultColorButton, &QPushButton::released, browser, &LogBrowserWidget::setDefaultLogColor);

    QCheckBox *autoScrollCheck = new QCheckBox("Auto scroll", this);
    gLayout->addWidget(autoScrollCheck, logLevels.size() + 1, 0);
    autoScrollCheck->setChecked(browser->isAutoScroll());
    connect(browser, &LogBrowserWidget::autoScrollChanged, autoScrollCheck, &QCheckBox::setChecked);
    connect(autoScrollCheck, &QCheckBox::toggled, browser, &LogBrowserWidget::setAutoScroll);
}



LogSettingWidget::LogSettingWidget(QWidget *parent)
    : QWidget(parent)
    , logBrowser(new LogBrowserWidget(this))
    , browserSetting(new LogBrowserSettingWidget(logBrowser, this))
    , logPathEdit(new QLineEdit(this))
{
    setGeometry(mutility::getRectFromScreenRatio(screen()->size(), 0.4f, 0.4f));

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QHBoxLayout *hLayout = new QHBoxLayout;
    QGridLayout *gLayout = new QGridLayout;

    QToolButton *openDialogButton = new QToolButton(this);
    QToolButton *openLogFileButton = new QToolButton(this);

    setLayout(vLayout);
    vLayout->addLayout(hLayout);
    hLayout->addWidget(logBrowser);
    hLayout->addWidget(browserSetting);
    vLayout->addLayout(gLayout);
    gLayout->addWidget(new QLabel("Log file path"), 0, 0);
    gLayout->addWidget(logPathEdit, 0, 1);
    gLayout->addWidget(openDialogButton, 0, 2);
    gLayout->addWidget(openLogFileButton, 0, 3);

    logPathEdit->setReadOnly(true);
    logPathEdit->setText(logger->logFilePath());
    logPathEdit->setToolTip(logger->logFilePath());
    openDialogButton->setText("...");
    openDialogButton->setToolTip("choose log file from dialog");
    openLogFileButton->setIcon(QIcon(StandardPixmap::File::folderOpen().scaled(20, 20)));
    openLogFileButton->setToolTip("open log file in default app");

    logBrowser->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    logBrowser->addAllFilter();

    connect(logger, &Logger::logPushed, logBrowser, &LogBrowserWidget::appendLog);
    connect(logger, &Logger::logPathChanged, logPathEdit, &QLineEdit::setText);
    connect(logger, &Logger::logPathChanged, logPathEdit, &QLineEdit::setToolTip);
    connect(openDialogButton, &QToolButton::released, this, &LogSettingWidget::openLogFileDialog);
    connect(openLogFileButton, &QToolButton::released, this, &LogSettingWidget::openLogFile);
}

void LogSettingWidget::openLogFileDialog()
{
    const QString path = QFileDialog::getOpenFileName(this);

    if(path.isEmpty()) return;

    logger->setLogFilePath(path);
}

void LogSettingWidget::openLogFile()
{
    if(!QDesktopServices::openUrl(QUrl::fromLocalFile(logPathEdit->text())))
        __LOGOUT__("failed to open log file in desktop survice.", Logger::LogLevel::Warn);
}










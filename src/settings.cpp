/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */


#include "settings.h"

#include <QSettings>
#include <QApplication>
#include <QDate>
#include <QThreadPool>
#include "logger.h"


SettingController::SettingController(QObject *parent)
    : QObject(parent)
{
    connect(this, &SettingController::loadRequested, this, &SettingController::loadSettings);
    connect(this, &SettingController::saveRequested, this, &SettingController::saveSettings);
}

void SettingController::loadSettings()
{
    Settings::setup();
    emit settingsLoaded();
}

void SettingController::saveSettings()
{
    Settings::save();
    emit settingsSaved();
}

void SettingController::moveToGlobalThread()
{
    moveToThread(QThreadPool::globalInstance()->thread());
}








void Settings::setup()
{
    QApplication::setApplicationName(applicationName());
    QApplication::setApplicationDisplayName(applicationName());
    QApplication::setApplicationVersion(applicationVersion());

    logger->setLogFilePath(applicationSettingsDirPath() + '/' + applicationName() + ".log");

    QSettings settings(iniFilePath(), QSettings::IniFormat);
    settings.beginGroup("Settings");
    settings.endGroup();

    qDebug() << __FILE__ << __LINE__ << "loaded";
}

void Settings::save()
{
    QSettings settings(iniFilePath(), QSettings::IniFormat);
    settings.beginGroup("Settings");
    settings.setValue("Date", QDate::currentDate().toString());
    settings.setValue("Version", applicationVersion());
    settings.setValue("path", "E:/repos/qt_project/LabQ/GnuplotEditor/src");
    settings.endGroup();
    settings.sync();

    qDebug() << __FILE__ << __LINE__ << "saved";
}

QString Settings::applicationVersion()
{
    return "1.0.00";
}

QString Settings::applicationName()
{
    return "GnuplotEditor";
}

QString Settings::applicationSettingsDirPath()
{
    return QApplication::applicationDirPath() + "/msettings";
}

QString Settings::iniFilePath()
{
    return applicationSettingsDirPath() + "/settings.ini";
}

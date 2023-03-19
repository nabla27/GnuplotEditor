/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QObject>


class SettingController : public QObject
{
    Q_OBJECT
public:
    explicit SettingController(QObject *parent = nullptr);

public slots:
    void loadSettings();
    void saveSettings();
    void moveToGlobalThread();

signals:
    void settingsLoaded();
    void settingsSaved();

    void loadRequested();
    void saveRequested();
};





class Settings : public QObject
{
public:
    static void setup();
    static void save();

    static QString applicationName();
    static QString applicationVersion();
    static QString applicationSettingsDirPath();
    static QString iniFilePath();
};

#endif // SETTINGS_H

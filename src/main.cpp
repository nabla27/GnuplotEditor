/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "gnuploteditor.h"
#include <QApplication>
#include <QObject>
#include <QThread>
#include <QThreadPool>
#include "settings.h"
#include "layoutparts.h"



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    GnuplotEditor window;

    SettingController sController;
    QThread thread;
    sController.moveToThread(&thread);
    thread.start();

    mlayout::LoopProgressDialog loopDialog;
    loopDialog.setAutoDisplay(true);
    loopDialog.setMessage("loading settings ...");;

    QObject::connect(&app, &QApplication::focusChanged, &window, &GnuplotEditor::setCurrentItem);
    QObject::connect(&sController, &SettingController::settingsLoaded, &window, &GnuplotEditor::show);
    QObject::connect(&sController, &SettingController::settingsLoaded, &sController, &SettingController::moveToGlobalThread);
    QObject::connect(&sController, &SettingController::settingsLoaded, &loopDialog, &mlayout::LoopProgressDialog::stop);
    QObject::connect(&sController, &SettingController::settingsLoaded, &thread, &QThread::quit);
    QObject::connect(&app, &QApplication::aboutToQuit, &sController, &SettingController::saveSettings);


    loopDialog.start();
    emit sController.loadRequested(); //thread safe

    return app.exec();
}

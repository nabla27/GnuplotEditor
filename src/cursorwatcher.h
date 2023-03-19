/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef CURSORWATCHER_H
#define CURSORWATCHER_H
#include <QObject>
#include <QApplication>

class CursorWatcher : public QObject
{
    Q_OBJECT
    explicit CursorWatcher(QObject *parent) : QObject(parent) {}

private:
    void test()
    {
        QApplication::cursorFlashTime();
    }
};


#endif // CURSORWATCHER_H

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

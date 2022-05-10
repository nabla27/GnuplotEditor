#include "gnuploteditor.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QString oldApp;

    if(argc == 3)
    {
        if(QString(argv[1]) == "-updated")
        {
            oldApp = QString(argv[2]);
        }
    }

    QApplication a(argc, argv);
    GnuplotEditor w(oldApp, nullptr);
    w.show();
    return a.exec();
}

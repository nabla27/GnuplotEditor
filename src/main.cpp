#include "gnuploteditor.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    if(argc == 3)
    {
        if(QString(argv[1]) == "-updated")
        {
            QThread::sleep(1000);
            QDir oldApp(argv[2]);
            oldApp.removeRecursively();
        }
    }

    QApplication a(argc, argv);
    GnuplotEditor w;
    w.show();
    return a.exec();
}

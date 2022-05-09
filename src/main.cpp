#include "gnuploteditor.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    if(argc == 3)
    {
        if(QString(argv[1]) == "-updated")
        {
            QThread::sleep(2);
            const QString oldAppPath(argv[2]);
            QDir oldAppDir(oldAppPath);
            oldAppDir.removeRecursively();
            oldAppDir.rmdir(oldAppPath);
        }
    }

    QApplication a(argc, argv);
    GnuplotEditor w;
    w.show();
    return a.exec();
}

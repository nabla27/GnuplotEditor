#include "gnuploteditor.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    if(argc == 3)
    {
        if(QString(argv[1]) == "-updated")
        {
            QThread::sleep(1);
            QDir oldApp(QString(argv[2]).first(QString(argv[2]).lastIndexOf('/')));
            oldApp.rmdir(argv[2]);
        }
    }

    QApplication a(argc, argv);
    GnuplotEditor w;
    w.show();
    return a.exec();
}

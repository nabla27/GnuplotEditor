#include "gnuploteditor.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    if(argc == 2)
    {
        if(QString(argv[0]) == "-updated")
        {
            QFile::remove(argv[1]);
        }
    }

    QApplication a(argc, argv);
    GnuplotEditor w;
    w.show();
    return a.exec();
}

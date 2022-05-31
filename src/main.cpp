#include "gnuploteditor.h"

#include <QApplication>

#include "filetreewidget.h" //DEBUG

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GnuplotEditor w;
    w.show();

    return a.exec();
}

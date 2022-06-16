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




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //ExcelDocument doc("test");
    //doc.readXml();

    GnuplotEditor w;
    w.show();

    return a.exec();

    return 0;
}

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


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GnuplotEditor w;
    w.show();

    QObject::connect(&a, &QApplication::focusChanged, &w, &GnuplotEditor::setCurrentItem);

    return a.exec();

    return 0;
}

#include <QCoreApplication>
#include <QTimer>
#include "checker.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    PluginChecker checker(a.arguments(), nullptr);

    QTimer::singleShot(0, &checker, &PluginChecker::check);

    return a.exec();
}

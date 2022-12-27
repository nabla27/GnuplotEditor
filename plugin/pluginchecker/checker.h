#ifndef CHECKER_H
#define CHECKER_H

#include <QObject>
#include <QCoreApplication>
#include <QFile>
#include <QLibrary>
#include "../plugin_base.h"
#include <iostream>

class PluginChecker : public QObject
{
    Q_OBJECT
public:
    PluginChecker(const QStringList& arguments, QObject *parent)
        : QObject(parent)
        , arguments(arguments) {}

    enum class ExitCode { ValidLibrary,
                          InvalidArguments,
                          NotFound,
                          FailedToLoad,
                          FailedToResolve,
                          FailedToCreateInstance,
                          InvalidLibrary };

    typedef void(*FuncPtr)(AbstractPlugin*&);

public slots:
    void check()
    {
        std::cout << "about to check ..." << std::endl;

        if(arguments.count() != 3)
        {
            std::cout << "arguments count must be 3." << std::endl;
            QCoreApplication::exit((int)ExitCode::InvalidArguments);
            return;
        }

        const QString libPath = arguments.at(1);
        const QString symbolName = arguments.at(2);

        QFile file(libPath);
        if(!file.exists())
        {
            std::cout << "file \"" << libPath.toStdString() << "\" was not found.";
            QCoreApplication::exit((int)ExitCode::NotFound);
            return;
        }

        QLibrary lib(libPath, this);

        if(!lib.load())
        {
            std::cout << "failed to load the library" << std::endl;
            QCoreApplication::exit((int)ExitCode::FailedToLoad);
            return;
        }

        FuncPtr func = (FuncPtr)(lib.resolve(symbolName.toUtf8().constData()));

        if(!func)
        {
            std::cout << "resolved function pointer is nullptr." << std::endl;
            QCoreApplication::exit((int)ExitCode::FailedToResolve);
            return;
        }

        AbstractPlugin *p = nullptr;
        func(p);

        if(!p)
        {
            std::cout << "created instance pointer is nullptr." << std::endl;
            QCoreApplication::exit((int)ExitCode::FailedToCreateInstance);
            return;
        }

        PluginInfo info;

        try
        {
            p->info(info);
        }
        catch(...)
        {

        }

        std::cout << "finished to check the library." << std::endl;
        QCoreApplication::exit((int)ExitCode::ValidLibrary);
    }

private:
    QStringList arguments;
};

#endif // CHECKER_H

#ifndef CPL_GNUPLOT_H
#define CPL_GNUPLOT_H

#include <QCompleter>
#include <QStringListModel>


void changeGnuplotCompleter(QCompleter* completer, const QString& first, const QString& current);

inline QAbstractItemModel* getCompleter(const QStringList& strlist) {
    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(strlist);

    return listModel;
}

QAbstractItemModel* getEditCompleter_non();

QAbstractItemModel* getEditCompleter_first();
QAbstractItemModel* getEditCompleter_Cwith();
QAbstractItemModel* getEditCompleter_Clinecolor();
QAbstractItemModel* getEditCompleter_Fplot();

QAbstractItemModel* getEditCompleter_Csmooth();
QAbstractItemModel* getEditCompleter_Cstyle();
QAbstractItemModel* getEditCompleter_Cclip();
QAbstractItemModel* getEditCompleter_Cangles();
QAbstractItemModel* getEditCompleter_Carrow();
QAbstractItemModel* getEditCompleter_Cc3();
QAbstractItemModel* getEditCompleter_Cc2();
QAbstractItemModel* getEditCompleter_Cc1();
QAbstractItemModel* getEditCompleter_Cr();
QAbstractItemModel* getEditCompleter_Cterminal();
QAbstractItemModel* getEditCompleter_Ccntrparam();
QAbstractItemModel* getEditCompleter_Ccontour();
QAbstractItemModel* getEditCompleter_Cmapping();
QAbstractItemModel* getEditCompleter_Cformat();
QAbstractItemModel* getEditCompleter_Cdatafile();

QAbstractItemModel* getEditCompleter_Foption();


#endif // CPL_GNUPLOT_H

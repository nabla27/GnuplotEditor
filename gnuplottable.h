#ifndef TableWidget_H
#define TableWidget_H

#include <QTableWidget>
#include <QProcess>
#include <QShortcut>
#include <QMenu>
#include <QApplication>
#include <QClipboard>
#include "gnuplot.h"

class GnuplotTable : public QTableWidget
{
public:
    GnuplotTable(QWidget *parent = nullptr);
    ~GnuplotTable();

public:
    template <class T> void setData(const QList<QList<T> >& data);
    template <class T> QList<QList<T> > getData() const;
    template <class T> QList<QList<QList<T> > > selectedData() const;
    void setGnuplot(Gnuplot *gnuplot) { this->gnuplot = gnuplot; }

public slots:
    void appendLineRow() { insertRow(rowCount()); }
    void removeLineRow() { removeRow(rowCount() - 1); }
    void appendLineCol() { insertColumn(columnCount()); }
    void removeLineCol() { removeColumn(columnCount() - 1); }

private:
    QString createPlotCmd(const QString& sheetName);
    QString optionCmd;
    QMenu *normalMenu;
    QProcess *process;
    QShortcut *scCtrC;
    QShortcut *scCtrV;
    QShortcut *scCtrX;
    Gnuplot *gnuplot;

private slots:
    void onCustomContextMenu(const QPoint& point);
    void copyCell();
    void cutCell();
    void pasteCell();
    void deleteCell();
    void insertRowUp();
    void insertRowDown();
    void insertColLeft();
    void insertColRight();
    void reverseRow();
    void reverseCol();
    void plot();
    void gnuplotClip();
    void visualize3DBar();
    void visualize3DScatter();
    void toLatexCode();
};

#endif // TableWidget_H

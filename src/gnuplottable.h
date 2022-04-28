#ifndef TableWidget_H
#define TableWidget_H

#include <QTableWidget>
#include <QProcess>
#include <QShortcut>
#include <QMenu>
#include <QApplication>
#include <QClipboard>
#include "gnuplot.h"
#include "tablewidget.h"





class GnuplotTable : public TableWidget
{
public:
    GnuplotTable(QWidget *parent = nullptr);
    ~GnuplotTable();

public slots:
    void appendLineRow() { insertRow(rowCount()); }
    void removeLineRow() { removeRow(rowCount() - 1); }
    void appendLineCol() { insertColumn(columnCount()); }
    void removeLineCol() { removeColumn(columnCount() - 1); }
    void setGnuplot(Gnuplot *gnuplot) { this->gnuplot = gnuplot; }

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onCustomContextMenu(const QPoint& point);
    void plot();
    void gnuplotClip();
    void toLatexCode();

private:
    void initializeContextMenu();
    QPoint startDragPoint;
    QPoint startDragTableSize;
    QPoint startDragCellSize;

private:
    QMenu *normalMenu = nullptr;
    QShortcut *scCtrC = nullptr;
    QShortcut *scCtrV = nullptr;
    QShortcut *scCtrX = nullptr;

    Gnuplot *gnuplot = nullptr;
    QProcess *process = nullptr;
    QString optionCmd;
};

#endif // TableWidget_H

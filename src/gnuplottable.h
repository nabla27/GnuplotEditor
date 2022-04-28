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
    QPoint startDragPoint = QPoint(-1, -1); //mouseEvent関係の関数内の無効値(-1,-1)と合わせる。初期化しなければ(0,0)に戻って特定の操作でクラッシュする
    QPoint startDragTableSize;
    QPoint startDragCellSize;

private:
    QMenu *normalMenu = nullptr;

    Gnuplot *gnuplot = nullptr;
    QProcess *process = nullptr;
    QString optionCmd;
};

#endif // TableWidget_H

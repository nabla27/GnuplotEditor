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
    void mousePressEvent(QMouseEvent *event) override
    {
        const int cursorX = event->pos().x();
        const int cursorY = event->pos().y();

        const int rowLastIndex = rowCount() - 1;
        const int colLastIndex = columnCount() - 1;

        /* 一番右下のセルの右下隅の座標(tableWidgetのローカル座標) */
        const int tableWidth = columnViewportPosition(colLastIndex) + columnWidth(colLastIndex);
        const int tableHeight = rowViewportPosition(rowLastIndex) + rowHeight(rowLastIndex);

        if(std::abs(cursorX - tableWidth) < 4 && std::abs(cursorY - tableHeight) < 4)
        {
            startDragPoint = event->pos();
            startDragTableSize = QPoint(columnCount(), rowCount());
            startDragCellSize = QPoint(columnWidth(colLastIndex), rowHeight(rowLastIndex));
        }

        TableWidget::mousePressEvent(event);
    }
    void mouseMoveEvent(QMouseEvent *event) override
    {
        if(startDragPoint == QPoint(-1, -1)) return;

        setCursor(Qt::ClosedHandCursor);

        /* 追加・削除分のセル数 */
        const int dx = (event->pos().x() - startDragPoint.x()) / startDragCellSize.x();
        const int dy = (event->pos().y() - startDragPoint.y()) / startDragCellSize.y();

        /* 変更後のセル数 */
        const int nextCol = startDragTableSize.x() + dx;
        const int nextRow = startDragTableSize.y() + dy;

        /* セルが1x1以下に減らないようにする */
        if(nextCol > 0)
            setColumnCount(startDragTableSize.x() + dx);
        if(nextRow > 0)
        setRowCount(startDragTableSize.y() + dy);

        TableWidget::mouseMoveEvent(event);
    }
    void mouseReleaseEvent(QMouseEvent *event) override
    {
        startDragPoint = QPoint(-1, -1);

        TableWidget::mouseReleaseEvent(event);
        setCursor(Qt::ArrowCursor);
    }



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

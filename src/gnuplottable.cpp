/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */


#include "gnuplottable.h"

#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include "gnuplot.h"
#include "logger.h"



GnuplotTable::GnuplotTable(QWidget *parent)
    : TableWidget(parent)
{
    /* contextMenu初期化 */
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &GnuplotTable::customContextMenuRequested, this, &GnuplotTable::onCustomContextMenu);
    initializeContextMenu();
}

GnuplotTable::~GnuplotTable()
{

}



/* 以下のマウスイベントでドラッグでセル数を変更できるようにする。
 * ドラッグ開始時のカーソル位置とセル数、セル幅を記憶しておき、カーソル位置の差分に応じて変更する。*/
void GnuplotTable::mousePressEvent(QMouseEvent *event)
{
    TableWidget::mousePressEvent(event);

    const int cursorX = event->pos().x();
    const int cursorY = event->pos().y();

    /* 一番右下のセルの右下隅の座標(tableWidgetのローカル座標) */
    const int rowLastIndex = rowCount() - 1;
    const int colLastIndex = columnCount() - 1;
    const int tableWidth = (rowLastIndex < 0) ? 0 : columnViewportPosition(colLastIndex) + columnWidth(colLastIndex);
    const int tableHeight = (colLastIndex < 0) ? 0 : rowViewportPosition(rowLastIndex) + rowHeight(rowLastIndex);

    /* セル右下角とカーソル位置が4ピクセル以下でマウスが押された場合に有効 */
    if(std::abs(cursorX - tableWidth) < 4 && std::abs(cursorY - tableHeight) < 4)
    {
        startDragPoint = event->pos();
        startDragTableSize = QPoint(columnCount(), rowCount());
        if(rowLastIndex < 0 || colLastIndex < 0)
            startDragCellSize = QPoint(100, 30);  //セルがない場合は、デフォルトの(100,30)でセルサイズを指定
        else
            startDragCellSize = QPoint(columnWidth(colLastIndex), rowHeight(rowLastIndex)); //右端のセルのサイズを追加するセルのサイズにする
    }
}

void GnuplotTable::mouseMoveEvent(QMouseEvent *event)
{
    TableWidget::mouseMoveEvent(event);

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
}

void GnuplotTable::mouseReleaseEvent(QMouseEvent *event)
{
    TableWidget::mouseReleaseEvent(event);

    startDragPoint = QPoint(-1, -1);

    setCursor(Qt::ArrowCursor);
}



void GnuplotTable::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key::Key_Backspace)
    {
        if(QTableWidgetItem *item = currentItem()) item->setText("");
    }

    QTableWidget::keyPressEvent(event);
}






void GnuplotTable::onCustomContextMenu(const QPoint& point)
{
    normalMenu->exec(mapToGlobal(point));
}

void GnuplotTable::initializeContextMenu()
{
    if(normalMenu) delete normalMenu;

    normalMenu = new QMenu(this);

    /* copy */
    QAction *actCopy = new QAction("copy", normalMenu);
    normalMenu->addAction(actCopy);
    connect(actCopy, &QAction::triggered, this, &GnuplotTable::copyCell);

    /* cut */
    QAction *actCut = new QAction("cut", normalMenu);
    normalMenu->addAction(actCut);
    connect(actCut, &QAction::triggered, this, &GnuplotTable::cutCell);

    /* paste */
    QAction *actPaste = new QAction("paste", normalMenu);
    normalMenu->addAction(actPaste);
    connect(actPaste, &QAction::triggered, this, &GnuplotTable::pasteCell);

    /* claer */
    QAction *actClear = new QAction("clear", normalMenu);
    normalMenu->addAction(actClear);
    connect(actClear, &QAction::triggered, this, &GnuplotTable::clearCell);

    /* delete */
    QAction *actDelete = new QAction("delete", normalMenu);
    normalMenu->addAction(actDelete);
    connect(actDelete, &QAction::triggered, this, &GnuplotTable::deleteCell);

    /* insert */
    QMenu *insertMenu = new QMenu(normalMenu);
    insertMenu->setTitle("insert");
    QAction *actInsertRowUp = new QAction("row (up)", insertMenu);
    QAction *actInsertRowDw = new QAction("row (down)", insertMenu);
    QAction *actInsertColLf = new QAction("col (left)", insertMenu);
    QAction *actInsertColRg = new QAction("col (right)", insertMenu);
    insertMenu->addAction(actInsertRowUp);
    insertMenu->addAction(actInsertRowDw);
    insertMenu->addAction(actInsertColLf);
    insertMenu->addAction(actInsertColRg);
    connect(actInsertRowUp, &QAction::triggered, this, &GnuplotTable::insertRowUp);
    connect(actInsertRowDw, &QAction::triggered, this, &GnuplotTable::insertRowDown);
    connect(actInsertColLf, &QAction::triggered, this, &GnuplotTable::insertColLeft);
    connect(actInsertColRg, &QAction::triggered, this, &GnuplotTable::insertColRight);
    normalMenu->addMenu(insertMenu);

    /* placement */
    QMenu *placementMenu = new QMenu(normalMenu);
    placementMenu->setTitle("placement");
    QAction *actReverseRow = new QAction("reverse row", placementMenu);
    QAction *actReverseCol = new QAction("reverse col", placementMenu);
    QAction *actTranspose = new QAction("transpose", placementMenu);
    QAction *actSortAscending = new QAction("sort ascending order", placementMenu);
    QAction *actSortDescending = new QAction("sort descening order", placementMenu);
    placementMenu->addAction(actReverseRow);
    placementMenu->addAction(actReverseCol);
    placementMenu->addAction(actTranspose);
    placementMenu->addAction(actSortAscending);
    placementMenu->addAction(actSortDescending);
    connect(actReverseRow, &QAction::triggered, this, &GnuplotTable::reverseRow);
    connect(actReverseCol, &QAction::triggered, this, &GnuplotTable::reverseCol);
    connect(actTranspose, &QAction::triggered, this, &GnuplotTable::transposeCell);
    connect(actSortAscending, &QAction::triggered, this, &GnuplotTable::sortAscending);
    connect(actSortDescending, &QAction::triggered, this, &GnuplotTable::sortDescending);
    normalMenu->addMenu(placementMenu);

    /* gnuplot */
    QMenu *gnuplotMenu = new QMenu(normalMenu);
    gnuplotMenu->setTitle("gnuplot");
    QAction *actPlot = new QAction("plot", gnuplotMenu);
    QAction *actPlotClip = new QAction("clipboard", gnuplotMenu);
    gnuplotMenu->addAction(actPlot);
    gnuplotMenu->addAction(actPlotClip);
    connect(actPlot, &QAction::triggered, [this](){ plotSelectedData(PlotType::Custom); });
    connect(actPlotClip, &QAction::triggered, this, &GnuplotTable::gnuplotClip);
    normalMenu->addMenu(gnuplotMenu);

    /* export */
    QMenu *exportMenu = new QMenu(normalMenu);
    exportMenu->setTitle("export");
    QAction *actLatexCode = new QAction("latex code", exportMenu);
    exportMenu->addAction(actLatexCode);
    connect(actLatexCode, &QAction::triggered, this, &GnuplotTable::toLatexCode);
    normalMenu->addMenu(exportMenu);
}

void GnuplotTable::plotSelectedData(const GnuplotTable::PlotType &plotType)
{
    QList<std::array<int, 3>> ranges;

    for(const QTableWidgetSelectionRange& range : selectedRanges())
    {
        for(int c = range.leftColumn(); c <= range.rightColumn(); ++c)
        {
            ranges.append({ c, range.topRow(), range.bottomRow() });
        }
    }

    QString cmd;

    switch(plotType)
    {
    case PlotType::Scatter2D:
        cmd = "plot \"-\" with points"; break;
    case PlotType::Lines2D:
        cmd = "plot \"-\" with lines"; break;
    case PlotType::LinesPoints2D:
        cmd = "plot \"-\" with linespoints"; break;

    case PlotType::Boxes2D:
        cmd = "plot \"-\" with boxes"; break;
    case PlotType::Steps2D:
        cmd = "plot \"-\" with steps"; break;
    case PlotType::FSteps2D:
        cmd = "plot \"-\" with fsteps"; break;
    case PlotType::FillSteps2D:
        cmd = "plot \"-\" with fillsteps"; break;
    case PlotType::HiSteps2D:
        cmd = "plot \"-\" with histeps"; break;
    case PlotType::Impulses2D:
        cmd = "plot \"-\" with impulses"; break;

    case PlotType::Custom:
        cmd = optionCmd; break;
    default:
        return;
    }

    cmd += "\n";

    plotCellPoints(ranges, cmd);
}

void GnuplotTable::plotCellPoints(const QList<std::array<int, 3> > &ranges, const QString& _cmd)
{
    QString cmd = _cmd;

    const int colCount = ranges.size();

    if(colCount < 1) return;

    const int rowCount = ranges.at(0).at(2) - ranges.at(0).at(1) + 1;

    for(int i = 0; i < rowCount; ++i)
    {
        for(int j = 0; j < colCount; ++j)
        {
            const int column = ranges.at(j).at(0);
            const int topRow = ranges.at(j).at(1);

            if(QTableWidgetItem *item = this->item(topRow + i, column))
            {
                cmd += item->text();
            }
            else
                cmd += "0";

            if(j == colCount - 1)
                cmd += "\n";
            else
                cmd += ", "; //カンマの後に空白がないと正しくプロットされない
        }
    }

    cmd += "e\n";

    __LOGOUT__("execute cell data requested.", Logger::LogLevel::Info);

    gnuplotExecutor->execGnuplot(QStringList() << cmd, false);
}

void GnuplotTable::gnuplotClip()
{
    /* 選択された範囲 */
    const QList<QTableWidgetSelectionRange> selectedRangeList = selectedRanges();
    if(selectedRangeList.size() < 1) { return; }
    const int startBlockNumber = 0;
    const int endBlockNumber = 0;
    const int startRowNumber = selectedRangeList.at(0).topRow();
    const int endRowNumber = selectedRangeList.at(0).bottomRow();
    const int startCol = selectedRangeList.at(0).leftColumn();
    const int endCol = selectedRangeList.at(0).rightColumn();

    /* 範囲のコマンドを生成 */
    QString cmd = "using ";
    if(endCol - startCol < 0) return;
    else if(endCol - startCol == 0) cmd += QString::number(startCol + 1);
    else if(endCol - startCol == 1) cmd += QString::number(startCol + 1) + ":" + QString::number(startCol + 2);
    else if(endCol - startCol == 2) cmd += QString::number(startCol + 1) + ":" + QString::number(startCol + 2) + ":" + QString::number(startCol + 3);
    else return;

    cmd += " every 1:1:" +
           QString::number(startRowNumber) + ":" +
           QString::number(startBlockNumber) + ":" +
           QString::number(endRowNumber) + ":" +
           QString::number(endBlockNumber);

    /* クリップボードに貼り付け */
    QApplication::clipboard()->setText(cmd);

    __LOGOUT__("plot range pasted to the clipboard.", Logger::LogLevel::Info);
}

void GnuplotTable::toLatexCode()
{
    /* 選択された範囲を取得 */
    const QList<QTableWidgetSelectionRange> selectedRangeList = selectedRanges();

    /* クリップボードに貼り付ける文字列 */
    QString clip;

    /* コードの生成 */
    for(const QTableWidgetSelectionRange& selected : selectedRangeList)
    {
        const int startRow = selected.topRow();
        const int startCol = selected.leftColumn();
        const int endRow = selected.bottomRow();
        const int endCol = selected.rightColumn();

        clip += "\\begin{table}[h]\n";
        clip += "\t\\centering\n";
        clip += "\t\\caption{}\n";
        clip += "\t\\begin{tabular}{|";
        for(int i = 0; i <= endCol - startCol; ++i)
            clip += "c|";
        clip += "}\n";
        clip += "\t\t\\hline\n";
        for(int row = startRow; row <= endRow; ++row){
            clip += "\t\t";
            for(int col = startCol; col <= endCol; ++col)
            {
                if(col != endCol)
                {
                    if(QTableWidgetItem *item = this->item(row, col))
                        clip += item->text() + " & ";
                    else
                        clip += " & ";
                }
                else
                {
                    if(QTableWidgetItem *item = this->item(row, col))
                        clip += item->text() + " \\\\\n";
                    else
                        clip += " \\\\\n";
                }
            }
        }
        clip += "\t\t\\hline\n";
        clip += "\t\\end{tabular}\n";
        clip += "\t\\label{}\n";
        clip += "\\end{table}\n";

        clip += "\n";
    }

    /* クリップボードに貼り付け */
    QApplication::clipboard()->setText(clip);

    __LOGOUT__("latex table cmd pasted to the clipboard.", Logger::LogLevel::Info);
}









































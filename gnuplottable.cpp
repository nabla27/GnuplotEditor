#include "gnuplottable.h"


GnuplotTable::GnuplotTable(QWidget *parent)
    : TableWidget(parent)
{
    process = new QProcess(this);

    scCtrC = new QShortcut(QKeySequence("Ctrl+C"), this);
    scCtrV = new QShortcut(QKeySequence("Ctrl+V"), this);
    scCtrX = new QShortcut(QKeySequence("Ctrl+X"), this);
    connect(scCtrC, &QShortcut::activated, this, &GnuplotTable::copyCell);
    connect(scCtrV, &QShortcut::activated, this, &GnuplotTable::pasteCell);
    connect(scCtrX, &QShortcut::activated, this, &GnuplotTable::cutCell);

    /* contextMenu初期化 */
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &GnuplotTable::customContextMenuRequested, this, &GnuplotTable::onCustomContextMenu);
    initializeContextMenu();
}

GnuplotTable::~GnuplotTable()
{
    delete normalMenu;
    delete scCtrC;
    delete scCtrV;
    delete scCtrX;

    process->close();
    delete process;
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
    QAction *actDelete = new QAction("delete", normalMenu);
    normalMenu->addAction(actDelete);
    connect(actDelete, &QAction::triggered, this, &GnuplotTable::clearCell);

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
    placementMenu->addAction(actReverseRow);
    placementMenu->addAction(actReverseCol);
    placementMenu->addAction(actTranspose);
    connect(actReverseRow, &QAction::triggered, this, &GnuplotTable::reverseRow);
    connect(actReverseCol, &QAction::triggered, this, &GnuplotTable::reverseCol);
    connect(actTranspose, &QAction::triggered, this, &GnuplotTable::transposeCell);
    normalMenu->addMenu(placementMenu);

    /* gnuplot */
    QMenu *gnuplotMenu = new QMenu(normalMenu);
    gnuplotMenu->setTitle("gnuplot");
    QAction *actPlot = new QAction("plot", gnuplotMenu);
    QAction *actPlotClip = new QAction("clipboard", gnuplotMenu);
    gnuplotMenu->addAction(actPlot);
    gnuplotMenu->addAction(actPlotClip);
    connect(actPlot, &QAction::triggered, this, &GnuplotTable::plot);
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

void GnuplotTable::plot()
{
    /* gnuplotが初期化されていなかったら(=nullptr)無効 */
    if(gnuplot == nullptr) return;

    /* 選択された範囲を取得 */
    const QList<QTableWidgetSelectionRange> selectedRangeList = selectedRanges();
    if(selectedRangeList.size() < 1) { return; }

    /* 次元数 */
    const int dim = selectedRangeList.at(0).rightColumn() - selectedRangeList.at(0).leftColumn() + 1;

    /* plotするコマンド */
    QString cmd;
    if(dim < 1) return;
    else if(dim == 1 || dim == 2) cmd += "plot ";
    else if(dim == 3) cmd += "splot ";
    else return;
    cmd += "\"-\" " + optionCmd + "\n";

    /* セルの選択範囲の値を参照しながらコマンドを生成 */
    for(const QTableWidgetSelectionRange& selected : selectedRangeList)
    {
        const int startRow = selected.topRow();
        const int startCol = selected.leftColumn();
        const int endRow = selected.bottomRow();
        const int endCol = selected.rightColumn();

        int index = 0;
        for(int row = startRow; row <= endRow; ++row)
        {
            if(dim == 1) cmd += QString::number(index) + ", ";
            for(int col = startCol; col <= endCol; ++col)
            {
                if(item(row, col) == nullptr)
                    cmd += "0, ";
                else
                    cmd += item(row, col)->text() + ", ";
            }
            cmd += "\n";
        }
        cmd += "\n";
    }
    cmd += "e\n";

    /* plotする */
    gnuplot->exc(process, QList<QString>() << "reset\n" << cmd);
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
                    clip += this->item(row, col)->text() + " & ";
                else
                    clip += this->item(row, col)->text() + " \\\\\n";
            }
        }
        clip += "\t\t\\hline\n";
        clip += "\t\\end{tabular}\n";
        clip += "\t\\caption{}\n";
        clip += "\t\\label{}\n";
        clip += "\\end{table}\n";

        clip += "\n";
    }

    /* クリップボードに貼り付け */
    QApplication::clipboard()->setText(clip);
}









































#include "tablewidget.h"

TableWidget::TableWidget(QWidget *parent)
    : QTableWidget(parent)
{
    {//プロセスの初期化
        process = new QProcess(this);
    }
    {//ショートカットキーの登録
        scCtrC = new QShortcut(QKeySequence("Ctrl+C"), this);
        scCtrV = new QShortcut(QKeySequence("Ctrl+V"), this);
        scCtrX = new QShortcut(QKeySequence("Ctrl+X"), this);
        connect(scCtrC, &QShortcut::activated, this, &TableWidget::copyCell);
        connect(scCtrV, &QShortcut::activated, this, &TableWidget::pasteCell);
        connect(scCtrX, &QShortcut::activated, this, &TableWidget::cutCell);
    }
    {//contextMenuの設定
        setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, &TableWidget::customContextMenuRequested,
                this, &TableWidget::onCustomContextMenu);
        normalMenu = new QMenu(this);
        /* copy */
        QAction *actCopy = new QAction("copy", normalMenu);
        normalMenu->addAction(actCopy);
        connect(actCopy, &QAction::triggered, this, &TableWidget::copyCell);
        /* cut */
        QAction *actCut = new QAction("cut", normalMenu);
        normalMenu->addAction(actCut);
        connect(actCut, &QAction::triggered, this, &TableWidget::cutCell);
        /* paste */
        QAction *actPaste = new QAction("paste", normalMenu);
        normalMenu->addAction(actPaste);
        connect(actPaste, &QAction::triggered, this, &TableWidget::pasteCell);
        /* delete */
        QAction *actDelete = new QAction("delete", normalMenu);
        normalMenu->addAction(actDelete);
        connect(actDelete, &QAction::triggered, this, &TableWidget::deleteCell);
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
        connect(actInsertRowUp, &QAction::triggered, this, &TableWidget::insertRowUp);
        connect(actInsertRowDw, &QAction::triggered, this, &TableWidget::insertRowDown);
        connect(actInsertColLf, &QAction::triggered, this, &TableWidget::insertColLeft);
        connect(actInsertColRg, &QAction::triggered, this, &TableWidget::insertColRight);
        normalMenu->addMenu(insertMenu);
        /* placement */
        QMenu *placementMenu = new QMenu(normalMenu);
        placementMenu->setTitle("placement");
        QAction *actReverseRow = new QAction("reverse row", placementMenu);
        QAction *actReverseCol = new QAction("reverse col", placementMenu);
        placementMenu->addAction(actReverseRow);
        placementMenu->addAction(actReverseCol);
        connect(actReverseRow, &QAction::triggered, this, &TableWidget::reverseRow);
        connect(actReverseCol, &QAction::triggered, this, &TableWidget::reverseCol);
        normalMenu->addMenu(placementMenu);
        /* gnuplot */
        QMenu *gnuplotMenu = new QMenu(normalMenu);
        gnuplotMenu->setTitle("gnuplot");
        QAction *actPlot = new QAction("plot", gnuplotMenu);
        QAction *actPlotClip = new QAction("clipboard", gnuplotMenu);
        gnuplotMenu->addAction(actPlot);
        gnuplotMenu->addAction(actPlotClip);
        connect(actPlot, &QAction::triggered, this, &TableWidget::plot);
        connect(actPlotClip, &QAction::triggered, this, &TableWidget::gnuplotClip);
        normalMenu->addMenu(gnuplotMenu);
        /* visualize */
        QMenu *visualizeMenu = new QMenu(normalMenu);
        visualizeMenu->setTitle("visualize");
        QAction *act3DBar = new QAction("3Dbar");
        QAction *act3DScatter = new QAction("3Dscatter");
        visualizeMenu->addAction(act3DBar);
        visualizeMenu->addAction(act3DScatter);
        connect(act3DBar, &QAction::triggered, this, &TableWidget::visualize3DBar);
        connect(act3DScatter, &QAction::triggered, this, &TableWidget::visualize3DScatter);
        /* export */
        QMenu *exportMenu = new QMenu(normalMenu);
        exportMenu->setTitle("export");
        QAction *actLatexCode = new QAction("latex code", exportMenu);
        exportMenu->addAction(actLatexCode);
        connect(actLatexCode, &QAction::triggered, this, &TableWidget::toLatexCode);
        normalMenu->addMenu(exportMenu);
    }
}

TableWidget::~TableWidget()
{
    delete process;
    delete normalMenu;
    delete scCtrC;
    delete scCtrV;
    delete scCtrX;
}



//######################
// データのセットと取得
//######################
/* tableにデータをセット */
template <>
void TableWidget::setData(const QList<QList<QString> >& data)
{
    this->clear();                                                               //セットされていた*itemはdeleteされる
    for(int row = 0; row < data.size(); ++row)
    {
        if(row >= rowCount()) { insertRow(rowCount()); }                         //行が足りなかったら足す
        for(int col = 0; col < data.at(row).size(); ++col)
        {
            if(col >= columnCount()) { insertColumn(columnCount()); }            //列が足りなかったら足す
            QTableWidgetItem *item = new QTableWidgetItem(data.at(row).at(col));
            setItem(row, col, item);                                             //アイテムをセット
        }
    }
}

/* table全体のデータを取得 */
template <>
QList<QList<QString> > TableWidget::getData() const
{
    const int rows = rowCount();
    const int cols = columnCount();
    QList<QList<QString> > data(rows, QList<QString>(cols));

    for(int row = 0; row < rows; ++row){
        for(int col = 0; col < cols; ++col){
            //item()はQTableWidgetItem*がセットされていなかったり、インデックスが範囲外であればエラー(nullptr)
            data[row][col] = (item(row, col) != nullptr) ? item(row, col)->text() : "";
        }
    }

    return data;
}

/* tableの選択されているデータを取得 */
template<>
QList<QList<QList<float> > > TableWidget::selectedData() const
{
    /* 選択されれるセルの範囲を取得 */
    const QList<QTableWidgetSelectionRange> selectedRangeList = selectedRanges();
    /* 戻り値 */
    QList<QList<QList<float> > > list(selectedRangeList.size());

    /* 選択された範囲をループ */
    qsizetype index = 0;
    for(const QTableWidgetSelectionRange& selected : selectedRangeList)
    {
        const int startRow = selected.topRow();
        const int startCol = selected.leftColumn();
        const int endRow = selected.bottomRow();
        const int endCol = selected.rightColumn();

        list[index] = QList<QList<float> >(endRow - startRow + 1, QList<float>(endCol - startCol + 1));

        for(int row = startRow; row <= endRow; ++row){
            for(int col = startCol; col <= endCol; ++col){
                list[index][row - startRow][col - startCol] =
                        (item(row, col) != nullptr) ? item(row, col)->text().toFloat() : NULL;
            }
        }
        index++;
    }

    return list;
}




//####################
// スロット関数
//####################
/* 右クリックによるcontextMenuの表示 */
void TableWidget::onCustomContextMenu(const QPoint& point)
{
    normalMenu->exec(viewport()->mapToGlobal(point));
}

/* コピー */
void TableWidget::copyCell()
{
    /* tableの情報を取得 */
    const QAbstractItemModel *model = this->model();                //table全体の値の情報
    const QItemSelectionModel *selection = this->selectionModel();  //選択された部分の情報

    QModelIndexList indexes = selection->selectedIndexes();         //選択されたセルのインデックス

    if(indexes.count() == 0) { return; }                            //選択されたインデックスがなければ何もしない

    std::sort(indexes.begin(), indexes.end());                      //並び替え

    QString clip;
    QModelIndex previous = indexes.first();
    indexes.removeFirst();
    clip.append(model->data(previous).toString());

    foreach(const QModelIndex &current, indexes)                    //行、列と捜査していく
    {
        if(current.row() != previous.row())
            clip.append("\n");
        else
            clip.append("\t");                                      //列の区切りはタブ幅にする

        clip.append(model->data(current).toString());
        previous = current;
    }

    QApplication::clipboard()->setText(clip);                       //クリップボードに貼り付ける
}

/* カット */
void TableWidget::cutCell()
{
    this->copyCell();
    this->deleteCell();
}

/* ペースト */
void TableWidget::pasteCell()
{
    /* 選択された行と列のインデックスを取得 */
    const QList<QTableWidgetSelectionRange> selectedRangeList = selectedRanges();

    /* クリップボードを読み取る */
    const QStringList rowStrList = QApplication::clipboard()->text().split("\n");

    /* 各選択範囲にペースト */
    for(const QTableWidgetSelectionRange& selected : selectedRangeList)
    {
        const int pasteRow = selected.topRow();            //ペースト開始点の行数
        const int pasteCol = selected.leftColumn();        //ペースト開始点の列数

        for(int row = 0; row < rowStrList.size(); ++row)
        {
            const QStringList colStrList = rowStrList.at(row).split(QRegularExpression("\\t| ")); //タブまたは空白区切りでセルにペースト
            if(pasteRow + row >= rowCount()) { insertRow(rowCount()); }                           //行数が足りない場合は追加

            for(int col = 0; col < colStrList.size(); ++col)
            {
                if(pasteCol + col >= columnCount()) { insertColumn(columnCount()); }              //列数が足りない場合は追加
                if(item(pasteRow + row, pasteCol + col) == nullptr){                              //QTableWidgetItemがセットされているか確認
                    QTableWidgetItem *item = new QTableWidgetItem(colStrList.at(col));
                    setItem(pasteRow + row, pasteCol + col, item);
                }
                else
                    item(pasteRow + row, pasteCol + col)->setText(colStrList.at(col));
            }
        }
    }
}

/* 選択されたセルの削除 */
void TableWidget::deleteCell()
{
    /* 選択された範囲を取得 */
    const QList<QTableWidgetSelectionRange> selectedRangeList = selectedRanges();

    /* 各選択された範囲を削除 */
    for(const QTableWidgetSelectionRange& selected : selectedRangeList)
    {
        const int startRow = selected.topRow();         //選択されたセルの開始行
        const int startCol = selected.leftColumn();     //選択されたセルの開始列
        const int endRow = selected.bottomRow();        //選択されたセルの終了行
        const int endCol = selected.rightColumn();      //選択されたセルの終了列

        for(int row = startRow; row <= endRow; ++row){
            for(int col = startCol; col <= endCol; ++col)
            {
                if(item(row, col) != nullptr)           //QTableWidgetItem*がセットされているか
                    item(row, col)->setText("");
            }
        }
    }
}

/* 選択されたセルの上に行を追加 */
void TableWidget::insertRowUp()
{
    insertRow(currentIndex().row());
}

/* 選択されたセルの下に行を追加 */
void TableWidget::insertRowDown()
{
    insertRow(currentIndex().row() + 1);
}

/* 選択されたセルの左に列を追加 */
void TableWidget::insertColLeft()
{
    insertColumn(currentIndex().column());
}

/* 選択されたセルの右に列を追加 */
void TableWidget::insertColRight()
{
    insertColumn(currentIndex().column() + 1);
}

/* 選択された範囲の行を上下対称に入れ替える */
void TableWidget::reverseRow()
{
    /* 選択された範囲を取得 */
    const QList<QTableWidgetSelectionRange> selectedRangeList = selectedRanges();

    /* 各選択された範囲 */
    for(const QTableWidgetSelectionRange& selected : selectedRangeList)
    {
        const int startRow = selected.topRow();      //選択された範囲の開始行
        const int startCol = selected.leftColumn();  //選択された範囲の開始列
        const int endRow = selected.bottomRow();     //選択された範囲の終了行
        const int endCol = selected.rightColumn();   //選択された範囲の終了列

        /* 選択された範囲の中央行 */
        const int middleRow = (endRow - startRow) / 2;

        /* 入れ替え */
        for(int row = 0; row <= middleRow; ++row){
            for(int col = startCol; col <= endCol; ++col)
            {
                if(item(startRow + row, col) == nullptr){                   //セルにQTableWidgetItem*がセットされていなかったらセット
                    QTableWidgetItem *topItem = new QTableWidgetItem("");
                    setItem(startRow + row, col, topItem);
                }
                if(item(endRow - row, col) == nullptr){                     //セルにQTableWidgetItem*がセットされていなかったらセット
                    QTableWidgetItem *btmItem = new QTableWidgetItem("");
                    setItem(endRow - row, col, btmItem);
                }

                const QString topText = item(startRow + row, col)->text();
                const QString btmText = item(endRow - row, col)->text();
                item(startRow + row, col)->setText(btmText);
                item(endRow - row, col)->setText(topText);
            }
        }
    }
}

/* 選択された範囲のセルを左右対称に入れ替える */
void TableWidget::reverseCol()
{
    /* 選択された範囲を取得 */
    const QList<QTableWidgetSelectionRange> selectedRangeList = selectedRanges();

    /* 各選択された範囲 */
    for(const QTableWidgetSelectionRange& selected : selectedRangeList)
    {
        const int startRow = selected.topRow();      //選択された範囲の開始行
        const int startCol = selected.leftColumn();  //選択された範囲の開始列
        const int endRow = selected.bottomRow();     //選択された範囲の終了行
        const int endCol = selected.rightColumn();   //選択された範囲の終了列

        /* 選択された範囲の中央列 */
        const int middleCol = (endCol - startCol) / 2;

        /* 入れ替え */
        for(int row = startRow; row <= endRow; ++row){
            for(int col = 0; col <= middleCol; ++col)
            {
                if(item(row, startCol + col) == nullptr){
                    QTableWidgetItem *leftItem = new QTableWidgetItem("");
                    setItem(row, startCol + col, leftItem);
                }
                if(item(row, endCol - col) == nullptr){
                    QTableWidgetItem *rightItem = new QTableWidgetItem("");
                    setItem(row, endCol - col, rightItem);
                }

                const QString leftText = item(row, startCol + col)->text();
                const QString rightText = item(row, endCol - col)->text();
                item(row, startCol + col)->setText(rightText);
                item(row, endCol - col)->setText(leftText);
            }
        }
    }
}

/* gnuplotでplot */
void TableWidget::plot()
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

/* 選択された範囲を表すgnuplotコマンドをクリップボードに貼り付ける */
void TableWidget::gnuplotClip()
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


void TableWidget::visualize3DBar()
{

}


void TableWidget::visualize3DScatter()
{

}

/* 選択された範囲のセルを表として描画するlatexコードをクリップボードに貼り付け */
void TableWidget::toLatexCode()
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


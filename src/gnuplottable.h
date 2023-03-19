/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef TableWidget_H
#define TableWidget_H

#include <QTableWidget>
#include "tablewidget.h"




class GnuplotTable : public TableWidget
{
    Q_OBJECT
public:
    GnuplotTable(QWidget *parent = nullptr);
    ~GnuplotTable();

    enum class PlotType { Scatter2D, Lines2D, LinesPoints2D,
                          Boxes2D, Steps2D, FSteps2D, FillSteps2D, HiSteps2D, Impulses2D,
                          Custom };
    Q_ENUM(PlotType)

public slots:
    void appendLineRow() { insertRow(rowCount()); }
    void removeLineRow() { removeRow(rowCount() - 1); }
    void appendLineCol() { insertColumn(columnCount()); }
    void removeLineCol() { removeColumn(columnCount() - 1); }

    void setOptionCmd(const QString& option) { this->optionCmd = option; }
    void gnuplotClip();
    void toLatexCode();
    void plotSelectedData(const GnuplotTable::PlotType& plotType);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onCustomContextMenu(const QPoint& point);

    void plotCellPoints(const QList<std::array<int, 3>>& ranges, const QString& cmd);

private:
    void initializeContextMenu();
    QPoint startDragPoint = QPoint(-1, -1); //mouseEvent関係の関数内の無効値(-1,-1)と合わせる。初期化しなければ(0,0)に戻って特定の操作でクラッシュする
    QPoint startDragTableSize;
    QPoint startDragCellSize;

private:
    QMenu *normalMenu = nullptr;
    QString optionCmd;
};

#endif // TableWidget_H

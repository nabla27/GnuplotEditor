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
#include <QProcess>
#include <QShortcut>
#include <QMenu>
#include <QApplication>
#include <QClipboard>
#include <QTimer>
#include "gnuplot.h"
#include "tablewidget.h"





class GnuplotTable : public TableWidget
{
    Q_OBJECT
public:
    GnuplotTable(Gnuplot *gnuplot, QWidget *parent = nullptr);
    ~GnuplotTable();

    enum class PlotType { Scatter2D, Lines2D, LinesPoints2D,
                          Boxes2D, Steps2D, FSteps2D, FillSteps2D, HiSteps2D, Impulses2D,
                          Custom };
    Q_ENUM(PlotType)

    bool isEnablenotifyUpdating() const { return notifyUpdatingEnable; }

public slots:
    void appendLineRow() { insertRow(rowCount()); }
    void removeLineRow() { removeRow(rowCount() - 1); }
    void appendLineCol() { insertColumn(columnCount()); }
    void removeLineCol() { removeColumn(columnCount() - 1); }

    void setOptionCmd(const QString& option) { this->optionCmd = option; qDebug() << __LINE__; }
    void gnuplotClip();
    void toLatexCode();
    void plotSelectedData(const GnuplotTable::PlotType& plotType);

    void changeUpdateNotification();
    void setUpdateMsec(const int msec) { updateMsec = msec; }

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onCustomContextMenu(const QPoint& point);
    void startItemChangedTimer();

    void plotCellPoints(const QList<std::array<int, 3>>& ranges, const QString& cmd);

private:
    void initializeContextMenu();
    QPoint startDragPoint = QPoint(-1, -1); //mouseEvent関係の関数内の無効値(-1,-1)と合わせる。初期化しなければ(0,0)に戻って特定の操作でクラッシュする
    QPoint startDragTableSize;
    QPoint startDragCellSize;

private:
    QMenu *normalMenu = nullptr;

    //Gnuplot *gnuplot;
    //QThread *gnuplotThread;
    QString optionCmd;

    bool notifyUpdatingEnable = false;
    QMetaObject::Connection startTimerConnection;
    QMetaObject::Connection requestUpdateConnection;
    QTimer *updateTimer;
    int updateMsec = 0;

signals:
    void tableUpdated();
    void plotRequested(QProcess *process, const QStringList& cmdList, bool preCmd);
};

#endif // TableWidget_H

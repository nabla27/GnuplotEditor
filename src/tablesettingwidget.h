/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef TABLESETTINGWIDGET_H
#define TABLESETTINGWIDGET_H

#include <QWidget>
#include "gnuplottable.h"

class QMenu;
class QScrollArea;
class QVBoxLayout;
class QStackedWidget;
class TableEditSettingWidget;
class TableCellSettingWidget;
class TablePlotSettingWidget;
class QLineEdit;
namespace mlayout { class IconLabel; }





class TableArea : public QWidget
{
    Q_OBJECT
public:
    explicit TableArea(QWidget *parent);

    QSize minimumSizeHint() const override { return QSize(0, 0); }
    GnuplotTable *tableWidget() const { return table; }

    static constexpr int iconSize = 20;

protected slots:
    void setSettingPage(QAction *action);
    void expandSettingPage();

protected:
    void resizeSettingPanel();
    //void resizeEvent(QResizeEvent *event) override;

private:
    void setupLayout();
    void setupConnection();

private:
    QScrollArea *settingScrollArea;
    QVBoxLayout *scrollContentsVLayout;

    QPixmap expandPixmap;
    QPixmap contractPixmap;

    mlayout::IconLabel *expandButton;
    mlayout::IconLabel *selectPageButton;

    QMenu *selectPageMenu;

    TableEditSettingWidget *tableEditSettingWidget;
    TableCellSettingWidget *tableCellSettingWidget;
    TablePlotSettingWidget *tablePlotSettingWidget;

    GnuplotTable *table;
};





class QSpinBox;
class QColorDialog;
class QFontComboBox;

class TableEditSettingWidget : public QWidget
{
    Q_OBJECT
public:
    TableEditSettingWidget(QWidget *parent);

public:
    void setRowCountSpinBox(const int old, const int rowCount);
    void setColCountSpinBox(const int old, const int colCount);

private slots:
    void emitChangedRowCount();
    void emitChangedColCount();
    void emitChangedCellColor();
    void emitChangedTextColor();
    void changeCellColorButton(const QColor& color);
    void changeTextColorButton(const QColor& color);

private:
    QSpinBox *rowCountSpinBox;
    QSpinBox *colCountSpinBox;
    QFontComboBox *fontComboBox;
    QSpinBox *textSizeSpinBox;
    mlayout::IconLabel *cellColorButton;
    mlayout::IconLabel *textColorButton;
    QColorDialog *cellColorDialog;
    QColorDialog *textColorDialog;

signals:
    void pasteRequested();
    void copyRequested();
    void cutRequested();
    void rowSpinBoxChanged(const int rowCount);
    void colSpinBoxChanged(const int colCount);
    void cellFontSet(const QFont& font);
    void cellTextSizeSet(const int ps);
    void boldSet();
    void italicSet();
    void underlineSet();
    void cellColorSet(const QColor& color);
    void cellTextColorSet(const QColor& color);
};





class TableCellSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TableCellSettingWidget(QWidget *parent);

signals:
    void textAlignmentSet(const Qt::AlignmentFlag& flag);
    void reverseRowRequested();
    void reverseColRequested();
    void transposeCellRequested();
    void clearCellTextRequested();
    void mergeCellsRequested();
    void splitCellsRequested();
    void insertRowAboveRequested();
    void insertRowBelowRequested();
    void insertColLeftRequested();
    void insertColRightRequested();
};







class TablePlotSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TablePlotSettingWidget(QWidget *parent);

private slots:
    void showOptionCmdEditor();

private:
    void setupMenu();

    QMenu *const plotScatter2DMenu;
    QMenu *const plotBar2DMenu;

    mlayout::IconLabel *const plotCustomExpand;
    QLineEdit *const optionCmdEdit;

signals:
    void plotRequested(const GnuplotTable::PlotType& plotType);
    void plotOptionSet(const QString& option);
};




#endif // TABLESETTINGWIDGET_H

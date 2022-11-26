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
#include <QIcon>
#include "gnuplottable.h"





class QScrollArea;
class QVBoxLayout;
class QStackedWidget;
class TableEditSettingWidget;
class TableCellSettingWidget;
class TablePlotSettingWidget;
namespace mlayout { class IconLabel; }






class TableArea : public QWidget
{
    Q_OBJECT
public:
    TableArea(QWidget *parent, QStackedWidget *sheetStack);

    void setupSheetWidget(GnuplotTable *table);

    static constexpr int iconSize = 20;

public slots:
    void setCurrentSheet(int currentIndex);

private slots:
    void setPanelExpand();
    void resizeSettingPanel();

    void setRowCount(const int row);
    void setColCount(const int col);
    void pasteCell();
    void copyCell();
    void cutCell();
    void setCellFont(const QFont& font);
    void setCellTextSize(const int ps);
    void setCellBold();
    void setCellItalic();
    void setCellUnderline();
    void setCellColor(const QColor& color);
    void setCellTextColor(const QColor& color);

    void setTextAlignment(const Qt::AlignmentFlag& flag);
    void reverseRow();
    void reverseCol();
    void transposeCell();
    void clearCellText();
    void mergeSelectedCells();
    void splitSelectedCells();
    void insertRowAbove();
    void insertRowBelow();
    void insertColLeft();
    void insertColRight();

    void plotSelectedData(const GnuplotTable::PlotType& plotType);
    void setPlotOption(const QString& optionCmd);


private:
    bool isPanelExpanding = false;
    QScrollArea *const settingScrollArea;
    QVBoxLayout *const scrollContentsVLayout;
    mlayout::IconLabel *const expandButton;
    const QPixmap expandIcon;
    const QPixmap contractIcon;

    TableEditSettingWidget *const tableEditSettingWidget;
    TableCellSettingWidget *const tableCellSettingWidget;
    TablePlotSettingWidget *const tablePlotSettingWidget;
    QStackedWidget *const sheetStack;
    GnuplotTable *currentTable;
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

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

class QStackedWidget;
class TableSettingWidget;
class GnuplotTable;



class TableArea : public QWidget
{
    Q_OBJECT
public:
    TableArea(QWidget *parent, QStackedWidget *sheetStack);

    void setupSheetWidget(GnuplotTable *table);

public slots:
    void setCurrentSheet(int currentIndex);

private slots:
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


private:
    TableSettingWidget *const tableSettingWidget;
    QStackedWidget *const sheetStack;
    GnuplotTable *currentTable;
};





class QSpinBox;
class QColorDialog;
class QFontComboBox;
namespace mlayout { class IconLabel; }

class TableSettingWidget : public QWidget
{
    Q_OBJECT
public:
    TableSettingWidget(QWidget *parent);

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

#endif // TABLESETTINGWIDGET_H

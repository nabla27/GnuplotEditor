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


private:
    TableSettingWidget *const tableSettingWidget;
    QStackedWidget *const sheetStack;
};





class QSpinBox;

class TableSettingWidget : public QWidget
{
    Q_OBJECT
public:
    TableSettingWidget(QWidget *parent);

public slots:
    void setRowCountSpinBox(const int old, const int rowCount);
    void setColCountSpinBox(const int old, const int colCount);

private slots:
    void emitChangedRowCount();
    void emitChangedColCount();

private:
    QSpinBox *rowCountSpinBox;
    QSpinBox *colCountSpinBox;

signals:
    void pasteRequested();
    void copyRequested();
    void cutRequested();
    void rowSpinBoxChanged(const int rowCount);
    void colSpinBoxChanged(const int colCount);
};

#endif // TABLESETTINGWIDGET_H

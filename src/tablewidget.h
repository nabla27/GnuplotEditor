#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableWidget>
#include <QTableWidget>
#include <QShortcut>
#include <QKeySequence>
#include <QApplication>
#include <QClipboard>
#include <QScreen>

class TableWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit TableWidget(QWidget *parent);

public:
    template <class T> void setData(const QList<QList<T> >& data);
    template <class T> QList<QList<T> > getData() const;

public slots:
    void appendRowLast() { insertRow(rowCount()); }
    void removeLastRow() { removeRow(rowCount() - 1); }
    void appendColLast() { insertColumn(columnCount()); }
    void removeLastCol() { removeColumn(columnCount() - 1); }
    void copyCell();
    void cutCell();
    void pasteCell();
    void clearCell();
    void deleteCell();
    void insertRowUp();
    void insertRowDown();
    void insertColLeft();
    void insertColRight();
    void reverseRow();
    void reverseCol();
    void transposeCell();
    void sortCol();
    void setSelectedCellColor(const QColor& color);
    void setSelectedTextColor(const QColor& color);
    void setSelectedTextFamily(const QString& family);
    void setSelectedTextSize(const int ps);
    void setSelectedTextBold();
    void setSelectedTextItalic();
    void setSelectedTextUnderline();
    void setSelectedTextAlignment(const Qt::AlignmentFlag& align);
    void mergeSelectedCells();
    void splitSelectedCells();
};



















#endif // TABLEWIDGET_H

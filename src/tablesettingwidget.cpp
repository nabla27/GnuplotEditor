/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "tablesettingwidget.h"

#include <QLayout>
#include <QStackedWidget>
#include <QSpinBox>
#include <QLabel>
#include <QHeaderView>
#include <QColorDialog>
#include <QFontComboBox>
#include "layoutparts.h"
#include "gnuplottable.h"


TableArea::TableArea(QWidget *parent, QStackedWidget *sheetStack)
    : QWidget(parent)
    , tableSettingWidget(new TableSettingWidget(this))
    , sheetStack(sheetStack)
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);

    setLayout(vLayout);
    vLayout->addWidget(tableSettingWidget);
    vLayout->addWidget(sheetStack);

    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);

    connect(sheetStack, &QStackedWidget::currentChanged, this, &TableArea::setCurrentSheet);
}

void TableArea::setupSheetWidget(GnuplotTable *table)
{
    /* table -> tableSettingWidget */
    connect(table->verticalHeader(), &QHeaderView::sectionCountChanged,
            tableSettingWidget, &TableSettingWidget::setRowCountSpinBox);
    connect(table->horizontalHeader(), &QHeaderView::sectionCountChanged,
            tableSettingWidget, &TableSettingWidget::setColCountSpinBox);

    /* tableSettingWidget -> table */
    connect(tableSettingWidget, &TableSettingWidget::rowSpinBoxChanged,
            this, &TableArea::setRowCount);
    connect(tableSettingWidget, &TableSettingWidget::colSpinBoxChanged,
            this, &TableArea::setColCount);
    connect(tableSettingWidget, &TableSettingWidget::pasteRequested,
            this, &TableArea::pasteCell);
    connect(tableSettingWidget, &TableSettingWidget::copyRequested,
            this, &TableArea::copyCell);
    connect(tableSettingWidget, &TableSettingWidget::cutRequested,
            this, &TableArea::cutCell);
    connect(tableSettingWidget, &TableSettingWidget::cellFontSet,
            this, &TableArea::setCellFont);
    connect(tableSettingWidget, &TableSettingWidget::cellTextSizeSet,
            this, &TableArea::setCellTextSize);
    connect(tableSettingWidget, &TableSettingWidget::boldSet,
            this, &TableArea::setCellBold);
    connect(tableSettingWidget, &TableSettingWidget::italicSet,
            this, &TableArea::setCellItalic);
    connect(tableSettingWidget, &TableSettingWidget::underlineSet,
            this, &TableArea::setCellUnderline);
    connect(tableSettingWidget, &TableSettingWidget::cellColorSet,
            this, &TableArea::setCellColor);
    connect(tableSettingWidget, &TableSettingWidget::cellTextColorSet,
            this, &TableArea::setCellTextColor);
}

void TableArea::setCurrentSheet(int currentIndex)
{
    if(GnuplotTable *table = static_cast<GnuplotTable*>(sheetStack->widget(currentIndex)))
    {
        tableSettingWidget->setRowCountSpinBox(0, table->rowCount());
        tableSettingWidget->setColCountSpinBox(0, table->columnCount());

        currentTable = table;
    }
}

void TableArea::setRowCount(const int row)
{
    if(currentTable) currentTable->setRowCount(row);
}

void TableArea::setColCount(const int col)
{
    if(currentTable) currentTable->setColumnCount(col);
}

void TableArea::pasteCell()
{
    if(currentTable) currentTable->pasteCell();
}

void TableArea::copyCell()
{
    if(currentTable) currentTable->copyCell();
}

void TableArea::cutCell()
{
    if(currentTable) currentTable->cutCell();
}

void TableArea::setCellFont(const QFont& font)
{
    if(currentTable)
    {
        currentTable->setSelectedTextFamily(font.family());
    }
}

void TableArea::setCellTextSize(const int ps)
{
    if(currentTable)
    {
        currentTable->setSelectedTextSize(ps);
    }
}

void TableArea::setCellBold()
{
    if(currentTable)
    {
        currentTable->setSelectedTextBold();
    }
}

void TableArea::setCellItalic()
{
    if(currentTable)
    {
        currentTable->setSelectedTextItalic();
    }
}

void TableArea::setCellUnderline()
{
    if(currentTable)
    {
        currentTable->setSelectedTextUnderline();
    }
}

void TableArea::setCellColor(const QColor& color)
{
    if(currentTable)
    {
        currentTable->setSelectedCellColor(color);
    }
}

void TableArea::setCellTextColor(const QColor& color)
{
    if(currentTable)
    {
        currentTable->setSelectedTextColor(color);
    }
}





TableSettingWidget::TableSettingWidget(QWidget *parent)
    : QWidget(parent)
    , rowCountSpinBox(new QSpinBox(this))
    , colCountSpinBox(new QSpinBox(this))
    , fontComboBox(new QFontComboBox(this))
    , textSizeSpinBox(new QSpinBox(this))
    , cellColorButton(new mlayout::IconLabel(this))
    , textColorButton(new mlayout::IconLabel(this))
    , cellColorDialog(new QColorDialog(Qt::white, this))
    , textColorDialog(new QColorDialog(Qt::black, this))
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QHBoxLayout *topHLayout = new QHBoxLayout;

    setLayout(vLayout);
    vLayout->addLayout(topHLayout);

    mlayout::IconLabel *pasteButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *copyButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *cutButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *boldButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *italicButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *underlineButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *cellColorExpand = new mlayout::IconLabel(this);
    mlayout::IconLabel *textColorExpand = new mlayout::IconLabel(this);

    topHLayout->addWidget(new QLabel(" Row "));
    topHLayout->addWidget(rowCountSpinBox);
    topHLayout->addWidget(new QLabel(" Col "));
    topHLayout->addWidget(colCountSpinBox);
    topHLayout->addWidget(pasteButton);
    topHLayout->addWidget(copyButton);
    topHLayout->addWidget(cutButton);
    topHLayout->addWidget(fontComboBox);
    topHLayout->addWidget(textSizeSpinBox);
    topHLayout->addWidget(boldButton);
    topHLayout->addWidget(italicButton);
    topHLayout->addWidget(underlineButton);
    topHLayout->addWidget(cellColorButton);
    topHLayout->addWidget(cellColorExpand);
    topHLayout->addWidget(textColorButton);
    topHLayout->addWidget(textColorExpand);
    topHLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

    static constexpr int iconSize = 20;

    pasteButton->setPixmap(QPixmap(":/icon/edit_paste").scaled(iconSize, iconSize));
    copyButton->setPixmap(QPixmap(":/icon/edit_copy").scaled(iconSize, iconSize));
    cutButton->setPixmap(QPixmap(":/icon/edit_cut").scaled(iconSize, iconSize));
    boldButton->setPixmap(QPixmap(":/icon/edit_bold").scaled(iconSize, iconSize));
    italicButton->setPixmap(QPixmap(":/icon/edit_italic").scaled(iconSize, iconSize));
    underlineButton->setPixmap(QPixmap(":/icon/edit_underline").scaled(iconSize, iconSize));
    cellColorButton->setPixmap(QPixmap(":/icon/edit_backgroundcolor").scaled(iconSize, iconSize));
    cellColorExpand->setPixmap(QPixmap(":/icon/icon_triangle").scaled(iconSize / 3, iconSize / 3));
    textColorButton->setPixmap(QPixmap(":/icon/edit_textcolor").scaled(iconSize, iconSize));
    textColorExpand->setPixmap(QPixmap(":/icon/icon_triangle").scaled(iconSize / 3, iconSize / 3));
    pasteButton->setHoveredPalette(QPalette(pasteButton->backgroundRole(), Qt::lightGray));
    copyButton->setHoveredPalette(QPalette(copyButton->backgroundRole(), Qt::lightGray));
    cutButton->setHoveredPalette(QPalette(cutButton->backgroundRole(), Qt::lightGray));
    boldButton->setHoveredPalette(QPalette(boldButton->backgroundRole(), Qt::lightGray));
    italicButton->setHoveredPalette(QPalette(italicButton->backgroundRole(), Qt::lightGray));
    underlineButton->setHoveredPalette(QPalette(underlineButton->backgroundRole(), Qt::lightGray));
    cellColorButton->setHoveredPalette(QPalette(cellColorButton->backgroundRole(), Qt::lightGray));
    cellColorExpand->setHoveredPalette(QPalette(cellColorExpand->backgroundRole(), Qt::lightGray));
    textColorButton->setHoveredPalette(QPalette(textColorButton->backgroundRole(), Qt::lightGray));
    textColorExpand->setHoveredPalette(QPalette(textColorExpand->backgroundRole(), Qt::lightGray));
    pasteButton->setAutoFillBackground(true);
    copyButton->setAutoFillBackground(true);
    cutButton->setAutoFillBackground(true);
    boldButton->setAutoFillBackground(true);
    italicButton->setAutoFillBackground(true);
    underlineButton->setAutoFillBackground(true);
    cellColorButton->setAutoFillBackground(true);
    cellColorExpand->setAutoFillBackground(true);
    textColorButton->setAutoFillBackground(true);
    textColorExpand->setAutoFillBackground(true);
    pasteButton->setToolTip("paste");
    copyButton->setToolTip("copy");
    cutButton->setToolTip("cut");
    boldButton->setToolTip("bold");
    italicButton->setToolTip("italic");
    underlineButton->setToolTip("underline");
    cellColorButton->setToolTip("background color");
    cellColorExpand->setToolTip("background color");
    textColorButton->setToolTip("text color");
    textColorExpand->setToolTip("text color");

    vLayout->setContentsMargins(0, 0, 0, 0);
    topHLayout->setContentsMargins(0, 0, 0, 0);
    topHLayout->setSpacing(1);

    textSizeSpinBox->setValue(9);
    cellColorButton->setPaintFrameColor(Qt::white);
    cellColorButton->setPaintFrameWidth(2);
    textColorButton->setPaintFrameColor(Qt::black);
    textColorButton->setPaintFrameWidth(2);

    connect(rowCountSpinBox, &QSpinBox::editingFinished, this, &TableSettingWidget::emitChangedRowCount);
    connect(colCountSpinBox, &QSpinBox::editingFinished, this, &TableSettingWidget::emitChangedColCount);
    connect(pasteButton, &mlayout::IconLabel::released, this, &TableSettingWidget::pasteRequested);
    connect(copyButton, &mlayout::IconLabel::released, this, &TableSettingWidget::copyRequested);
    connect(cutButton, &mlayout::IconLabel::released, this, &TableSettingWidget::cutRequested);
    connect(fontComboBox, &QFontComboBox::currentFontChanged, this, &TableSettingWidget::cellFontSet);
    connect(textSizeSpinBox, &QSpinBox::valueChanged, this, &TableSettingWidget::cellTextSizeSet);
    connect(boldButton, &mlayout::IconLabel::released, this, &TableSettingWidget::boldSet);
    connect(italicButton, &mlayout::IconLabel::released, this, &TableSettingWidget::italicSet);
    connect(underlineButton, &mlayout::IconLabel::released, this, &TableSettingWidget::underlineSet);
    connect(cellColorButton, &mlayout::IconLabel::released, this, &TableSettingWidget::emitChangedCellColor);
    connect(cellColorExpand, &mlayout::IconLabel::released, cellColorDialog, &QColorDialog::show);
    connect(cellColorDialog, &QColorDialog::currentColorChanged, this, &TableSettingWidget::changeCellColorButton);
    connect(textColorButton, &mlayout::IconLabel::released, this, &TableSettingWidget::emitChangedTextColor);
    connect(textColorExpand, &mlayout::IconLabel::released, textColorDialog, &QColorDialog::show);
    connect(textColorDialog, &QColorDialog::currentColorChanged, this, &TableSettingWidget::changeTextColorButton);
}

void TableSettingWidget::emitChangedRowCount()
{
    emit rowSpinBoxChanged(rowCountSpinBox->value());
}

void TableSettingWidget::emitChangedColCount()
{
    emit colSpinBoxChanged(colCountSpinBox->value());
}

void TableSettingWidget::emitChangedCellColor()
{
    emit cellColorSet(cellColorDialog->currentColor());
}

void TableSettingWidget::emitChangedTextColor()
{
    emit cellTextColorSet(textColorDialog->currentColor());
}

void TableSettingWidget::changeCellColorButton(const QColor& color)
{
    cellColorButton->setPaintFrameColor(color);
    emitChangedCellColor();
}

void TableSettingWidget::changeTextColorButton(const QColor& color)
{
    textColorButton->setPaintFrameColor(color);
    emitChangedTextColor();
}

void TableSettingWidget::setRowCountSpinBox(const int, const int rowCount)
{
    rowCountSpinBox->setValue(rowCount);
}

void TableSettingWidget::setColCountSpinBox(const int, const int colCount)
{
    colCountSpinBox->setValue(colCount);
}






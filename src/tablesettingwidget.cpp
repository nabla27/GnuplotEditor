/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "tableSettingWidget.h"

#include <QLayout>
#include <QStackedWidget>
#include <QSpinBox>
#include <QLabel>
#include <QHeaderView>
#include <QColorDialog>
#include <QFontComboBox>
#include <QScrollArea>
#include "layoutparts.h"
#include "gnuplottable.h"


TableArea::TableArea(QWidget *parent, QStackedWidget *sheetStack)
    : QWidget(parent)
    , settingScrollArea(new QScrollArea(this))
    , scrollContentsVLayout(new QVBoxLayout)
    , tableEditSettingWidget(new TableEditSettingWidget(this))
    , sheetStack(sheetStack)
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QWidget *scrollContents = new QWidget(settingScrollArea);

    setLayout(vLayout);
    vLayout->addWidget(settingScrollArea);
    vLayout->addWidget(sheetStack);
    settingScrollArea->setWidget(scrollContents);
    scrollContents->setLayout(scrollContentsVLayout);

    scrollContentsVLayout->addWidget(tableEditSettingWidget);

    vLayout->setSpacing(0);
    scrollContentsVLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);
    settingScrollArea->setContentsMargins(0, 0, 0, 0);
    scrollContents->setContentsMargins(0, 0, 0, 0);
    scrollContentsVLayout->setContentsMargins(0, 0, 0, 0);

    settingScrollArea->setWidgetResizable(true);
    //scrollContentsVLayout->setSizeConstraint(QLayout::SetMinimumSize);

    settingScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    scrollContents->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    tableEditSettingWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    sheetStack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(settingScrollArea->horizontalScrollBar(), &QScrollBar::rangeChanged,
            this, &TableArea::resizePanelFromHorizontalBar);
    connect(sheetStack, &QStackedWidget::currentChanged, this, &TableArea::setCurrentSheet);
}

void TableArea::setupSheetWidget(GnuplotTable *table)
{
    /* table -> tableEditSettingWidget */
    connect(table->verticalHeader(), &QHeaderView::sectionCountChanged,
            tableEditSettingWidget, &TableEditSettingWidget::setRowCountSpinBox);
    connect(table->horizontalHeader(), &QHeaderView::sectionCountChanged,
            tableEditSettingWidget, &TableEditSettingWidget::setColCountSpinBox);

    /* tableEditSettingWidget -> table */
    connect(tableEditSettingWidget, &TableEditSettingWidget::rowSpinBoxChanged,
            this, &TableArea::setRowCount);
    connect(tableEditSettingWidget, &TableEditSettingWidget::colSpinBoxChanged,
            this, &TableArea::setColCount);
    connect(tableEditSettingWidget, &TableEditSettingWidget::pasteRequested,
            this, &TableArea::pasteCell);
    connect(tableEditSettingWidget, &TableEditSettingWidget::copyRequested,
            this, &TableArea::copyCell);
    connect(tableEditSettingWidget, &TableEditSettingWidget::cutRequested,
            this, &TableArea::cutCell);
    connect(tableEditSettingWidget, &TableEditSettingWidget::cellFontSet,
            this, &TableArea::setCellFont);
    connect(tableEditSettingWidget, &TableEditSettingWidget::cellTextSizeSet,
            this, &TableArea::setCellTextSize);
    connect(tableEditSettingWidget, &TableEditSettingWidget::boldSet,
            this, &TableArea::setCellBold);
    connect(tableEditSettingWidget, &TableEditSettingWidget::italicSet,
            this, &TableArea::setCellItalic);
    connect(tableEditSettingWidget, &TableEditSettingWidget::underlineSet,
            this, &TableArea::setCellUnderline);
    connect(tableEditSettingWidget, &TableEditSettingWidget::cellColorSet,
            this, &TableArea::setCellColor);
    connect(tableEditSettingWidget, &TableEditSettingWidget::cellTextColorSet,
            this, &TableArea::setCellTextColor);
}

void TableArea::setCurrentSheet(int currentIndex)
{
    if(GnuplotTable *table = static_cast<GnuplotTable*>(sheetStack->widget(currentIndex)))
    {
        tableEditSettingWidget->setRowCountSpinBox(0, table->rowCount());
        tableEditSettingWidget->setColCountSpinBox(0, table->columnCount());

        currentTable = table;
    }
}

/* horizontalScrollBarの有無によってsettingScrollAreaの高さを変更する */
void TableArea::resizePanelFromHorizontalBar(int, int max)
{
    int settingPanelHeight = settingScrollArea->sizeHint().height();

    if(max > 0)
        settingPanelHeight += settingScrollArea->horizontalScrollBar()->height();

    settingScrollArea->setFixedHeight(settingPanelHeight);
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





TableEditSettingWidget::TableEditSettingWidget(QWidget *parent)
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
    topHLayout->setSpacing(0);

    textSizeSpinBox->setValue(9);
    cellColorButton->setPaintFrameColor(Qt::white);
    cellColorButton->setPaintFrameWidth(2);
    textColorButton->setPaintFrameColor(Qt::black);
    textColorButton->setPaintFrameWidth(2);

    connect(rowCountSpinBox, &QSpinBox::editingFinished, this, &TableEditSettingWidget::emitChangedRowCount);
    connect(colCountSpinBox, &QSpinBox::editingFinished, this, &TableEditSettingWidget::emitChangedColCount);
    connect(pasteButton, &mlayout::IconLabel::released, this, &TableEditSettingWidget::pasteRequested);
    connect(copyButton, &mlayout::IconLabel::released, this, &TableEditSettingWidget::copyRequested);
    connect(cutButton, &mlayout::IconLabel::released, this, &TableEditSettingWidget::cutRequested);
    connect(fontComboBox, &QFontComboBox::currentFontChanged, this, &TableEditSettingWidget::cellFontSet);
    connect(textSizeSpinBox, &QSpinBox::valueChanged, this, &TableEditSettingWidget::cellTextSizeSet);
    connect(boldButton, &mlayout::IconLabel::released, this, &TableEditSettingWidget::boldSet);
    connect(italicButton, &mlayout::IconLabel::released, this, &TableEditSettingWidget::italicSet);
    connect(underlineButton, &mlayout::IconLabel::released, this, &TableEditSettingWidget::underlineSet);
    connect(cellColorButton, &mlayout::IconLabel::released, this, &TableEditSettingWidget::emitChangedCellColor);
    connect(cellColorExpand, &mlayout::IconLabel::released, cellColorDialog, &QColorDialog::show);
    connect(cellColorDialog, &QColorDialog::currentColorChanged, this, &TableEditSettingWidget::changeCellColorButton);
    connect(textColorButton, &mlayout::IconLabel::released, this, &TableEditSettingWidget::emitChangedTextColor);
    connect(textColorExpand, &mlayout::IconLabel::released, textColorDialog, &QColorDialog::show);
    connect(textColorDialog, &QColorDialog::currentColorChanged, this, &TableEditSettingWidget::changeTextColorButton);
}

void TableEditSettingWidget::emitChangedRowCount()
{
    emit rowSpinBoxChanged(rowCountSpinBox->value());
}

void TableEditSettingWidget::emitChangedColCount()
{
    emit colSpinBoxChanged(colCountSpinBox->value());
}

void TableEditSettingWidget::emitChangedCellColor()
{
    emit cellColorSet(cellColorDialog->currentColor());
}

void TableEditSettingWidget::emitChangedTextColor()
{
    emit cellTextColorSet(textColorDialog->currentColor());
}

void TableEditSettingWidget::changeCellColorButton(const QColor& color)
{
    cellColorButton->setPaintFrameColor(color);
    emitChangedCellColor();
}

void TableEditSettingWidget::changeTextColorButton(const QColor& color)
{
    textColorButton->setPaintFrameColor(color);
    emitChangedTextColor();
}

void TableEditSettingWidget::setRowCountSpinBox(const int, const int rowCount)
{
    rowCountSpinBox->setValue(rowCount);
}

void TableEditSettingWidget::setColCountSpinBox(const int, const int colCount)
{
    colCountSpinBox->setValue(colCount);
}






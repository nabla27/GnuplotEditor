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
    , tableCellSettingWidget(new TableCellSettingWidget(this))
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
    scrollContentsVLayout->addWidget(tableCellSettingWidget);

    vLayout->setSpacing(0);
    scrollContentsVLayout->setSpacing(1);
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

    /* tableCellSetting -> table */
    connect(tableCellSettingWidget, &TableCellSettingWidget::textAlignmentSet,
            this, &TableArea::setTextAlignment);
    connect(tableCellSettingWidget, &TableCellSettingWidget::reverseRowRequested,
            this, &TableArea::reverseRow);
    connect(tableCellSettingWidget, &TableCellSettingWidget::reverseColRequested,
            this, &TableArea::reverseCol);
    connect(tableCellSettingWidget, &TableCellSettingWidget::transposeCellRequested,
            this, &TableArea::transposeCell);
    connect(tableCellSettingWidget, &TableCellSettingWidget::clearCellTextRequested,
            this, &TableArea::clearCellText);
    connect(tableCellSettingWidget, &TableCellSettingWidget::mergeCellsRequested,
            this, &TableArea::mergeSelectedCells);
    connect(tableCellSettingWidget, &TableCellSettingWidget::splitCellsRequested,
            this, &TableArea::splitSelectedCells);
    connect(tableCellSettingWidget, &TableCellSettingWidget::insertRowAboveRequested,
            this, &TableArea::insertRowAbove);
    connect(tableCellSettingWidget, &TableCellSettingWidget::insertRowBelowRequested,
            this, &TableArea::insertRowBelow);
    connect(tableCellSettingWidget, &TableCellSettingWidget::insertColLeftRequested,
            this, &TableArea::insertColLeft);
    connect(tableCellSettingWidget, &TableCellSettingWidget::insertColRightRequested,
            this, &TableArea::insertColRight);
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

void TableArea::setTextAlignment(const Qt::AlignmentFlag &flag)
{
    if(currentTable)
    {
        currentTable->setSelectedTextAlignment(flag);
    }
}

void TableArea::reverseRow()
{
    if(currentTable)
    {
        currentTable->reverseRow();
    }
}

void TableArea::reverseCol()
{
    if(currentTable)
    {
        currentTable->reverseCol();
    }
}

void TableArea::transposeCell()
{
    if(currentTable)
    {
        currentTable->transposeCell();
    }
}

void TableArea::clearCellText()
{
    if(currentTable)
    {
        currentTable->clearCell();
    }
}

void TableArea::mergeSelectedCells()
{
    if(currentTable)
    {
        currentTable->mergeSelectedCells();
    }
}

void TableArea::splitSelectedCells()
{
    if(currentTable)
    {
        currentTable->splitSelectedCells();
    }
}

void TableArea::insertRowAbove()
{
    if(currentTable) currentTable->insertRowUp();
}

void TableArea::insertRowBelow()
{
    if(currentTable) currentTable->insertRowDown();
}

void TableArea::insertColLeft()
{
    if(currentTable) currentTable->insertColLeft();
}

void TableArea::insertColRight()
{
    if(currentTable) currentTable->insertColRight();
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
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    setLayout(hLayout);

    mlayout::IconLabel *pasteButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *copyButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *cutButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *boldButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *italicButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *underlineButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *cellColorExpand = new mlayout::IconLabel(this);
    mlayout::IconLabel *textColorExpand = new mlayout::IconLabel(this);

    hLayout->addWidget(new QLabel(" Row "));
    hLayout->addWidget(rowCountSpinBox);
    hLayout->addWidget(new QLabel(" Col "));
    hLayout->addWidget(colCountSpinBox);
    hLayout->addWidget(pasteButton);
    hLayout->addWidget(copyButton);
    hLayout->addWidget(cutButton);
    hLayout->addWidget(fontComboBox);
    hLayout->addWidget(textSizeSpinBox);
    hLayout->addWidget(boldButton);
    hLayout->addWidget(italicButton);
    hLayout->addWidget(underlineButton);
    hLayout->addWidget(cellColorButton);
    hLayout->addWidget(cellColorExpand);
    hLayout->addWidget(textColorButton);
    hLayout->addWidget(textColorExpand);
    hLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

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

    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(1);

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











TableCellSettingWidget::TableCellSettingWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    setLayout(hLayout);
    hLayout->setSpacing(1);
    hLayout->setContentsMargins(0, 0, 0, 0);

    mlayout::IconLabel *alignLeftButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *alignHCenterButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *alignRightButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *alignJustButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *alignTopButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *alignVCenterButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *alignBottomButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *alignBaselineButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *reverseRowButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *reverseColButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *transposeButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *clearTextButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *mergeCellsButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *splitCellsButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *insertRowAboveButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *insertRowBelowButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *insertColLeftButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *insertColRightButton = new mlayout::IconLabel(this);

    hLayout->addWidget(alignLeftButton);
    hLayout->addWidget(alignHCenterButton);
    hLayout->addWidget(alignRightButton);
    hLayout->addWidget(alignJustButton);
    hLayout->addWidget(new mlayout::SeparatorLineWidget(this, Qt::Orientation::Vertical, 1, Qt::lightGray, 0));
    hLayout->addWidget(alignTopButton);
    hLayout->addWidget(alignVCenterButton);
    hLayout->addWidget(alignBottomButton);
    hLayout->addWidget(alignBaselineButton);
    hLayout->addWidget(new mlayout::SeparatorLineWidget(this, Qt::Orientation::Vertical, 1, Qt::lightGray, 0));
    hLayout->addWidget(reverseRowButton);
    hLayout->addWidget(reverseColButton);
    hLayout->addWidget(transposeButton);
    hLayout->addWidget(new mlayout::SeparatorLineWidget(this, Qt::Orientation::Vertical, 1, Qt::lightGray, 0));
    hLayout->addWidget(clearTextButton);
    hLayout->addWidget(mergeCellsButton);
    hLayout->addWidget(splitCellsButton);
    hLayout->addWidget(new mlayout::SeparatorLineWidget(this, Qt::Orientation::Vertical, 1, Qt::lightGray, 0));
    hLayout->addWidget(insertRowAboveButton);
    hLayout->addWidget(insertRowBelowButton);
    hLayout->addWidget(insertColLeftButton);
    hLayout->addWidget(insertColRightButton);
    hLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

    static constexpr int iconSize = 20;

    alignLeftButton->setPixmap(QPixmap(":/icon/edit_alignleft").scaled(iconSize, iconSize));
    alignHCenterButton->setPixmap(QPixmap(":/icon/edit_alignhcenter").scaled(iconSize, iconSize));
    alignRightButton->setPixmap(QPixmap(":/icon/edit_alignright").scaled(iconSize, iconSize));
    alignJustButton->setPixmap(QPixmap(":/icon/edit_alignjust").scaled(iconSize, iconSize));
    alignTopButton->setPixmap(QPixmap(":/icon/edit_aligntop").scaled(iconSize, iconSize));
    alignVCenterButton->setPixmap(QPixmap(":/icon/edit_alignvcenter").scaled(iconSize, iconSize));
    alignBottomButton->setPixmap(QPixmap(":/icon/edit_alignbottom").scaled(iconSize, iconSize));
    alignBaselineButton->setPixmap(QPixmap(":/icon/edit_alignbaseline").scaled(iconSize, iconSize));
    reverseRowButton->setPixmap(QPixmap(":/icon/edit_reverserow").scaled(iconSize, iconSize));
    reverseColButton->setPixmap(QPixmap(":/icon/edit_reversecol").scaled(iconSize, iconSize));
    transposeButton->setPixmap(QPixmap(":/icon/edit_reverserow").transformed(QTransform().rotate(45)).scaled(iconSize, iconSize));
    clearTextButton->setPixmap(QPixmap(":/icon/edit_clear").scaled(iconSize, iconSize));
    mergeCellsButton->setPixmap(QPixmap(":/icon/edit_mergecells").scaled(iconSize, iconSize));
    splitCellsButton->setPixmap(QPixmap(":/icon/edit_splitcells").scaled(iconSize, iconSize));
    insertRowAboveButton->setPixmap(QPixmap(":/icon/edit_insertrowabove").scaled(iconSize, iconSize));
    insertRowBelowButton->setPixmap(QPixmap(":/icon/edit_insertrowbelow").scaled(iconSize, iconSize));
    insertColLeftButton->setPixmap(QPixmap(":/icon/edit_insertcolleft").scaled(iconSize, iconSize));
    insertColRightButton->setPixmap(QPixmap(":/icon/edit_insertcolright").scaled(iconSize, iconSize));

    QPalette buttonHoveredPalette(alignLeftButton->foregroundRole(), Qt::lightGray);
    alignLeftButton->setHoveredPalette(buttonHoveredPalette);
    alignHCenterButton->setHoveredPalette(buttonHoveredPalette);
    alignRightButton->setHoveredPalette(buttonHoveredPalette);
    alignJustButton->setHoveredPalette(buttonHoveredPalette);
    alignTopButton->setHoveredPalette(buttonHoveredPalette);
    alignVCenterButton->setHoveredPalette(buttonHoveredPalette);
    alignBottomButton->setHoveredPalette(buttonHoveredPalette);
    alignBaselineButton->setHoveredPalette(buttonHoveredPalette);
    reverseRowButton->setHoveredPalette(buttonHoveredPalette);
    reverseColButton->setHoveredPalette(buttonHoveredPalette);
    transposeButton->setHoveredPalette(buttonHoveredPalette);;
    clearTextButton->setHoveredPalette(buttonHoveredPalette);
    mergeCellsButton->setHoveredPalette(buttonHoveredPalette);
    splitCellsButton->setHoveredPalette(buttonHoveredPalette);
    insertRowAboveButton->setHoveredPalette(buttonHoveredPalette);
    insertRowBelowButton->setHoveredPalette(buttonHoveredPalette);
    insertColLeftButton->setHoveredPalette(buttonHoveredPalette);
    insertColRightButton->setHoveredPalette(buttonHoveredPalette);

    alignLeftButton->setAutoFillBackground(true);
    alignHCenterButton->setAutoFillBackground(true);
    alignRightButton->setAutoFillBackground(true);
    alignJustButton->setAutoFillBackground(true);
    alignTopButton->setAutoFillBackground(true);
    alignVCenterButton->setAutoFillBackground(true);
    alignBottomButton->setAutoFillBackground(true);
    alignBaselineButton->setAutoFillBackground(true);
    reverseRowButton->setAutoFillBackground(true);
    reverseColButton->setAutoFillBackground(true);
    transposeButton->setAutoFillBackground(true);
    clearTextButton->setAutoFillBackground(true);
    mergeCellsButton->setAutoFillBackground(true);
    splitCellsButton->setAutoFillBackground(true);
    insertRowAboveButton->setAutoFillBackground(true);
    insertRowBelowButton->setAutoFillBackground(true);
    insertColLeftButton->setAutoFillBackground(true);
    insertColRightButton->setAutoFillBackground(true);

    alignLeftButton->setToolTip("Aligns with the left edge.");
    alignHCenterButton->setToolTip("Centers horizontally in the available space.");
    alignRightButton->setToolTip("Aligns with the right edge.");
    alignJustButton->setToolTip("Justifies the text in the available space.");
    alignTopButton->setToolTip("Aligns with the top.");
    alignVCenterButton->setToolTip("Aligns with the bottom.");
    alignBottomButton->setToolTip("Centers vertically in the available space.");
    alignBaselineButton->setToolTip("Aligns with the baseline.");
    reverseRowButton->setToolTip("reverse row");
    reverseColButton->setToolTip("reverse column");
    transposeButton->setToolTip("transpose");
    clearTextButton->setToolTip("clear text");
    mergeCellsButton->setToolTip("merge cells");
    splitCellsButton->setToolTip("split cells");
    insertRowAboveButton->setToolTip("insert row above");
    insertRowBelowButton->setToolTip("insert row below");
    insertColLeftButton->setToolTip("insert column left");
    insertColRightButton->setToolTip("insert column right");

    connect(alignLeftButton, &mlayout::IconLabel::released, [this](){ emit textAlignmentSet(Qt::AlignLeft); });
    connect(alignHCenterButton, &mlayout::IconLabel::released, [this](){ emit textAlignmentSet(Qt::AlignHCenter); });
    connect(alignRightButton, &mlayout::IconLabel::released, [this](){ emit textAlignmentSet(Qt::AlignRight); });
    connect(alignJustButton, &mlayout::IconLabel::released, [this](){ emit textAlignmentSet(Qt::AlignJustify); });
    connect(alignTopButton, &mlayout::IconLabel::released, [this](){ emit textAlignmentSet(Qt::AlignTop); });
    connect(alignVCenterButton, &mlayout::IconLabel::released, [this](){ emit textAlignmentSet(Qt::AlignVCenter); });
    connect(alignBottomButton, &mlayout::IconLabel::released, [this](){ emit textAlignmentSet(Qt::AlignBottom); });
    connect(alignBaselineButton, &mlayout::IconLabel::released, [this](){ emit textAlignmentSet(Qt::AlignBaseline); });
    connect(reverseRowButton, &mlayout::IconLabel::released, this, &TableCellSettingWidget::reverseRowRequested);
    connect(reverseColButton, &mlayout::IconLabel::released, this, &TableCellSettingWidget::reverseColRequested);
    connect(transposeButton, &mlayout::IconLabel::released, this, &TableCellSettingWidget::transposeCellRequested);
    connect(clearTextButton, &mlayout::IconLabel::released, this, &TableCellSettingWidget::clearCellTextRequested);
    connect(mergeCellsButton, &mlayout::IconLabel::released, this, &TableCellSettingWidget::mergeCellsRequested);
    connect(splitCellsButton, &mlayout::IconLabel::released, this, &TableCellSettingWidget::splitCellsRequested);
    connect(insertRowAboveButton, &mlayout::IconLabel::released, this, &TableCellSettingWidget::insertRowAboveRequested);
    connect(insertRowBelowButton, &mlayout::IconLabel::released, this, &TableCellSettingWidget::insertRowBelowRequested);
    connect(insertColLeftButton, &mlayout::IconLabel::released, this, &TableCellSettingWidget::insertColLeftRequested);
    connect(insertColRightButton, &mlayout::IconLabel::released, this, &TableCellSettingWidget::insertColRightRequested);
}





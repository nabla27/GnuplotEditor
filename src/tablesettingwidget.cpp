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
#include <QScrollBar>
#include <QMenu>
#include "layoutparts.h"
#include "gnuplottable.h"
#include "standardpixmap.h"



TableArea::TableArea(QWidget *parent)
    : QWidget(parent)
    , settingScrollArea(new QScrollArea(this))
    , scrollContentsVLayout(new QVBoxLayout)
    , expandPixmap(StandardPixmap::Icon::triangle().scaled(iconSize, iconSize))
    , contractPixmap(StandardPixmap::Icon::triangle().transformed(QTransform().rotate(180)).scaled(iconSize, iconSize))

    , expandButton(new mlayout::IconLabel(this))
    , selectPageButton(new mlayout::IconLabel(this))

    , tableEditSettingWidget(new TableEditSettingWidget(nullptr))
    , tableCellSettingWidget(new TableCellSettingWidget(nullptr))
    , tablePlotSettingWidget(new TablePlotSettingWidget(nullptr))

    , selectPageMenu(new QMenu(this))

    , table(new GnuplotTable(nullptr))
{
    setupLayout();

    connect(selectPageButton, &mlayout::IconLabel::released, [this](){ selectPageMenu->exec(cursor().pos()); });
    connect(selectPageMenu, &QMenu::triggered, this, &TableArea::setSettingPage);
    connect(expandButton, &mlayout::IconLabel::released, this, &TableArea::expandSettingPage);

    setupConnection();

    settingScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);
    settingScrollArea->horizontalScrollBar()->setFixedHeight(17);
    resizeSettingPanel();
}

TableArea::~TableArea()
{
    delete table; table = nullptr;
}

void TableArea::setupLayout()
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QWidget *scrollContents = new QWidget(settingScrollArea);
    QHBoxLayout *settingHLayout = new QHBoxLayout;
    QHBoxLayout *panelButtonHLayout = new QHBoxLayout;

    expandButton->setPixmap(expandPixmap);
    selectPageButton->setPixmap(StandardPixmap::Icon::page().scaled(iconSize, iconSize));
    expandButton->setHoveredPalette(QPalette(expandButton->backgroundRole(), Qt::lightGray));
    selectPageButton->setHoveredPalette(QPalette(selectPageButton->backgroundRole(), Qt::lightGray));
    expandButton->setAutoFillBackground(true);
    selectPageButton->setAutoFillBackground(true);

    setLayout(vLayout);
    vLayout->addLayout(settingHLayout);
    vLayout->addWidget(table);
    settingHLayout->addWidget(settingScrollArea);
    settingHLayout->addLayout(panelButtonHLayout);
    settingScrollArea->setWidget(scrollContents);
    scrollContents->setLayout(scrollContentsVLayout);
    panelButtonHLayout->addWidget(new mlayout::SeparatorLineWidget(this, Qt::Orientation::Vertical));
    panelButtonHLayout->addWidget(selectPageButton);
    panelButtonHLayout->addWidget(expandButton);

    vLayout->setSpacing(0);
    scrollContentsVLayout->setSpacing(2);
    settingHLayout->setSpacing(0);
    panelButtonHLayout->setSpacing(0);

    vLayout->setContentsMargins(0, 0, 0, 0);
    settingScrollArea->setContentsMargins(0, 0, 0, 0);
    scrollContents->setContentsMargins(0, 0, 0, 0);
    scrollContentsVLayout->setContentsMargins(0, 0, 0, 0);
    settingHLayout->setContentsMargins(0, 0, 0, 0);
    panelButtonHLayout->setContentsMargins(0, 0, 0, 0);

    scrollContentsVLayout->addWidget(tableEditSettingWidget);

    settingScrollArea->setWidgetResizable(true);
    settingScrollArea->setFrameShape(QFrame::Shape::NoFrame);
    scrollContentsVLayout->setSizeConstraint(QLayout::SetMinimumSize);

    settingScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    scrollContents->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    tableEditSettingWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    selectPageMenu->addAction(new QAction("Edit", tableEditSettingWidget));
    selectPageMenu->addAction(new QAction("Cell", tableCellSettingWidget));
    selectPageMenu->addAction(new QAction("Plot", tablePlotSettingWidget));
}

void TableArea::setupConnection()
{
    /* TableEditSettingWidget */
    connect(table->verticalHeader(), &QHeaderView::sectionCountChanged, tableEditSettingWidget, &TableEditSettingWidget::setRowCountSpinBox);
    connect(table->horizontalHeader(), &QHeaderView::sectionCountChanged, tableEditSettingWidget, &TableEditSettingWidget::setColCountSpinBox);
    connect(tableEditSettingWidget, &TableEditSettingWidget::pasteRequested, table, &GnuplotTable::pasteCell);
    connect(tableEditSettingWidget, &TableEditSettingWidget::copyRequested, table, &GnuplotTable::copyCell);
    connect(tableEditSettingWidget, &TableEditSettingWidget::cutRequested, table, &GnuplotTable::cutCell);
    connect(tableEditSettingWidget, &TableEditSettingWidget::rowSpinBoxChanged, table, &GnuplotTable::setRowCount);
    connect(tableEditSettingWidget, &TableEditSettingWidget::colSpinBoxChanged, table, &GnuplotTable::setColumnCount);
    connect(tableEditSettingWidget, &TableEditSettingWidget::cellFontSet, table, &GnuplotTable::setFont);
    connect(tableEditSettingWidget, &TableEditSettingWidget::cellTextSizeSet, table, &GnuplotTable::setSelectedTextSize);
    connect(tableEditSettingWidget, &TableEditSettingWidget::boldSet, table, &GnuplotTable::setSelectedTextBold);
    connect(tableEditSettingWidget, &TableEditSettingWidget::italicSet, table, &GnuplotTable::setSelectedTextItalic);
    connect(tableEditSettingWidget, &TableEditSettingWidget::underlineSet, table, &GnuplotTable::setSelectedTextUnderline);
    connect(tableEditSettingWidget, &TableEditSettingWidget::cellColorSet, table, &GnuplotTable::setSelectedCellColor);
    connect(tableEditSettingWidget, &TableEditSettingWidget::cellTextColorSet, table, &GnuplotTable::setSelectedTextColor);
    /* tableCellSettingWidget */
    connect(tableCellSettingWidget, &TableCellSettingWidget::textAlignmentSet, table, &GnuplotTable::setSelectedTextAlignment);
    connect(tableCellSettingWidget, &TableCellSettingWidget::reverseRowRequested, table, &GnuplotTable::reverseRow);
    connect(tableCellSettingWidget, &TableCellSettingWidget::reverseColRequested, table, &GnuplotTable::reverseCol);
    connect(tableCellSettingWidget, &TableCellSettingWidget::transposeCellRequested, table, &GnuplotTable::transposeCell);
    connect(tableCellSettingWidget, &TableCellSettingWidget::clearCellTextRequested, table, &GnuplotTable::clearCell);
    connect(tableCellSettingWidget, &TableCellSettingWidget::mergeCellsRequested, table, &GnuplotTable::mergeSelectedCells);
    connect(tableCellSettingWidget, &TableCellSettingWidget::splitCellsRequested, table, &GnuplotTable::splitSelectedCells);
    connect(tableCellSettingWidget, &TableCellSettingWidget::insertRowAboveRequested, table, &GnuplotTable::insertRowUp);
    connect(tableCellSettingWidget, &TableCellSettingWidget::insertRowBelowRequested, table, &GnuplotTable::insertRowDown);
    connect(tableCellSettingWidget, &TableCellSettingWidget::insertColLeftRequested, table, &GnuplotTable::insertColLeft);
    connect(tableCellSettingWidget, &TableCellSettingWidget::insertColRightRequested, table, &GnuplotTable::insertColRight);
    /* tablePlotSettingWidget */
    connect(tablePlotSettingWidget, &TablePlotSettingWidget::plotRequested, table, &GnuplotTable::plotSelectedData);
    connect(tablePlotSettingWidget, &TablePlotSettingWidget::plotOptionSet, table, &GnuplotTable::setOptionCmd);
}

void TableArea::resizeSettingPanel()
{
    int height = 0;

    for(int i = 0; i < scrollContentsVLayout->count(); ++i)
    {
        QWidget *w = scrollContentsVLayout->itemAt(i)->widget();
        if(w) height += w->sizeHint().height() + 2;
    }

    //if(settingScrollArea->horizontalScrollBar()->maximum() > 0) //水平スクロールバーが表示された場合
    //if(!settingScrollArea->horizontalScrollBar()->isVisible())
        height += settingScrollArea->horizontalScrollBar()->height();

    settingScrollArea->setFixedHeight(height);
}

void TableArea::expandSettingPage()
{
    static bool isExpanding = false;

    if(isExpanding)
    {
        scrollContentsVLayout->removeWidget(tableCellSettingWidget);
        scrollContentsVLayout->removeWidget(tablePlotSettingWidget);
        expandButton->setPixmap(expandPixmap);
    }
    else
    {
        scrollContentsVLayout->addWidget(tableEditSettingWidget);
        scrollContentsVLayout->addWidget(tableCellSettingWidget);
        scrollContentsVLayout->addWidget(tablePlotSettingWidget);
        expandButton->setPixmap(contractPixmap);
    }

    isExpanding = !isExpanding;

    resizeSettingPanel();
}

void TableArea::setSettingPage(QAction *action)
{
    const int layoutCount = scrollContentsVLayout->count();

    if(layoutCount > 1) return; //expand状態

    if(layoutCount > 0)
    {
        QWidget *w = scrollContentsVLayout->takeAt(0)->widget();
        if(w) w->setParent(nullptr);
    }

    scrollContentsVLayout->addWidget(qobject_cast<QWidget*>(action->parent()));

    resizeSettingPanel();
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
    hLayout->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);

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

    pasteButton->setPixmap(StandardPixmap::Edit::paste().scaled(iconSize, iconSize));
    copyButton->setPixmap(StandardPixmap::Edit::copy().scaled(iconSize, iconSize));
    cutButton->setPixmap(StandardPixmap::Edit::cut().scaled(iconSize, iconSize));
    boldButton->setPixmap(StandardPixmap::Edit::bold().scaled(iconSize, iconSize));
    italicButton->setPixmap(StandardPixmap::Edit::italic().scaled(iconSize, iconSize));
    underlineButton->setPixmap(StandardPixmap::Edit::underline().scaled(iconSize, iconSize));
    cellColorButton->setPixmap(StandardPixmap::Edit::backgroundColor().scaled(iconSize, iconSize));
    cellColorExpand->setPixmap(StandardPixmap::Icon::triangle().scaled(iconSize / 3, iconSize / 3));
    textColorButton->setPixmap(StandardPixmap::Edit::textColor().scaled(iconSize, iconSize));
    textColorExpand->setPixmap(StandardPixmap::Icon::triangle().scaled(iconSize / 3, iconSize / 3));
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
    setContentsMargins(0, 0, 0, 0);

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

    alignLeftButton->setPixmap(StandardPixmap::Edit::alignLeft().scaled(iconSize, iconSize));
    alignHCenterButton->setPixmap(StandardPixmap::Edit::alignHCenter().scaled(iconSize, iconSize));
    alignRightButton->setPixmap(StandardPixmap::Edit::alignRight().scaled(iconSize, iconSize));
    alignJustButton->setPixmap(StandardPixmap::Edit::alignJust().scaled(iconSize, iconSize));
    alignTopButton->setPixmap(StandardPixmap::Edit::alignTop().scaled(iconSize, iconSize));
    alignVCenterButton->setPixmap(StandardPixmap::Edit::alignVCenter().scaled(iconSize, iconSize));
    alignBottomButton->setPixmap(StandardPixmap::Edit::alignBottom().scaled(iconSize, iconSize));
    alignBaselineButton->setPixmap(StandardPixmap::Edit::alignBaseline().scaled(iconSize, iconSize));
    reverseRowButton->setPixmap(StandardPixmap::Edit::reverseRow().scaled(iconSize, iconSize));
    reverseColButton->setPixmap(StandardPixmap::Edit::reverseCol().scaled(iconSize, iconSize));
    transposeButton->setPixmap(StandardPixmap::Edit::reverseRow().transformed(QTransform().rotate(45)).scaled(iconSize, iconSize));
    clearTextButton->setPixmap(StandardPixmap::Edit::clear().scaled(iconSize, iconSize));
    mergeCellsButton->setPixmap(StandardPixmap::Edit::mergeCells().scaled(iconSize, iconSize));
    splitCellsButton->setPixmap(StandardPixmap::Edit::splitCells().scaled(iconSize, iconSize));
    insertRowAboveButton->setPixmap(StandardPixmap::Edit::insertRowAbove().scaled(iconSize, iconSize));
    insertRowBelowButton->setPixmap(StandardPixmap::Edit::insertRowBelow().scaled(iconSize, iconSize));
    insertColLeftButton->setPixmap(StandardPixmap::Edit::insertColLeft().scaled(iconSize, iconSize));
    insertColRightButton->setPixmap(StandardPixmap::Edit::insertColRight().scaled(iconSize, iconSize));

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






TablePlotSettingWidget::TablePlotSettingWidget(QWidget *parent)
    : QWidget(parent)
    , plotScatter2DMenu(new QMenu(this))
    , plotBar2DMenu(new QMenu(this))
    , plotCustomExpand(new mlayout::IconLabel(this))
    , optionCmdEdit(new QLineEdit(this))
{
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    setLayout(hLayout);
    hLayout->setSpacing(0);
    hLayout->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);

    mlayout::IconLabel *plotScatterButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *plotScatterExpand = new mlayout::IconLabel(this);
    mlayout::IconLabel *plotBarButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *plotBarExpand = new mlayout::IconLabel(this);
    mlayout::IconLabel *plotCustomButton = new mlayout::IconLabel(this);

    hLayout->addWidget(plotScatterButton);
    hLayout->addWidget(plotScatterExpand);
    hLayout->addWidget(plotBarButton);
    hLayout->addWidget(plotBarExpand);
    hLayout->addWidget(plotCustomButton);
    hLayout->addWidget(plotCustomExpand);
    hLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

    plotScatterButton->setPixmap(StandardPixmap::Graph::scatter().scaled(TableArea::iconSize, TableArea::iconSize));
    plotScatterExpand->setPixmap(StandardPixmap::Icon::triangle().scaled(TableArea::iconSize / 3, TableArea::iconSize / 3));
    plotBarButton->setPixmap(StandardPixmap::Graph::bar().scaled(TableArea::iconSize, TableArea::iconSize));
    plotBarExpand->setPixmap(StandardPixmap::Icon::triangle().scaled(TableArea::iconSize / 3, TableArea::iconSize / 3));
    plotCustomButton->setPixmap(StandardPixmap::Graph::script().scaled(TableArea::iconSize, TableArea::iconSize));
    plotCustomExpand->setPixmap(StandardPixmap::Icon::triangle().scaled(TableArea::iconSize / 3, TableArea::iconSize / 3));

    QPalette hoveredPalette(plotScatterButton->backgroundRole(), Qt::lightGray);
    plotScatterButton->setHoveredPalette(hoveredPalette);
    plotScatterExpand->setHoveredPalette(hoveredPalette);
    plotBarButton->setHoveredPalette(hoveredPalette);
    plotBarExpand->setHoveredPalette(hoveredPalette);
    plotCustomButton->setHoveredPalette(hoveredPalette);
    plotCustomExpand->setHoveredPalette(hoveredPalette);

    plotScatterButton->setAutoFillBackground(true);
    plotScatterExpand->setAutoFillBackground(true);
    plotBarButton->setAutoFillBackground(true);
    plotBarExpand->setAutoFillBackground(true);
    plotCustomButton->setAutoFillBackground(true);
    plotCustomExpand->setAutoFillBackground(true);

    plotScatterButton->setToolTip("plot scatter graph");
    plotBarButton->setToolTip("plot bar graph");
    plotCustomButton->setToolTip("plot custom graph");

    plotScatterExpand->setMenu(plotScatter2DMenu);
    plotBarExpand->setMenu(plotBar2DMenu);

    connect(plotScatterButton, &mlayout::IconLabel::released, [this](){ emit plotRequested(GnuplotTable::PlotType::LinesPoints2D); });
    connect(plotBarButton, &mlayout::IconLabel::released, [this](){ emit plotRequested(GnuplotTable::PlotType::Boxes2D); });
    connect(plotCustomButton, &mlayout::IconLabel::released, [this](){ emit plotRequested(GnuplotTable::PlotType::Custom) ;});
    connect(plotCustomExpand, &mlayout::IconLabel::released, this, &TablePlotSettingWidget::showOptionCmdEditor);
    connect(optionCmdEdit, &QLineEdit::textChanged, this, &TablePlotSettingWidget::plotOptionSet);
    connect(optionCmdEdit, &QLineEdit::editingFinished, optionCmdEdit, &QLineEdit::hide);

    setupMenu();
    optionCmdEdit->setWindowFlags(Qt::WindowType::Tool | Qt::FramelessWindowHint);
}

void TablePlotSettingWidget::showOptionCmdEditor()
{
    optionCmdEdit->setGeometry(cursor().pos().x(), cursor().pos().y(),
                               geometry().right() - plotCustomExpand->geometry().right(),
                               TableArea::iconSize);
    optionCmdEdit->show();
}

void TablePlotSettingWidget::setupMenu()
{
    QAction *scatter2dAct = plotScatter2DMenu->addAction("scatter 2d");
    QAction *lines2dAct = plotScatter2DMenu->addAction("line 2d");
    QAction *linespoints2dAct = plotScatter2DMenu->addAction("linespoints 2d");

    connect(scatter2dAct, &QAction::triggered, [this](){ emit plotRequested(GnuplotTable::PlotType::Scatter2D); });
    connect(lines2dAct, &QAction::triggered, [this](){ emit plotRequested(GnuplotTable::PlotType::Lines2D); });
    connect(linespoints2dAct, &QAction::triggered, [this](){ emit plotRequested(GnuplotTable::PlotType::LinesPoints2D); });

    QAction *boxes2dAct = plotBar2DMenu->addAction("boxes 2d");
    QAction *steps2dAct = plotBar2DMenu->addAction("steps 2d");
    QAction *fsteps2dAct = plotBar2DMenu->addAction("floor steps 2d");
    QAction *fillsteps2dAct = plotBar2DMenu->addAction("fill steps 2d");
    QAction *histeps2dAct = plotBar2DMenu->addAction("histogram steps 2d");
    QAction *impulses2dAct = plotBar2DMenu->addAction("impulses 2d");

    connect(boxes2dAct, &QAction::triggered, [this](){ emit plotRequested(GnuplotTable::PlotType::Boxes2D); });
    connect(steps2dAct, &QAction::triggered, [this](){ emit plotRequested(GnuplotTable::PlotType::Steps2D); });
    connect(fsteps2dAct, &QAction::triggered, [this](){ emit plotRequested(GnuplotTable::PlotType::FSteps2D); });
    connect(fillsteps2dAct, &QAction::triggered, [this](){ emit plotRequested(GnuplotTable::PlotType::FillSteps2D); });
    connect(histeps2dAct, &QAction::triggered, [this](){ emit plotRequested(GnuplotTable::PlotType::HiSteps2D); });
    connect(impulses2dAct, &QAction::triggered, [this](){ emit plotRequested(GnuplotTable::PlotType::Impulses2D); });
}











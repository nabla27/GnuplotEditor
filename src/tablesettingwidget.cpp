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
    connect(table->verticalHeader(), &QHeaderView::sectionCountChanged,
            tableSettingWidget, &TableSettingWidget::setRowCountSpinBox);
    connect(table->horizontalHeader(), &QHeaderView::sectionCountChanged,
            tableSettingWidget, &TableSettingWidget::setColCountSpinBox);
    connect(tableSettingWidget, &TableSettingWidget::rowSpinBoxChanged,
            table, &GnuplotTable::setRowCount);
    connect(tableSettingWidget, &TableSettingWidget::colSpinBoxChanged,
            table, &GnuplotTable::setColumnCount);
    connect(tableSettingWidget, &TableSettingWidget::pasteRequested,
            table, &GnuplotTable::pasteCell);
    connect(tableSettingWidget, &TableSettingWidget::copyRequested,
            table, &GnuplotTable::copyCell);
    connect(tableSettingWidget, &TableSettingWidget::cutRequested,
            table, &GnuplotTable::cutCell);
}

void TableArea::setCurrentSheet(int currentIndex)
{
    if(GnuplotTable *table = static_cast<GnuplotTable*>(sheetStack->widget(currentIndex)))
    {
        tableSettingWidget->setRowCountSpinBox(0, table->rowCount());
        tableSettingWidget->setColCountSpinBox(0, table->columnCount());
    }
}




TableSettingWidget::TableSettingWidget(QWidget *parent)
    : QWidget(parent)
    , rowCountSpinBox(new QSpinBox(this))
    , colCountSpinBox(new QSpinBox(this))
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QHBoxLayout *topHLayout = new QHBoxLayout;

    setLayout(vLayout);
    vLayout->addLayout(topHLayout);

    mlayout::IconLabel *pasteButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *copyButton = new mlayout::IconLabel(this);
    mlayout::IconLabel *cutButton = new mlayout::IconLabel(this);

    topHLayout->addWidget(new QLabel("Row"));
    topHLayout->addWidget(rowCountSpinBox);
    topHLayout->addWidget(new QLabel("Col"));
    topHLayout->addWidget(colCountSpinBox);
    topHLayout->addWidget(pasteButton);
    topHLayout->addWidget(copyButton);
    topHLayout->addWidget(cutButton);
    topHLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

    static constexpr int iconSize = 22;

    pasteButton->setPixmap(QPixmap(":/icon/edit_paste").scaled(iconSize, iconSize));
    copyButton->setPixmap(QPixmap(":/icon/edit_copy").scaled(iconSize, iconSize));
    cutButton->setPixmap(QPixmap(":/icon/edit_cut").scaled(iconSize, iconSize));
    pasteButton->setHoveredFrameShape(QFrame::Box);
    copyButton->setHoveredFrameShape(QFrame::Box);
    cutButton->setHoveredFrameShape(QFrame::Box);
    pasteButton->setToolTip("paste");
    copyButton->setToolTip("copy");
    cutButton->setToolTip("cut");

    vLayout->setContentsMargins(0, 0, 0, 0);
    topHLayout->setContentsMargins(0, 0, 0, 0);

    connect(rowCountSpinBox, &QSpinBox::editingFinished, this, &TableSettingWidget::emitChangedRowCount);
    connect(colCountSpinBox, &QSpinBox::editingFinished, this, &TableSettingWidget::emitChangedColCount);
    connect(pasteButton, &mlayout::IconLabel::released, this, &TableSettingWidget::pasteRequested);
    connect(copyButton, &mlayout::IconLabel::released, this, &TableSettingWidget::copyRequested);
    connect(cutButton, &mlayout::IconLabel::released, this, &TableSettingWidget::cutRequested);
}

void TableSettingWidget::emitChangedRowCount()
{
    emit rowSpinBoxChanged(rowCountSpinBox->value());
}

void TableSettingWidget::emitChangedColCount()
{
    emit colSpinBoxChanged(colCountSpinBox->value());
}

void TableSettingWidget::setRowCountSpinBox(const int, const int rowCount)
{
    rowCountSpinBox->setValue(rowCount);
}

void TableSettingWidget::setColCountSpinBox(const int, const int colCount)
{
    colCountSpinBox->setValue(colCount);
}






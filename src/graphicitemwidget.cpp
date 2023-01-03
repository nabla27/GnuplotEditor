#include "graphicitemwidget.h"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QAction>
#include <QMenu>
#include <QStackedWidget>
#include <QSplitter>
#include <QGraphicsScene>
#include <QApplication>
#include <QLineEdit>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QComboBox>

#include "layoutparts.h"
#include "standardpixmap.h"
#include "graphicitems.h"
#include "utility.h"
#include "logger.h"





GraphicsItemSettingWidget::GraphicsItemSettingWidget(QWidget *parent)
    : QWidget(parent)
    , panel(new GraphicsItemPanel(this))
    , tree(new GraphicsItemTree(this))
    , scrollArea(new GraphicsItemSettingArea(this))
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QSplitter *splitter = new QSplitter(Qt::Orientation::Vertical, this);

    setLayout(vLayout);

    vLayout->addWidget(panel);

    vLayout->addWidget(splitter);
    splitter->addWidget(tree);
    splitter->addWidget(scrollArea);

    tree->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    scrollArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);
    scrollArea->setContentsMargins(0, 0, 0, 0);

    scrollArea->setWidgetResizable(true);

    connect(tree, &GraphicsItemTree::changeSettingPageRequested, scrollArea, &GraphicsItemSettingArea::setCurrentWidget);
    connect(panel, &GraphicsItemPanel::createItemRequested, this, &GraphicsItemSettingWidget::createGraphicsItem);
}

void GraphicsItemSettingWidget::setCurrentItems(const QList<QGraphicsItem*>& items)
{
    tree->setCurrentItems(items);

    if(items.count() < 1) return;

    GraphicsItemWidget *w = this->items.value(items.at(0));

    if(w)
    {
        w->setupValue();
        if(scrollArea->widget() != w)
        {
            scrollArea->setCurrentWidget(w);
        }
    }
    else
    {
        __LOGOUT__("selected item does not have setting widget.", Logger::LogLevel::Debug);
    }
}

void GraphicsItemSettingWidget::createGraphicsItem(int type)
{
    QGraphicsItem *graphicsItem = nullptr;
    GraphicsTreeItem *treeItem = nullptr;
    GraphicsItemWidget *widget = nullptr;

    const GraphicsItemInfo::GraphicsItem itemEnumType = GraphicsItemInfo::GraphicsItem(type);

    switch(itemEnumType)
    {
    case GraphicsItemInfo::GraphicsItem::SimpleText:
    {

        GraphicsSimpleTextItem *i = new GraphicsSimpleTextItem(nullptr, this);
        widget = new GraphicsSimpleTextWidget(scrollArea, i);
        treeItem = new GraphicsTreeItem(tree, i);

        i->setText("Simple Text");
        graphicsItem = i;

        break;
    }
    case GraphicsItemInfo::GraphicsItem::PlainText:
    case GraphicsItemInfo::GraphicsItem::Arrow:
    {
        GraphicsArrowItem *i = new GraphicsArrowItem(nullptr, this);
        widget = new GraphicsArrowWidget(scrollArea, i);
        treeItem = new GraphicsTreeItem(tree, i);

        graphicsItem = i;
        break;
    }
    case GraphicsItemInfo::GraphicsItem::Ellipse:
    case GraphicsItemInfo::GraphicsItem::Polygon:
    case GraphicsItemInfo::GraphicsItem::Rect:
    case GraphicsItemInfo::GraphicsItem::Path:
    case GraphicsItemInfo::GraphicsItem::Image:
    case GraphicsItemInfo::GraphicsItem::Svg:
    case GraphicsItemInfo::GraphicsItem::Table:
    default:
    {
        /* plugin */
        break;
    }
    }

    if(graphicsItem)
    {
        emit graphicsItemCreated(graphicsItem);

        graphicsItem->setPos(graphicsItem->scene()->width() / 2, graphicsItem->scene()->height() / 2);

        scrollArea->setCurrentWidget(items.value(graphicsItem));

        items.insert(graphicsItem, widget);
        widget->setupValue();

        if(treeItem)
        {
            treeItem->setText(0, mutility::enumToString(itemEnumType));
            tree->addTopLevelItem(treeItem);
        }
    }
}









GraphicsItemSettingWidget::GraphicsItemPanel::GraphicsItemPanel(QWidget *parent)
    : QWidget(parent)
    , itemMenu(new QMenu(this))
{
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    mlayout::IconLabel *plusLabel = new mlayout::IconLabel(this);

    setLayout(hLayout);
    hLayout->addWidget(plusLabel);
    hLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

    static constexpr int iconSize = 20;
    plusLabel->setPixmap(StandardPixmap::Icon::plus().scaled(iconSize, iconSize));
    plusLabel->setAutoFillBackground(true);
    plusLabel->setHoveredPalette(QPalette(plusLabel->backgroundRole(), Qt::lightGray));

    hLayout->setSpacing(0);
    hLayout->setContentsMargins(0, 0, 0, 0);

    connect(plusLabel, &mlayout::IconLabel::released, this, &GraphicsItemPanel::execItemAddMenu);

    setupItemMenu();
}

void GraphicsItemSettingWidget::GraphicsItemPanel::execItemAddMenu()
{
    itemMenu->exec(cursor().pos());
}

void GraphicsItemSettingWidget::GraphicsItemPanel::setupItemMenu()
{
    const QStringList typeList = mutility::enumToStrings(GraphicsItemInfo::ItemType(0));

    for(int i = 0; i < typeList.count(); ++i)
    {
        QMenu *menu = new QMenu(typeList.at(i), itemMenu);
        itemMenu->addMenu(menu);

        for(int j = 0;; ++j)
        {
            const int itemIndex = 100 * i + j;

            const QString itemName(QMetaEnum::fromType<GraphicsItemInfo::GraphicsItem>().valueToKey(itemIndex));

            if(itemName.isEmpty()) break;

            auto emitAddItem = [this, itemIndex](){ emit createItemRequested(itemIndex); };

            menu->addAction(itemName, emitAddItem);
        }
    }
}









GraphicsItemSettingWidget::GraphicsItemTree::GraphicsItemTree(GraphicsItemSettingWidget *w)
    : QTreeWidget(w)
    , settingWidget(w)
    , contextMenu(new QMenu(this))
{
    setSelectionMode(SelectionMode::MultiSelection);

    setHeaderHidden(true);

    setContextMenuPolicy(Qt::CustomContextMenu);
    setupContextMenu();

    connect(this, &GraphicsItemTree::customContextMenuRequested, this, &GraphicsItemTree::onCustomContextMenu);
}

void GraphicsItemSettingWidget::GraphicsItemTree::setCurrentItems(const QList<QGraphicsItem *> items)
{
    clearSelection();

    const int itemCount = topLevelItemCount();
    for(int i = 0; i < itemCount; ++i)
    {
        GraphicsTreeItem *item = static_cast<GraphicsTreeItem*>(topLevelItem(i));
        if(items.contains(item->graphicsItem()))
        {
            item->setSelected(true);
        }
    }
}

void GraphicsItemSettingWidget::GraphicsItemTree::mouseReleaseEvent(QMouseEvent *event)
{
    QTreeWidget::mouseReleaseEvent(event);

    if(!currentItem()) return;

    QGraphicsItem *graphicsItem = static_cast<GraphicsTreeItem*>(currentItem())->graphicsItem();

    if(graphicsItem)
    {
        if(QApplication::keyboardModifiers() != Qt::ControlModifier)
        {
            clearSelection();
            graphicsItem->scene()->clearSelection();
        }

        currentItem()->setSelected(true);
        graphicsItem->setSelected(true);
        graphicsItem->scene()->blockSignals(false);

        emit changeSettingPageRequested(settingWidget->items.value(graphicsItem));
    }
}

void GraphicsItemSettingWidget::GraphicsItemTree::renameCurrentItem()
{
    QTreeWidgetItem *item = currentItem();
    if(!item) return;

    editItem(item, 0);
}

void GraphicsItemSettingWidget::GraphicsItemTree::removeCurrentItem()
{
    GraphicsTreeItem *item = static_cast<GraphicsTreeItem*>(currentItem());
    if(!item) return;

    QGraphicsItem *graphicsItem = item->graphicsItem();
    QWidget *widget = settingWidget->items.value(graphicsItem);

    const int index = indexOfTopLevelItem(item);

    if(index >= 0)
    {   //トップレベルアイテム(グループに属さないアイテム)
        delete takeTopLevelItem(index);
    }
    else
    {
        if(QTreeWidgetItem *parentItem = item->parent())
            parentItem->removeChild(item);
        else
            return;
    }

    settingWidget->items.remove(graphicsItem);

    graphicsItem->scene()->removeItem(graphicsItem);
    delete graphicsItem;

    if(settingWidget->scrollArea->widget() == widget)
    {
        settingWidget->scrollArea->takeWidget()->deleteLater();
    }
    else
    {
        widget->deleteLater();
    }
}

void GraphicsItemSettingWidget::GraphicsItemTree::setupContextMenu()
{
    QAction *renameAct = contextMenu->addAction("Rename");
    QAction *removeAct = contextMenu->addAction("Remove");

    connect(renameAct, &QAction::triggered, this, &GraphicsItemTree::renameCurrentItem);
    connect(removeAct, &QAction::triggered, this, &GraphicsItemTree::removeCurrentItem);
}

void GraphicsItemSettingWidget::GraphicsItemTree::onCustomContextMenu(const QPoint &pos)
{
    if(itemAt(pos))
    {
        contextMenu->exec(viewport()->mapToGlobal(pos));
    }
}









GraphicsItemSettingWidget::GraphicsTreeItem::GraphicsTreeItem(GraphicsTreeItem *parent, QGraphicsItem *item)
    : QTreeWidgetItem(parent)
    , _item(item)
{
    setFlags(flags() | Qt::ItemIsEditable);
}

GraphicsItemSettingWidget::GraphicsTreeItem::GraphicsTreeItem(QTreeWidget *treeWidget, QGraphicsItem *item)
    : QTreeWidgetItem(treeWidget)
    , _item(item)
{
    setFlags(flags() | Qt::ItemIsEditable);
}











GraphicsItemSettingWidget::GraphicsItemSettingArea::GraphicsItemSettingArea(QWidget *parent)
    : QScrollArea(parent)
{

}

void GraphicsItemSettingWidget::GraphicsItemSettingArea::setCurrentWidget(QWidget *w)
{
    if(w)
    {
        takeWidget();
        setWidget(w);
    }
}














GraphicsItemWidget::GraphicsItemWidget(QWidget *parent, QGraphicsItem *item)
    : QWidget(parent)
    , fLayout(new QFormLayout(this))

    , item(item)

    , visibleCheckBox(new QCheckBox(this))
    , posXSpinBox(new QDoubleSpinBox(this))
    , posYSpinBox(new QDoubleSpinBox(this))
    , posZSpinBox(new QDoubleSpinBox(this))
    , scaleSpinBox(new QDoubleSpinBox(this))
    , rotationSpinBox(new QDoubleSpinBox(this))
{
    setLayout(fLayout);


    fLayout->addRow("Visible", visibleCheckBox);
    fLayout->addRow("PosX", posXSpinBox);
    fLayout->addRow("PosY", posYSpinBox);
    fLayout->addRow("PosZ", posZSpinBox);
    fLayout->addRow("Scale", scaleSpinBox);
    fLayout->addRow("Rotation", rotationSpinBox);

    posXSpinBox->setMinimum(-10000);
    posXSpinBox->setMaximum(10000);
    posYSpinBox->setMinimum(-10000);
    posYSpinBox->setMaximum(10000);
    posZSpinBox->setMinimum(0);
    posZSpinBox->setMaximum(99);
    scaleSpinBox->setMinimum(0.01);
    scaleSpinBox->setSingleStep(0.1);
    rotationSpinBox->setMinimum(-180);
    rotationSpinBox->setMaximum(180);
    rotationSpinBox->setSingleStep(2);

    connect(visibleCheckBox, &QCheckBox::clicked, this, &GraphicsItemWidget::setItemVisible);
    connect(posXSpinBox, &QDoubleSpinBox::valueChanged, this, &GraphicsItemWidget::setXPos);
    connect(posYSpinBox, &QDoubleSpinBox::valueChanged, this, &GraphicsItemWidget::setYPos);
    connect(posZSpinBox, &QDoubleSpinBox::valueChanged, this, &GraphicsItemWidget::setZPos);
    connect(scaleSpinBox, &QDoubleSpinBox::valueChanged, this, &GraphicsItemWidget::setScale);
    connect(rotationSpinBox, &QDoubleSpinBox::valueChanged, this, &GraphicsItemWidget::setRotation);
}

GraphicsItemWidget::~GraphicsItemWidget()
{

}

void GraphicsItemWidget::setupValue()
{
    visibleCheckBox->setChecked(item->isVisible());
    posXSpinBox->setValue(item->x());
    posYSpinBox->setValue(item->y());
    posZSpinBox->setValue(item->zValue());
    scaleSpinBox->setValue(item->scale());
    rotationSpinBox->setValue(item->rotation());
}

void GraphicsItemWidget::setItemVisible(const bool visible)
{
    if(item) item->setVisible(visible);
}

void GraphicsItemWidget::setXPos(const qreal &xpos)
{
    if(item) item->setX(xpos);
}

void GraphicsItemWidget::setYPos(const qreal &ypos)
{
    if(item) item->setY(ypos);
}

void GraphicsItemWidget::setZPos(const qreal &zpos)
{
    if(item) item->setZValue(zpos);
}

void GraphicsItemWidget::setScale(const qreal &scale)
{
    if(item) item->setScale(scale);
}

void GraphicsItemWidget::setRotation(const qreal &rotate)
{
    if(item) item->setRotation(rotate);
}








AbstractGraphicsShapeWidget::AbstractGraphicsShapeWidget(QWidget *parent, QAbstractGraphicsShapeItem *item)
    : GraphicsItemWidget(parent, item)
    , item(item)
    , brushColorButton(new mlayout::ColorButton("Sample", QPalette::ButtonText, item->brush().color(), this))
    , brushStyleCombo(new QComboBox(this))
    , penButton(new mlayout::PenButton("Option", item->pen(), this))
{
    fLayout->addRow("Brush Color", brushColorButton);
    fLayout->addRow("Brush Style", brushStyleCombo);
    fLayout->addRow("Pen", penButton);

    for(int i = 0; i < mutility::getEnumCount(Qt::BrushStyle(0)) - 1; ++i)
        brushStyleCombo->addItem(mutility::enumToString(Qt::BrushStyle(i)));

    connect(brushColorButton, &mlayout::ColorButton::colorChanged, this, &AbstractGraphicsShapeWidget::setBrushColor);
    connect(brushStyleCombo, &QComboBox::currentIndexChanged, this, &AbstractGraphicsShapeWidget::setBrushStyle);
    connect(penButton, &mlayout::PenButton::penChanged, this, &AbstractGraphicsShapeWidget::setPen);
}

AbstractGraphicsShapeWidget::~AbstractGraphicsShapeWidget()
{

}

void AbstractGraphicsShapeWidget::setupValue()
{
    brushColorButton->setDialogColor(item->brush().color());
    brushStyleCombo->setCurrentIndex((int)item->brush().style());
    penButton->setDialogPen(item->pen());

    GraphicsItemWidget::setupValue();
}

void AbstractGraphicsShapeWidget::setBrushColor(const QColor &color)
{
    QBrush brush(item->brush());
    brush.setColor(color);
    item->setBrush(brush);
}

void AbstractGraphicsShapeWidget::setBrushStyle(const int enumIndex)
{
    QBrush brush(item->brush());
    brush.setStyle(Qt::BrushStyle(enumIndex));
    item->setBrush(brush);
}

void AbstractGraphicsShapeWidget::setPen(const QPen &pen)
{
    item->setPen(pen);
}














GraphicsSimpleTextWidget::GraphicsSimpleTextWidget(QWidget *parent, GraphicsSimpleTextItem *item)
    : AbstractGraphicsShapeWidget(parent, item)
    , item(item)

    , textEdit(new QLineEdit(this))
    , fontButton(new mlayout::FontButton("Sample", item->font(), this))
    , backgroundEnableCheck(new QCheckBox(this))
    , backgroundColorButton(new mlayout::ColorButton("Sample", QPalette::Button, item->backgroundColor(), this))
    , frameVisibleCheck(new QCheckBox(this))
    , framePenStyleCombo(new QComboBox(this))
    , frameWidthSpinBox(new QSpinBox(this))
    , frameColorButton(new mlayout::ColorButton("Sample", QPalette::Button, item->frameColor(), this))
{

    fLayout->addRow("Text", textEdit);
    fLayout->addRow("Font", fontButton);
    fLayout->addRow("Fill Background", backgroundEnableCheck);
    fLayout->addRow("Background Color", backgroundColorButton);
    fLayout->addRow("Frame Visible", frameVisibleCheck);
    fLayout->addRow("Frame Style", framePenStyleCombo);
    fLayout->addRow("Frame Width", frameWidthSpinBox);
    fLayout->addRow("Frame Color", frameColorButton);

    framePenStyleCombo->addItems(mutility::enumToStrings(Qt::PenStyle(0)));
    frameWidthSpinBox->setMinimum(0);

    connect(textEdit, &QLineEdit::textChanged, this, &GraphicsSimpleTextWidget::setText);
    connect(fontButton, &mlayout::FontButton::fontChanged, this, &GraphicsSimpleTextWidget::setItemFont);
    connect(backgroundEnableCheck, &QCheckBox::clicked, item, &GraphicsSimpleTextItem::setBackgroundEnable);
    connect(backgroundEnableCheck, &QCheckBox::clicked, backgroundColorButton, &mlayout::ColorButton::setVisible);
    connect(backgroundColorButton, &mlayout::ColorButton::colorChanged, item, &GraphicsSimpleTextItem::setBackgroundColor);
    connect(frameVisibleCheck, &QCheckBox::clicked, item, &GraphicsSimpleTextItem::setFrameVisible);
    connect(frameVisibleCheck, &QCheckBox::clicked, framePenStyleCombo, &QComboBox::setVisible);
    connect(frameVisibleCheck, &QCheckBox::clicked, frameWidthSpinBox, &QSpinBox::setVisible);
    connect(frameVisibleCheck, &QCheckBox::clicked, frameColorButton, &mlayout::ColorButton::setVisible);
    connect(framePenStyleCombo, &QComboBox::currentIndexChanged, [item](const int index) { item->setFramePenStyle(Qt::PenStyle(index)); });
    connect(frameWidthSpinBox, &QSpinBox::valueChanged, item, &GraphicsSimpleTextItem::setFrameWidth);
    connect(frameColorButton, &mlayout::ColorButton::colorChanged, item, &GraphicsSimpleTextItem::setFrameColor);
}

void GraphicsSimpleTextWidget::setupValue()
{
    textEdit->setText(item->text());
    fontButton->setDialogFont(item->font());

    const bool isFillBackground = item->isBackgroundEnable();
    backgroundEnableCheck->setChecked(isFillBackground);
    backgroundColorButton->setVisible(isFillBackground);

    backgroundColorButton->setDialogColor(item->backgroundColor());

    const bool isFrameVisible = item->isFrameVisible();
    frameVisibleCheck->setChecked(isFrameVisible);
    framePenStyleCombo->setVisible(isFrameVisible);
    frameWidthSpinBox->setVisible(isFrameVisible);
    frameColorButton->setVisible(isFrameVisible);

    framePenStyleCombo->setCurrentIndex((int)item->framePenStyle());
    frameWidthSpinBox->setValue(item->frameWidth());
    frameColorButton->setDialogColor(item->frameColor());

    AbstractGraphicsShapeWidget::setupValue();
}

void GraphicsSimpleTextWidget::setText(const QString &text)
{
    item->setText(text);
}

void GraphicsSimpleTextWidget::setItemFont(const QFont &font)
{
    item->setFont(font);
}









GraphicsArrowWidget::GraphicsArrowWidget(QWidget *parent, GraphicsArrowItem *item)
    : GraphicsItemWidget(parent, item)
    , item(item)
    , arrowTypeCombo(new QComboBox(this))
    , headTypeCombo(new QComboBox(this))
    , brushStyleCombo(new QComboBox(this))
    , brushColorButton(new mlayout::ColorButton("Sample", QPalette::Text, item->brushColor(), this))
    , penButton(new mlayout::PenButton("Option", item->pen(), this))
    , lengthSpinBox(new QDoubleSpinBox(this))
    , headAngleSpinBox(new QDoubleSpinBox(this))
    , headLengthSpinBox(new QDoubleSpinBox(this))
{
    fLayout->addRow("Arrow Type", arrowTypeCombo);
    fLayout->addRow("Head Type", headTypeCombo);
    fLayout->addRow("Brush Style", brushStyleCombo);
    fLayout->addRow("Brush Color", brushColorButton);
    fLayout->addRow("Pen", penButton);
    fLayout->addRow("Length", lengthSpinBox);
    fLayout->addRow("Head Angle", headAngleSpinBox);
    fLayout->addRow("Head Length", headLengthSpinBox);

    arrowTypeCombo->addItems(mutility::enumToStrings(GraphicsArrowItem::ArrowType(0)));
    headTypeCombo->addItems(mutility::enumToStrings(GraphicsArrowItem::HeadType(0)));
    brushStyleCombo->addItems(mutility::enumToStrings(Qt::BrushStyle(0)));
    lengthSpinBox->setMaximum(100000);
    lengthSpinBox->setSingleStep(5);
    headAngleSpinBox->setMaximum(360);
    headAngleSpinBox->setSingleStep(2);
    headLengthSpinBox->setMaximum(100000);
    headLengthSpinBox->setSingleStep(2);

    connect(arrowTypeCombo, &QComboBox::currentIndexChanged, item, &GraphicsArrowItem::setArrowType);
    connect(headTypeCombo, &QComboBox::currentIndexChanged, item, &GraphicsArrowItem::setHeadType);
    connect(brushStyleCombo, &QComboBox::currentIndexChanged, item, &GraphicsArrowItem::setBrushStyle);
    connect(brushColorButton, &mlayout::ColorButton::colorChanged, item, &GraphicsArrowItem::setBrushColor);
    connect(penButton, &mlayout::PenButton::penChanged, item, &GraphicsArrowItem::setPen);
    connect(lengthSpinBox, &QDoubleSpinBox::valueChanged, item, &GraphicsArrowItem::setLength);
    connect(headAngleSpinBox, &QDoubleSpinBox::valueChanged, item, &GraphicsArrowItem::setHeadAngle);
    connect(headLengthSpinBox, &QDoubleSpinBox::valueChanged, item, &GraphicsArrowItem::setHeadLength);
}

void GraphicsArrowWidget::setupValue()
{
    arrowTypeCombo->setCurrentIndex((int)item->arrowType());
    headTypeCombo->setCurrentIndex((int)item->headType());
    brushStyleCombo->setCurrentIndex((int)item->brushStyle());
    brushColorButton->setDialogColor(item->brushColor());
    penButton->setDialogPen(item->pen());
    lengthSpinBox->setValue(item->length());
    headAngleSpinBox->setValue(item->headAngle());
    headLengthSpinBox->setValue(item->headLength());

    GraphicsItemWidget::setupValue();
}











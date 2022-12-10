#include "editorwidget.h"

#include <QComboBox>
#include <QSplitter>

#include "layoutparts.h"
#include "standardpixmap.h"

//DEBUG
#include "texteditor.h"
#include "filetreewidget.h"
#include <QScrollArea>

EditorStackedWidget::EditorStackedWidget(EditorArea *editorArea, QSplitter *parentSplitter)
    : QScrollArea(parentSplitter)
    , editorArea(editorArea)
    , parentSplitter(parentSplitter)
    , contents(new QWidget(this))
    , editorListCombo(new QComboBox(this))
    , editorStack(new QStackedWidget(this))
    , executeScript(new mlayout::IconLabel(this))
{
    setWidgetResizable(true);

    editorListCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    connect(editorStack, &QStackedWidget::currentChanged, editorListCombo, &QComboBox::setCurrentIndex);
    connect(editorStack, &QStackedWidget::widgetRemoved, editorListCombo, &QComboBox::removeItem);
    connect(editorStack, &QStackedWidget::widgetRemoved, this, &EditorStackedWidget::removeItem);
    connect(editorStack, &QStackedWidget::currentChanged, this, &EditorStackedWidget::setCurrentItem);
    connect(editorListCombo, &QComboBox::currentIndexChanged, editorStack, &QStackedWidget::setCurrentIndex);

    setupLayout();
}

EditorStackedWidget::~EditorStackedWidget()
{
}

void EditorStackedWidget::setupLayout()
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QHBoxLayout *hLayout = new QHBoxLayout;
    mlayout::IconLabel *removeEditor = new mlayout::IconLabel(this);
    mlayout::IconLabel *separateAreaHorizontal = new mlayout::IconLabel(this);
    mlayout::IconLabel *separateAreaVertical = new mlayout::IconLabel(this);
    mlayout::IconLabel *closeArea = new mlayout::IconLabel(this);

    setWidget(contents);
    contents->setLayout(vLayout);
    vLayout->addLayout(hLayout);
    vLayout->addWidget(editorStack);

    hLayout->addWidget(editorListCombo);
    hLayout->addWidget(removeEditor);
    hLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));
    hLayout->addWidget(executeScript);
    //hLayout->addWidget(new mlayout::SeparatorLineWidget(this, Qt::Orientation::Vertical));
    hLayout->addWidget(separateAreaHorizontal);
    hLayout->addWidget(separateAreaVertical);
    hLayout->addWidget(closeArea);

    contents->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);
    hLayout->setSpacing(0);
    setContentsMargins(0, 0, 0, 0);
    vLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setContentsMargins(0, 0, 0, 0);

    static int iconSize = 20;
    removeEditor->setPixmap(StandardPixmap::Icon::remove().scaled(iconSize, iconSize));
    executeScript->setPixmap(StandardPixmap::Icon::execute().scaled(iconSize, iconSize));
    separateAreaHorizontal->setPixmap(StandardPixmap::Icon::separateH().scaled(iconSize, iconSize));
    separateAreaVertical->setPixmap(StandardPixmap::Icon::separateV().scaled(iconSize, iconSize));
    closeArea->setPixmap(StandardPixmap::Icon::closeWidget().scaled(iconSize, iconSize));

    QPalette buttonPalette(removeEditor->backgroundRole(), Qt::lightGray);
    removeEditor->setHoveredPalette(buttonPalette);
    executeScript->setHoveredPalette(buttonPalette);
    separateAreaHorizontal->setHoveredPalette(buttonPalette);
    separateAreaVertical->setHoveredPalette(buttonPalette);
    closeArea->setHoveredPalette(buttonPalette);

    removeEditor->setAutoFillBackground(true);
    executeScript->setAutoFillBackground(true);
    separateAreaHorizontal->setAutoFillBackground(true);
    separateAreaVertical->setAutoFillBackground(true);
    closeArea->setAutoFillBackground(true);

    executeScript->setHidden(true);

    connect(removeEditor, &mlayout::IconLabel::released,
            this, &EditorStackedWidget::removeCurrentWidget);
    connect(executeScript, &mlayout::IconLabel::released,
            this, &EditorStackedWidget::requestExecute);
    connect(separateAreaHorizontal, &mlayout::IconLabel::released,
            this, &EditorStackedWidget::separateAreaH);
    connect(separateAreaVertical, &mlayout::IconLabel::released,
            this, &EditorStackedWidget::separateAreaV);
    connect(closeArea, &mlayout::IconLabel::released,
            this, &EditorStackedWidget::closeThisArea);
}

void EditorStackedWidget::separateArea(const Qt::Orientation& orient)
{
    if(!parentSplitter) return;

    /* S:splitter, W:widget,
     *
     *  S1{W1}
     *   |
     *   | W1+=W2
     *   V
     * S1{S2{W1+W2}}
     *   |
     *   | W2+=W3
     *   V
     * S1{S2{W1+S3{W2+W3}}}
     */

    const int index = parentSplitter->indexOf(this);
    const QList<int> sizes = parentSplitter->sizes();

    setParent(nullptr);

    QSplitter *splitter = new QSplitter(orient);
    splitter->addWidget(this);
    splitter->addWidget(new EditorStackedWidget(editorArea, splitter));

    parentSplitter->insertWidget(index, splitter);
    parentSplitter->setSizes(sizes);

    splitter->setSizes(QList<int>() << splitter->width() / 2 << splitter->width() / 2);

    parentSplitter = splitter; //順番に気をつける
}


void EditorStackedWidget::separateAreaH()
{
    separateArea(Qt::Orientation::Horizontal);
}

void EditorStackedWidget::separateAreaV()
{
    separateArea(Qt::Orientation::Vertical);
}

void EditorStackedWidget::addWidget(QWidget *widget, TreeFileItem *item)
{
    if(!item) return;

    const int index = editorStack->indexOf(widget);

    //stackに無い
    if(index == -1)
    {
        editorStack->addWidget(widget);

        if(item->isSaved())
            editorListCombo->addItem(item->text(0));
        else
            editorListCombo->addItem(item->text(0) + "*");

        editorListCombo->setItemIcon(editorListCombo->count() - 1, item->icon(0));
        items.append(item);

        if((int)item->type() == (int)FileTreeWidget::TreeItemType::Script)
        {
            executeScript->setVisible(true);
        }
    }

    editorStack->setCurrentWidget(widget);

    connect(item, &TreeFileItem::editStateChanged, this, &EditorStackedWidget::changeEditState);
    //updateTimer()が発せられる間に，stackのeditorが変更された場合，異なるitemがrequestExecuteされる問題がある
    connect(item, &TreeFileItem::updated, this, &EditorStackedWidget::requestExecute);
}

void EditorStackedWidget::removeCurrentWidget()
{
    if(editorListCombo->count() < 1) return;

    QWidget *currentWidget = editorStack->currentWidget();

    editorStack->removeWidget(currentWidget);               //widgetをremoveしたらいくつかのスロットによりcomboも削除されたりなどする

    /* removeしただけでは，親がstackedWidgetのままになる．
     * そのままではEditorStackedWidgetが削除されたときに，
     * 一緒にdeleteされるため，再び選択された場合にcrashする．
     * parentを変更してStackedWidgetが削除されたときに親子関係によりdeleteされないようにする．*/
    currentWidget->setParent(nullptr);
    currentWidget->hide();
}

void EditorStackedWidget::removeAllStackedWidget()
{
    while(editorStack->count() > 0)
    {
        QWidget *w = editorStack->currentWidget();
        editorStack->removeWidget(w);

        w->setParent(nullptr);
        w->hide();
    }
}

void EditorStackedWidget::closeThisArea()
{
    setParent(nullptr);

    /*
     * Sp1(v) --- Sp2(w) --- w1
     *                    |- Sp3(h) --- Sp4(w) --- w2
     *                               |          |- w4
     *                               |- w3
     */

    /* splitterが空であれば削除 */
    if(parentSplitter)
        if(parentSplitter->count() == 0)
            parentSplitter->deleteLater();

    while(editorStack->count() > 0)
    {
        QWidget *w = editorStack->currentWidget();
        editorStack->removeWidget(w);
        w->setParent(nullptr);
        w->hide();
    }

    deleteLater();
}

void EditorStackedWidget::removeItem(const int index)
{
    if(index < 0 || items.count() <= index) return;

    TreeFileItem *item = items.at(index);
    item->disconnect(item, &TreeFileItem::editStateChanged, this, &EditorStackedWidget::changeEditState);
    item->disconnect(item, &TreeFileItem::updated, this, &EditorStackedWidget::requestExecute);
    items.removeAt(index);
}

TreeFileItem* EditorStackedWidget::currentTreeFileItem() const
{
    const int currentIndex = editorStack->currentIndex();

    if(currentIndex < 0 || items.count() <= currentIndex) return nullptr;

    return items.at(currentIndex);
}

void EditorStackedWidget::setCurrentItem(const int index)
{
    if(index < 0 || items.count() <= index) return;

    if((int)items.at(index)->type() == (int)FileTreeWidget::TreeItemType::Script)
    {
        executeScript->setVisible(true);
    }
    else
        executeScript->setVisible(false);
}

void EditorStackedWidget::requestExecute()
{
    emit editorArea->executeRequested(currentTreeFileItem());
}

void EditorStackedWidget::changeEditState(bool edited)
{
    const int index = editorStack->currentIndex();
    TreeFileItem *item = items.at(index);

    if(edited)
        editorListCombo->setItemText(index, item->text(0));
    else
    {
        const QString text = editorListCombo->itemText(index);
        if(!text.isEmpty() && text.back() != '*')
        {
            editorListCombo->setItemText(index, item->text(0) + "*");
        }
    }
}















EditorArea::EditorArea(QWidget *parent)
    : QWidget(parent)
    , vLayout(new QVBoxLayout(this))
    , rootSplitter(nullptr)
{
    setLayout(vLayout);
    setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);
}

void EditorArea::setWidget(QWidget *widget, TreeFileItem *item)
{
    if(!rootSplitter) setupRootSplitter();

    if(EditorStackedWidget *focusedStack = currentFocusedWidget<EditorStackedWidget>())
    {
        focusedStack->addWidget(widget, item);
        return;
    }
    else
    {
        if(EditorStackedWidget *stackChild = findChild<EditorStackedWidget*>())
        {
            stackChild->addWidget(widget, item);
            return;
        }

        EditorStackedWidget *w = new EditorStackedWidget(this, rootSplitter);
        rootSplitter->addWidget(w);
        w->addWidget(widget, item);
    }
}

void EditorArea::setupRootSplitter()
{
    rootSplitter = new QSplitter(Qt::Orientation::Vertical, this);

    rootSplitter->addWidget(new EditorStackedWidget(this, rootSplitter));
    vLayout->addWidget(rootSplitter);

    connect(rootSplitter, &QSplitter::destroyed, this, &EditorArea::resetRootSplitter);
}

TreeFileItem* EditorArea::currentTreeFileItem() const
{
    if(EditorStackedWidget *stack = currentFocusedWidget<EditorStackedWidget>())
    {
        return stack->currentTreeFileItem();
    }
    else
        return nullptr;
}

void EditorArea::splitFocusedWidgetVertically()
{
    if(EditorStackedWidget *w = currentFocusedWidget<EditorStackedWidget>())
        w->separateAreaV();
}

void EditorArea::splitFocusedWidgetHorizontally()
{
    if(EditorStackedWidget *w = currentFocusedWidget<EditorStackedWidget>())
        w->separateAreaH();
}

void EditorArea::closeFocusedWidget()
{
    if(EditorStackedWidget *w = currentFocusedWidget<EditorStackedWidget>())
        w->closeThisArea();
}

void EditorArea::removeAllStackedWidget()
{
    for(EditorStackedWidget *w : findChildren<EditorStackedWidget*>())
    {
        if(w) w->removeAllStackedWidget();
    }
}











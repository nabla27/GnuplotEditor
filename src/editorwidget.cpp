/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "editorwidget.h"

#include <QComboBox>
#include <QSplitter>
#include <QMimeData>
#include <QDrag>

#include "layoutparts.h"
#include "standardpixmap.h"
#include "logger.h"

//DEBUG
#include "texteditor.h"
#include "filetreewidget.h"
#include <QScrollArea>

EditorStackedWidget::EditorStackedWidget(EditorArea *editorArea, QSplitter *parentSplitter)
    : QScrollArea(parentSplitter)
    , editorArea(editorArea)
    , parentSplitter(parentSplitter)
    , contents(new QWidget(this))
    , fileComboBox(new FileComboBox(this))
    , editorStack(new QStackedWidget(this))
    , executeScript(new mlayout::IconLabel(this))
{
    setWidgetResizable(true);

    fileComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    connect(editorStack, &QStackedWidget::currentChanged, fileComboBox, &QComboBox::setCurrentIndex);
    connect(editorStack, &QStackedWidget::widgetRemoved, fileComboBox, &QComboBox::removeItem);
    connect(editorStack, &QStackedWidget::widgetRemoved, this, &EditorStackedWidget::removeItem);
    connect(editorStack, &QStackedWidget::currentChanged, this, &EditorStackedWidget::setCurrentItem);
    connect(fileComboBox, &EditorStackedWidget::FileComboBox::currentIndexChanged, editorStack, &QStackedWidget::setCurrentIndex);
    connect(fileComboBox, &EditorStackedWidget::FileComboBox::dropItemRequested, this, &EditorStackedWidget::addItem);

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

    hLayout->addWidget(fileComboBox);
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

    __LOGOUT__("editor area separated.", Logger::LogLevel::Info);
}


void EditorStackedWidget::separateAreaH()
{
    separateArea(Qt::Orientation::Horizontal);
}

void EditorStackedWidget::separateAreaV()
{
    separateArea(Qt::Orientation::Vertical);
}

void EditorStackedWidget::addItem(TreeFileItem *item)
{
    if(!item)
    {
        __LOGOUT__("nullptr item receive.", Logger::LogLevel::Warn);
        return;
    }

    QWidget *widget = item->widget();

    if(!widget)
    {
        __LOGOUT__("widget of the item \"" + item->fileInfo().absoluteFilePath() + "\" is nullptr", Logger::LogLevel::Warn);
        return;
    }

    const int index = editorStack->indexOf(widget);

    //stackに無い
    if(index == -1)
    {
        editorStack->addWidget(widget);

        if(item->isSaved())
            fileComboBox->addItem(item->text(0));
        else
            fileComboBox->addItem(item->text(0) + "*");

        fileComboBox->setItemIcon(fileComboBox->count() - 1, item->icon(0));
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

    __LOGOUT__("a new widget is added.", Logger::LogLevel::Info);
}

void EditorStackedWidget::removeCurrentWidget()
{
    if(fileComboBox->count() < 1)
    {
        __LOGOUT__("tried to remove current widget, but the editor list combobox was already empty", Logger::LogLevel::Warn);
        return;
    }

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

        __LOGOUT__("all stacked widget are removed.", Logger::LogLevel::Info);
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

    __LOGOUT__("this stacked widget is deleted.", Logger::LogLevel::Info);
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
    __LOGOUT__("execute requested from " + QString(EditorStackedWidget::staticMetaObject.className()), Logger::LogLevel::Info);

    emit editorArea->executeRequested(currentTreeFileItem());
}

void EditorStackedWidget::changeEditState(bool isSaved)
{
    const int index = editorStack->currentIndex();
    TreeFileItem *item = items.at(index);

    const QString text = fileComboBox->itemText(index);
    if(text.isEmpty()) return;

    const bool isPrevSaved = text.back() !='*';  //前の状態は保存状態であったか


    if(isSaved && !isPrevSaved)
    {
        //非保存状態 -> 保存状態
        fileComboBox->setItemText(index, item->text(0));
    }
    else if(!isSaved && isPrevSaved)
    {
        //保存状態 -> 非保存状態
        fileComboBox->setItemText(index, item->text(0) + "*");
    }

    if(isSaved != isPrevSaved)
        __LOGOUT__("the editor state of current widget is changed.", Logger::LogLevel::Info);
}














EditorStackedWidget::FileComboBox::FileComboBox(EditorStackedWidget *w)
    : QComboBox(w)
    , parentStackedWidget(w)
{
    setAcceptDrops(true);
}

/* mimeDataに一般のクラスオブジェクトをセットするとき，QVariantを用いてvoid*型でデータ渡し，
 * objectNameにクラス名を与える．
 */
void EditorStackedWidget::FileComboBox::mousePressEvent(QMouseEvent *e)
{
    if(e->button() != Qt::LeftButton) return;

    QMimeData *mimeData = new QMimeData;
    QDrag *drag = new QDrag(this);
    TreeFileItem *item = parentStackedWidget->currentTreeFileItem();

    mimeData->setParent(this);
    mimeData->setColorData(QVariant(QVariant::fromValue(static_cast<QObject*>(item))));
    mimeData->setObjectName(TreeFileItem::staticMetaObject.className());

    drag->setMimeData(mimeData);
    drag->setPixmap(item->icon(0).pixmap(25, 25));

    Qt::DropAction dropAction = drag->exec(Qt::DropAction::MoveAction);

    switch(dropAction)
    {
    case Qt::DropAction::IgnoreAction:
    {
        mimeData->deleteLater();
        drag->deleteLater();
        break;
    }
    default:
        break;
    }

    if(underMouse())
        QComboBox::mousePressEvent(e);  //マウスが置かれていた場合にのみ，comboBoxをpopupShowする
}

void EditorStackedWidget::FileComboBox::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->objectName() == QString(TreeFileItem::staticMetaObject.className()))
    {
        e->acceptProposedAction();
    }
    else
    {
        __LOGOUT__("invalid item dragged.", Logger::LogLevel::Info);
    }
}

void EditorStackedWidget::FileComboBox::dropEvent(QDropEvent *e)
{
    if(e->mimeData()->objectName() == QString(TreeFileItem::staticMetaObject.className()))
    {
        if(e->source()->metaObject()->className() != QString(EditorStackedWidget::FileComboBox::staticMetaObject.className()))
        {
            __LOGOUT__("source class is not different. this case is not surpported.", Logger::LogLevel::Debug);
            return;
        }
        emit dropItemRequested(qvariant_cast<TreeFileItem*>(e->mimeData()->colorData()));
    }
    else
    {
        __LOGOUT__("invalid item dropped.", Logger::LogLevel::Info);
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

EditorArea::~EditorArea()
{
    if(rootSplitter)
    {
        /* ASSERT failure in EditorArea:
         * "Called object is not of the correct type
         * (class destructor may have already run)", file C:\Qt\6.4.1\msvc2019_64\include\QtCore/qobjectdefs_impl.h, line 120
         */
        disconnect(rootSplitter, &QSplitter::destroyed, this, &EditorArea::resetRootSplitter);
    }
}

void EditorArea::setItem(TreeFileItem *item)
{
    if(!rootSplitter) setupRootSplitter();

    if(EditorStackedWidget *focusedStack = currentFocusedWidget<EditorStackedWidget>())
    {
        focusedStack->addItem(item);
        return;
    }
    else
    {
        if(EditorStackedWidget *stackChild = findChild<EditorStackedWidget*>())
        {
            stackChild->addItem(item);
            return;
        }

        EditorStackedWidget *w = new EditorStackedWidget(this, rootSplitter);
        rootSplitter->addWidget(w);
        w->addItem(item);
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











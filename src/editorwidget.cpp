#include "editorwidget.h"

#include <QComboBox>
#include <QSplitter>

#include "layoutparts.h"

//DEBUG
#include "texteditor.h"
#include <QScrollArea>

EditorStackedWidget::EditorStackedWidget(QWidget *parent, QSplitter *parentSplitter)
    : QScrollArea(parent)
    , parentSplitter(parentSplitter)
    , contents(new QWidget(this))
    , editorListCombo(new QComboBox(this))
    , editorStack(new QStackedWidget(this))
{
    setWidgetResizable(true);

    editorListCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    connect(editorStack, &QStackedWidget::currentChanged, editorListCombo, &QComboBox::setCurrentIndex);
    connect(editorListCombo, &QComboBox::currentIndexChanged, editorStack, &QStackedWidget::setCurrentIndex);

    setupLayout();
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
    removeEditor->setPixmap(QPixmap(":/icon/icon_remove").scaled(iconSize, iconSize));
    separateAreaHorizontal->setPixmap(QPixmap(":/icon/icon_separateh").scaled(iconSize, iconSize));
    separateAreaVertical->setPixmap(QPixmap(":/icon/icon_separatev").scaled(iconSize, iconSize));
    closeArea->setPixmap(QPixmap(":/icon/icon_closewidget").scaled(iconSize, iconSize));

    QPalette buttonPalette(removeEditor->backgroundRole(), Qt::lightGray);
    removeEditor->setHoveredPalette(buttonPalette);
    separateAreaHorizontal->setHoveredPalette(buttonPalette);
    separateAreaVertical->setHoveredPalette(buttonPalette);
    closeArea->setHoveredPalette(buttonPalette);

    removeEditor->setAutoFillBackground(true);
    separateAreaHorizontal->setAutoFillBackground(true);
    separateAreaVertical->setAutoFillBackground(true);
    closeArea->setAutoFillBackground(true);

    connect(removeEditor, &mlayout::IconLabel::released,
            this, &EditorStackedWidget::removeCurrentWidget);
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
    splitter->addWidget(new EditorStackedWidget(splitter, splitter));

    parentSplitter->insertWidget(index, splitter);
    parentSplitter->setSizes(sizes);

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

void EditorStackedWidget::addWidget(const QString& name,
                                    QWidget *widget,
                                    const QIcon& icon)
{
    const int index = editorStack->indexOf(widget);

    if(index == -1)
    {
        editorStack->addWidget(widget);
        editorListCombo->addItem(name);
        editorListCombo->setItemIcon(editorListCombo->count() - 1, icon);
    }

    editorStack->setCurrentWidget(widget);
}

void EditorStackedWidget::removeCurrentWidget()
{
    if(editorListCombo->count() < 1) return;

    QWidget *currentWidget = editorStack->currentWidget();
    editorListCombo->removeItem(editorListCombo->currentIndex());
    editorStack->removeWidget(currentWidget);
    currentWidget->hide();
}

void EditorStackedWidget::closeThisArea()
{
    setParent(nullptr);

    while(editorStack->count() > 0)
    {
        QWidget *w = editorStack->currentWidget();
        editorStack->removeWidget(w);
        w->setParent(nullptr);
        w->hide();
    }

    deleteLater();
}















EditorArea::EditorArea(QWidget *parent)
    : QWidget(parent)
{
    //DEBUG
    setWindowFlag(Qt::Window);
    setWindowFlag(Qt::WindowTitleHint, false);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QSplitter *splitter = new QSplitter(Qt::Orientation::Vertical, this);

    setLayout(vLayout);
    setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->addWidget(splitter);

    splitter->addWidget(new EditorStackedWidget(splitter, splitter));
}

QWidget* EditorArea::currentFocusedWidget(const QString& className) const
{
    QWidget *targetWidget = focusWidget();

    for(;;)
    {
        if(!targetWidget) return nullptr;

        if(targetWidget->metaObject()->className() == className)
        {
            return targetWidget;
        }
        else
        {
            targetWidget = targetWidget->parentWidget();
        }
    }
}

void EditorArea::setWidget(const QString& name, QWidget *widget, const QIcon& icon)
{
    if(EditorStackedWidget *focusedStack
            = static_cast<EditorStackedWidget*>(currentFocusedWidget("EditorStackedWidget")))
    {
        focusedStack->addWidget(name, widget, icon);
    }
}











#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H
#include <QStackedWidget>
#include <QScrollArea>

//DEBUG
#include <QDebug>
#include <QApplication>


class QComboBox;
class QSplitter;
class QVBoxLayout;
class TreeFileItem;
class EditorArea;
namespace mlayout { class IconLabel; }


class EditorStackedWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit EditorStackedWidget(EditorArea *editorArea, QSplitter *parentSplitter);
    ~EditorStackedWidget();

    QSize minimumSizeHint() const override { return QSize(0, 0); }

public:
    void addWidget(QWidget *widget, TreeFileItem *item);
    TreeFileItem* currentTreeFileItem() const;

    void separateAreaH();
    void separateAreaV();
    void closeThisArea();
    void removeAllStackedWidget();

private:
    void setupLayout();

private slots:
    void separateArea(const Qt::Orientation& orient);
    void removeCurrentWidget();
    void removeItem(const int index);
    void setCurrentItem(const int index);
    void requestExecute();

private:
    EditorArea *editorArea;
    QList<TreeFileItem*> items;
    QSplitter *parentSplitter;
    QWidget *contents;
    QComboBox *editorListCombo;
    QStackedWidget *editorStack;
    mlayout::IconLabel *executeScript;
};



class EditorArea : public QWidget
{
    Q_OBJECT
public:
    explicit EditorArea(QWidget *parent);

    QSize minimumSizeHint() const override { return QSize(0, 0); }

    void setWidget(QWidget *widget, TreeFileItem *item);

    TreeFileItem *currentTreeFileItem() const;

public slots:
    void splitFocusedWidgetVertically();
    void splitFocusedWidgetHorizontally();
    void closeFocusedWidget();
    void removeAllStackedWidget();

private:
    template <typename T>
    T* currentFocusedWidget() const;

private slots:
    void setupRootSplitter();
    void resetRootSplitter() { rootSplitter = nullptr; }

private:
    QVBoxLayout *vLayout;
    QSplitter *rootSplitter;

signals:
    void executeRequested(TreeFileItem *item);
};




template <typename T>
T* EditorArea::currentFocusedWidget() const
{
    QWidget *targetWidget = focusWidget();

    const QString className = T::staticMetaObject.className();

    for(;;)
    {
        if(!targetWidget) return nullptr;

        if(QString(targetWidget->metaObject()->className()) == className)
        {
            return static_cast<T*>(targetWidget);
        }
        else
        {
            targetWidget = targetWidget->parentWidget();
        }
    }
}



#endif // EDITORWIDGET_H

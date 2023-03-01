/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */


#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H
#include <QStackedWidget>
#include <QScrollArea>
#include <QComboBox>


//DEBUG
#include <QDebug>
#include <QApplication>


class QComboBox;
class QSplitter;
class QVBoxLayout;
class TreeFileItem;
class EditorArea;
class QLabel;
namespace mlayout { class IconLabel; }


class EditorStackedWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit EditorStackedWidget(EditorArea *editorArea, QSplitter *parentSplitter);
    ~EditorStackedWidget();

    QSize minimumSizeHint() const override { return QSize(0, 0); }

public:
    void addItem(TreeFileItem *item);
    TreeFileItem* currentTreeFileItem() const;

    void setStateToLoading();
    void setStateToLoaded();

    void separateAreaH();
    void separateAreaV();
    void closeThisArea();
    void removeAllStackedWidget();

private:
    void setupLayout();
    void connectFileItem(TreeFileItem*);

private slots:
    void separateArea(const Qt::Orientation& orient);
    void removeCurrentWidget();
    void removeItem(const int index);
    void setCurrentItem(const int index);
    void requestExecute();
    void changeEditState(bool edited);
    void changeFilePath(const QString& old, const QString& path);

protected:
    class FileComboBox;

private:
    inline static constexpr int iconSize = 20;
    QMovie *loadingMovie;
    int loadingCount = 0;

    EditorArea *editorArea;
    QList<TreeFileItem*> items;
    QSplitter *parentSplitter;
    QWidget *contents;
    FileComboBox *fileComboBox;
    QStackedWidget *editorStack;
    QLabel *loadingLabel;
    mlayout::IconLabel *executeScript;
};







class EditorStackedWidget::FileComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit FileComboBox(EditorStackedWidget *w);

public:
    static void setDragItem(TreeFileItem *item);

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e) override;

private:
    EditorStackedWidget *parentStackedWidget;

signals:
    void dropItemRequested(TreeFileItem *item);
};







class EditorArea : public QWidget
{
    Q_OBJECT
public:
    explicit EditorArea(QWidget *parent);
    ~EditorArea();

    QSize minimumSizeHint() const override { return QSize(0, 0); }

    void setItem(TreeFileItem *item);

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

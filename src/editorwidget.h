#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H
#include <QStackedWidget>
#include <QScrollArea>

//DEBUG
#include <QDebug>
#include <QApplication>


class QComboBox;
class QSplitter;


class EditorStackedWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit EditorStackedWidget(QWidget *parent, QSplitter *parentSplitter);

    QSize minimumSizeHint() const override { return QSize(0, 0); }

public:
    void addWidget(const QString& name, QWidget *widget, const QIcon& icon);

private:
    void setupLayout();

private slots:
    void separateArea(const Qt::Orientation& orient);
    void separateAreaH();
    void separateAreaV();
    void closeThisArea();
    void removeCurrentWidget();

private:
    QSplitter *parentSplitter;
    QWidget *contents;
    QComboBox *editorListCombo;
    QStackedWidget *editorStack;
};



class EditorArea : public QWidget
{
    Q_OBJECT
public:
    explicit EditorArea(QWidget *parent);

    QSize minimumSizeHint() const override { return QSize(0, 0); }

private:
    QWidget *currentFocusedWidget(const QString& className) const;

public:
    void setWidget(const QString& name, QWidget *widget, const QIcon& icon);
};

#endif // EDITORWIDGET_H

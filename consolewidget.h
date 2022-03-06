#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <QPlainTextEdit>
#include <QWidget>
#include <QCompleter>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QPainter>
#include <QTextBlock>
#include <QCursor>
#include <QKeyEvent>

class ConsoleWidget : public QPlainTextEdit
{
    Q_OBJECT

public:
    ConsoleWidget(QWidget *parent);

public:
    void setCompleter(QCompleter *c);
    QCompleter* completer() const;

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;

private:
    void insertCompletion(const QString& completion);
    QString textUnderCursor() const;

private:
    QCompleter *c = nullptr;



public:
    void leftAreaPaintEvent(QPaintEvent *event);
    int leftAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLeftAreaWidth(int newBlockCount);
    void updateLeftArea(const QRect& rect, int dy);

private:
    QWidget *leftArea;
};



class LeftArea : public QWidget
{
public:
    LeftArea(ConsoleWidget *console) : QWidget(console), console(console) {}
    ~LeftArea() {}

public:
    QSize sizeHint() const override{
        return QSize(console->leftAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override{
        console->leftAreaPaintEvent(event);
    }

private:
    ConsoleWidget *console;
};




















#endif // CONSOLEWIDGET_H

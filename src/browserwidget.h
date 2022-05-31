#ifndef BROWSERWIDGET_H
#define BROWSERWIDGET_H
#include <QTextBrowser>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QScrollBar>
#include "utility.h"

class BrowserSyntaxHighlighter;

class BrowserWidget : public QTextBrowser
{
    Q_OBJECT

public:
    explicit BrowserWidget(QWidget *parent = nullptr);
    ~BrowserWidget();

    enum class MessageType { GnuplotStdOut,
                             GnuplotStdErr,
                             FileSystemErr,
                             ProcessErr,
                             SystemErr,
                             Unknown };
    Q_ENUM(MessageType)

public:
    void outputText(const QString& text, const MessageType messageType);
    void grayOutAll();
    QSize minimumSizeHint() const override { return QSize(0, 0); }

private:
    BrowserSyntaxHighlighter *highlighter;
};








class BrowserSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit BrowserSyntaxHighlighter(QTextDocument *document);

public:
    void highlightBlock(const QString& text) override;

};

#endif // BROWSERWIDGET_H

#ifndef EDITORSYNTAXHIGHLIGHTER_H
#define EDITORSYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QString>

class EditorSyntaxHighlighter : public QSyntaxHighlighter
{
public:
    explicit EditorSyntaxHighlighter(QTextDocument *documtent);
    virtual ~EditorSyntaxHighlighter() {}

public:
    virtual void highlightBlock(const QString& text);
    void setFirstCmdColor(const QColor& color) { firstOrderFormat.setForeground(QColor(color)); rehighlight(); }
    void setCommentColor(const QColor& color) { commentsFormat.setForeground(QColor(color)); rehighlight(); }
    void setStringColor(const QColor& color) { quotationFormat.setForeground(QColor(color)); rehighlight(); }

private:
    QTextCharFormat firstOrderFormat;
    QStringList firstCmdList;
    QTextCharFormat commentsFormat;
    QTextCharFormat quotationFormat;
};


#endif // EDITORSYNTAXHIGHLIGHTER_H

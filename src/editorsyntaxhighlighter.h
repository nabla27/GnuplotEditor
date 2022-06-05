/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

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

public:
    QList<qsizetype> doubleQuotationLeft;
    QList<qsizetype> doubleQuotationRight;
    QList<qsizetype> singleQuotationLeft;
    QList<qsizetype> singleQuotationRight;
    qsizetype bracketLeft = 0;
    qsizetype bracketRight = 0;
    qsizetype squareBracketLeft = 0;
    qsizetype squareBracketRight = 0;

private:
    QTextCharFormat firstOrderFormat;
    QStringList firstCmdList;
    QTextCharFormat commentsFormat;
    QTextCharFormat quotationFormat;
};


#endif // EDITORSYNTAXHIGHLIGHTER_H

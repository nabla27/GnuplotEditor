/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "editorsyntaxhighlighter.h"

EditorSyntaxHighlighter::EditorSyntaxHighlighter(QTextDocument *document)
    : QSyntaxHighlighter(document)
{
    commentsFormat.setForeground(Qt::lightGray);
    commentsFormat.setFontItalic(true);

    firstOrderFormat.setForeground(Qt::cyan);
    firstOrderFormat.setFontWeight(QFont::Bold);

    quotationFormat.setForeground(QColor(255, 200, 0));

    firstCmdList << "break" <<
                    "cd" <<
                    "call" <<
                    "clear" <<
                    "continue" <<
                    "do" <<
                    "evaluate" <<
                    "exit" <<
                    "fit" <<
                    "help" <<
                    "history" <<
                    "if" <<
                    "for" <<
                    "import" <<
                    "load" <<
                    "lower" <<
                    "plot" <<
                    "print" <<
                    "printerr" <<
                    "pwdb" <<
                    "quit" <<
                    "raise" <<
                    "refresh" <<
                    "replot" <<
                    "reread" <<
                    "reset" <<
                    "save" <<
                    "set" <<
                    "show" <<
                    "shell" <<
                    "splot" <<
                    "stats" <<
                    "system" <<
                    "test" <<
                    "toggle" <<
                    "undefine" <<
                    "unset" <<
                    "update" <<
                    "while";
}

/* カーソル行の文字列がtextとして自動で渡される */
void EditorSyntaxHighlighter::highlightBlock(const QString &text)
{
    if(text.isEmpty()) return;

    /* firstCmdのハイライト */
    {
        /* firstCmdより空白スペースが前にあれば、ハイライトされない事に注意 */
        const qsizetype index = text.indexOf(' '); //空白の最初のインデックス
        if(index != qsizetype(-1))
        {
            const QString firstCmd = text.first(index);
            if(firstCmdList.contains(firstCmd))
            {
                setFormat(0, index, firstOrderFormat);
            }
        }
    }

    bracketLeft = text.count('(');
    bracketRight = text.count(')');
    squareBracketLeft = text.count('[');
    squareBracketRight = text.count(']');

    /* ダブルクォーテーション、シングルクォーテーション、シャープ#の位置を探す。
     * クォーテーション内部にシャープ記号がある場合はシャープのコメントアウトの効果を無効にするために、
     * 一緒に走査する必要がある */
    doubleQuotationLeft.clear();
    doubleQuotationRight.clear();
    singleQuotationLeft.clear();
    singleQuotationRight.clear();

    {
        qsizetype doubleQuotationCount = 0;
        qsizetype singleQuotationCount = 0;
        int commentStartPoint = -1;

        for(qsizetype index = 0; index < text.size(); ++index)
        {
            const QChar c = text.at(index);

            if(c == '\"')
            {
                if(doubleQuotationCount % 2 == 0)
                    doubleQuotationLeft << index;
                else
                    doubleQuotationRight << index;

                doubleQuotationCount++;
            }
            else if(c == '\'')
            {
                if(singleQuotationCount % 2 == 0)
                    singleQuotationLeft << index;
                else
                    singleQuotationRight << index;

                singleQuotationCount++;
            }
            else if(c == '#' && commentStartPoint == -1)
            {
                if(doubleQuotationLeft.size() == doubleQuotationRight.size() &&   //'#'はダブルクォーテーション内部でない
                   singleQuotationLeft.size() == singleQuotationRight.size())     //'#'はシングルクォーテーション内部でない
                    commentStartPoint = index;
            }
        }

        /* ダブルクォーテーション部分のハイライト */
        const qsizetype doubleQuotationPairCount = doubleQuotationRight.size();
        for(qsizetype i = 0; i < doubleQuotationPairCount; ++i)
        {
            setFormat(doubleQuotationLeft.at(i),                                   //クォーテーション開始位置
                      doubleQuotationRight.at(i) - doubleQuotationLeft.at(i) + 1,  //クォーテーション部分の長さ
                      quotationFormat);                                            //クォーテーション部分のフォーマット
        }

        /* コメント部分のハイライト */
        if(commentStartPoint != -1)
        {
            const qsizetype commentLength = text.size() - commentStartPoint + 1;  //コメント部分の文字列長
            setFormat(commentStartPoint,                                          //コメント部分の始まり
                      commentLength,                                              //コメント部分の長さ
                      commentsFormat);                                            //コメント部分のフォーマット
        }

    }
}

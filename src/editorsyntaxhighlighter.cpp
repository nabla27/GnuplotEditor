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

    /* ダブルクォーテーション部分のハイライト */
    if(text.contains('\"'))
    {
        QList<qsizetype> braLeft;                                              //左括弧のインデックスを格納
        QList<qsizetype> braRight;                                             //右括弧のインデックスを格納
        qsizetype count = 0;                                                   //括弧の数
        for(qsizetype index = 0; index < text.size(); ++index){                //テキストを一文字ずつ見る
            if(text.at(index) == '\"'){
                if(count % 2 == 0) braLeft << index;                           //偶数個目 = 左括弧
                else braRight << index;                                        //奇数個目 = 右括弧
                count++;                                                       //括弧の数インクリメント
            }
        }
        for(qsizetype i = 0; i < braRight.size(); ++i)                         //それぞれのクォーテーションにフォーマットを適用
        {
            setFormat(braLeft.at(i),                                           //クォーテーション開始位置
                      braRight.at(i) - braLeft.at(i) + 1,                      //クォーテーション部分の長さ
                      quotationFormat);                                        //クォーテーション部分のフォーマット
        }
    }

    /* コメントアウト部分のハイライト */
    const qsizetype commentsStartPoint = text.indexOf('#');                    //text内で#が最初に出てくるインデックス
    if(commentsStartPoint != qsizetype(-1))                                    //#が見つからなかった場合は-1を返される
    {
        const qsizetype commentsLength = text.size() - commentsStartPoint + 1; //コメント部分の文字列長
        setFormat(commentsStartPoint,                                          //コメント部分の始まり
                  commentsLength,                                              //コメント部分の長さ
                  commentsFormat);                                             //コメント部分のフォーマット
    }
}

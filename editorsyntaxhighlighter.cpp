#include "editorsyntaxhighlighter.h"

EditorSyntaxHighlighter::EditorSyntaxHighlighter(QTextDocument *document)
    : QSyntaxHighlighter(document)
{
    commentsFormat.setForeground(Qt::lightGray);
    commentsFormat.setFontItalic(true);

    firstOrderFormat.setForeground(Qt::cyan);
    firstOrderFormat.setFontWeight(QFont::Bold);

    quotationFormat.setForeground(QColor(255, 200, 0));

    firstOrderKeyWord << "\\bbreak\\b" <<
                         "\\bcd\\b" <<
                         "\\bcall\\b" <<
                         "\\bclear\\b" <<
                         "\\bcontinue\\b" <<
                         "\\bdo\\b" <<
                         "\\bevaluate\\b" <<
                         "\\bexit\\b" <<
                         "\\bfit\\b" <<
                         "\\bhelp\\b" <<
                         "\\bhistory\\b" <<
                         "\\bif\\b" <<
                         "\\bfor\\b" <<
                         "\\bimport\\b" <<
                         "\\bload\\b" <<
                         "\\blower\\b" <<
                         "\\bplot\\b" <<
                         "\\bprint\\b" <<
                         "\\bprinterr\\b" <<
                         "\\bpwd\\b" <<
                         "\\bquit\\b" <<
                         "\\braise\\b" <<
                         "\\brefresh\\b" <<
                         "\\breplot\\b" <<
                         "\\breread\\b" <<
                         "\\breset\\b" <<
                         "\\bsave\\b" <<
                         "\\bset\\b" <<
                         "\\bshow\\b" <<
                         "\\bshell\\b" <<
                         "\\bsplot\\b" <<
                         "\\bstats\\b" <<
                         "\\bsystem\\b" <<
                         "\\btest\\b" <<
                         "\\btoggle\\b" <<
                         "\\bundefine\\b" <<
                         "\\bunset\\b" <<
                         "\\bupdate\\b" <<
                         "\\bwhile\\b";
}

/* カーソル行の文字列がtextとして自動で渡される */
void EditorSyntaxHighlighter::highlightBlock(const QString &text)
{
    /* firstCmdのハイライト */
    foreach(const QString& pattern, firstOrderKeyWord)                         //ハイライト候補firstOrderKeyWordの中で一致するものがないのかループ
    {
        const QRegularExpression regexp(pattern);                              //ハイライト候補(QString)を正規表現regexpに
        QRegularExpressionMatchIterator i = regexp.globalMatch(text);          //text内で正規表現regexpと一致したイテレータ
        while(i.hasNext())                                                     //一致したイテレータが終わるまで
        {
            QRegularExpressionMatch match = i.next();                          //一致したイテレータの情報を順に進める
            setFormat(match.capturedStart(),                                   //ハイライトする語句の初めのインデックス
                      match.capturedLength(),                                  //ハイライトする語句の長さ
                      firstOrderFormat);                                       //ハイライトする語句のフォーマット
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

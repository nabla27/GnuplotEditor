/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "browserwidget.h"


BrowserSyntaxHighlighter::BrowserSyntaxHighlighter(QTextDocument *document)
    : QSyntaxHighlighter(document)
{

}

/* エディタ変更後に行単位のtextが行数分だけ呼ばれる */
void BrowserSyntaxHighlighter::highlightBlock(const QString &text)
{
    QRegularExpression errorRegExp("^\\[ .*Err \\]$");              // []の中にErrを含む
    QRegularExpression gnuplotStdOutExp("^\\[ .*GnuplotStdOut \\]$"); // []の中がGnuplotStdOut
    QRegularExpression stdRegExp("^\\[ .* \\]$");                   // []を含む

    if(currentBlockState() != -1) return;

    /* ハイライト */
    //メッセージタイプのある行でその行のstate(色)を設定する
    //その他の行では前の行のstateを継承して、色を設定する
    if(text.contains(errorRegExp)) {
        setCurrentBlockState(Qt::GlobalColor::red);
    }
    else if(text.contains(gnuplotStdOutExp)) {
        setCurrentBlockState(Qt::GlobalColor::blue);
    }
    else if(text.contains(stdRegExp))
        setCurrentBlockState(Qt::GlobalColor::black);
    else
    {
        setCurrentBlockState(previousBlockState());
        setFormat(0, text.length(), Qt::GlobalColor(this->previousBlockState()));
    }
}













//BrowserWidget::BrowserWidget(QWidget *parent)
//    : QTextBrowser(parent)
//{
//    highlighter = new BrowserSyntaxHighlighter(document());
//    setFrameShape(QFrame::Shape::NoFrame);
//}
//
//BrowserWidget::~BrowserWidget()
//{
//    delete highlighter;
//}
//
//void BrowserWidget::outputText(const QString &text, const MessageType messageType)
//{
//    append("\n[ " + enumToString(messageType) + " ]"); //append()が最後に改行含むよ!
//    append(text);
//    verticalScrollBar()->setValue(verticalScrollBar()->maximum());  //最後(一番下)までスクロールしよう!
//}
//
//void BrowserWidget::grayOutAll()
//{
//    if(toPlainText().isEmpty()) return;
//
//    QTextCursor tc = textCursor();
//    tc.setPosition(0, QTextCursor::MoveMode::MoveAnchor);
//    tc.setPosition(toPlainText().size() - 1, QTextCursor::MoveMode::KeepAnchor);
//    QTextCharFormat format = tc.charFormat();
//    format.setForeground(Qt::GlobalColor::gray);
//    tc.setCharFormat(format);
//}












































/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "texteditor.h"

#include <QCompleter>
#include <QStandardItemModel>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QToolTip>
#include <QPainter>
#include <QTextBlock>
#include <QTimer>

#include "editormanager.h"
#include "logger.h"





QHash<QChar, QChar> TextEditor::braketList =
{
    {'"', '"'},
    {'\'', '\''},
    {'(', ')'},
    {'[', ']'},
    {'<', '>'},
    {'{', '}'}
};

TextEditor::TextEditor(QWidget *parent)
    : QPlainTextEdit(parent)

    , toolTipTimer(new QTimer(this))

    , completer(new QCompleter(this))
    , editorManager(new EditorManager(nullptr))
    , cursorLineColor(QColor(50, 50, 50))

    , _lineNumberArea(new TextEditor::LineNumberArea(this))
{
    /* lineNumberArea */
    connect(this, &TextEditor::blockCountChanged, this, &TextEditor::updateLineNumberAreaWidth);
    connect(this, &TextEditor::updateRequest, this, &TextEditor::updateLineNumberArea);
    /* highlight */
    connect(this, &TextEditor::cursorPositionChanged, this, &TextEditor::highlight);


    setBackgroundColor(Qt::black);
    setTextColor(Qt::white);
}

TextEditor::~TextEditor()
{
    editorManager->deleteLater();
    editorManager = nullptr;
}

void TextEditor::setBackgroundColor(const QColor &color)
{
    QPalette p = this->palette();
    p.setColor(QPalette::Base, color);
    setPalette(p);
}

void TextEditor::setTextColor(const QColor &color)
{
    QPalette p = this->palette();
    p.setColor(QPalette::Text, color);
    setPalette(p);
}

void TextEditor::setWrap(const bool wrap)
{
    if(wrap)
        setLineWrapMode(LineWrapMode::WidgetWidth);
    else
        setLineWrapMode(LineWrapMode::NoWrap);
}

void TextEditor::setCursorLineColor(const QColor &color)
{
    cursorLineColor = color;
}

///* 選択範囲の行の先頭にテキストを挿入or削除する．
// * 既に行頭に指定のテキストがそれを削除し，なければ挿入する．*/
//void TextEditor::reverseTextLineHead(const QString &text)
//{
//    QTextCursor tc = textCursor();
//    const int start = tc.selectionStart();    //挿入位置の開始
//    int end = tc.selectionEnd();              //挿入されるごとにずれる．
//    const int textSize = text.size();
//
//    tc.setPosition(start, QTextCursor::MoveMode::MoveAnchor);
//
//    for(int i = 0; i < 500; ++i)
//    {
//        if(!tc.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::KeepAnchor, 1))
//            break;
//
//        if(tc.selectedText() == text)
//        {   //一致するば削除
//            tc.removeSelectedText();
//            end -= textSize;
//        }
//        else
//        {   //一致しなければ挿入
//            if(!tc.movePosition(QTextCursor::MoveOperation::StartOfLine, QTextCursor::MoveAnchor))
//                break;
//            tc.insertText(text);
//            end += textSize;
//        }
//
//        if(!tc.movePosition(QTextCursor::MoveOperation::NextBlock, QTextCursor::MoveMode::MoveAnchor))
//            break;
//
//        if(tc.position() > end)
//            return;
//    }
//
//    __LOGOUT__("An infinite loop occurred or the upper limit of the selection range was exceeded.", Logger::LogLevel::Warn);
//}

QString TextEditor::textUnderCursor()
{
    QTextCursor tc = textCursor();

    tc.movePosition(QTextCursor::MoveOperation::StartOfWord, QTextCursor::MoveAnchor);
    tc.movePosition(QTextCursor::MoveOperation::EndOfWord, QTextCursor::KeepAnchor);

    return tc.selectedText();
}

void TextEditor::setCompletionModel(QAbstractItemModel *model)
{
    completer->setModel(model);
    completer->setCompletionPrefix(textUnderCursor());

    /* 予測変換windowのサイズ設定．ピッタリだと見えないので+10する */
    const int width
            = completer->popup()->sizeHintForColumn(0)
            + completer->popup()->verticalScrollBar()->sizeHint().width()
            + 10;

    QRect cr = cursorRect();
    cr.setWidth(width);
    completer->complete(cr);
}

void TextEditor::setCompletionToolTip(const QString &tooltip)
{
    //前回表示されたtoolTipと同じであった場合，表示位置は移動しないので，一度空文字で非表示にする
    QToolTip::showText(QPoint(), "");

    if(tooltip.isEmpty()) return;

    if(completer->popup()->isVisible())
    {   //popupのhighlightによるtoolTip
        const int rightSpace                                         //画面に対するpopupWindowの右側の空き幅
                = screen()->geometry().width()
                - completer->popup()->rect().right();

        if(rightSpace > 100)
        {   //popupWindowの右側に十分にスペースがある場合，右側に表示
            const int rowIndex                                           //表示上のインデックス
                    = completer->popup()->currentIndex().row()           //モデルインデックス
                    - completer->popup()->verticalScrollBar()->value();
            const int rowPos
                    = rowIndex * completer->popup()->sizeHintForRow(0)   //表示上のインデックス * 一行の高さ
                    - 15;                                                //補正因子

            const QPoint pos
                    = completer->popup()->pos()
                    + QPoint(completer->popup()->width(), rowPos);

            QToolTip::showText(pos, tooltip, this);
        }
        else
        {   //下に表示
            const QPoint pos
                    = completer->popup()->pos()
                    + QPoint(0, completer->popup()->height());

            QToolTip::showText(pos, tooltip, this);
        }
    }
    else
    {   //textUnderCursorによるtoolTip
        QToolTip::showText(cursor().pos(), tooltip);
    }
}

void TextEditor::keyPressEvent(QKeyEvent *e)
{
    /* 予測変換中ではキー入力でエディタは変化させない */
    if(completer->popup()->isVisible())
    {
        switch(e->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return;
        default:
            break;
        }
    }

    /* Ctrl+Tabのよる移動 */
    if(e->keyCombination().keyboardModifiers() == Qt::KeyboardModifier::ShiftModifier &&
       e->keyCombination().key() == Qt::Key::Key_Backtab)
    {
        backTabOperation();
        return;
    }
    /* 括弧類の入力補完 */
    if(!e->text().isEmpty())
    {
        const QChar c = e->text().front();
        if(braketList.contains(c) || braketList.values().contains(c))
        {
            insertBraketOperation(c);
            return;
        }
    }
    /* 括弧類の削除補完 */
    if(e->key() == Qt::Key_Backspace)
    {
        removeBraketOperation(e);
        return;
    }
    /* 予測変換を無効にする場合 */
    {
        static QString eow("~!#$%{}|:<>?,;\\");
        if(!e->text().isEmpty() && eow.contains(e->text().front())) return;
    }
    /* 補完候補を変更する場合 */
    {
        static QString separator("|!%{}|:<>?,:\\ ()\"'");
        if(!e->text().isEmpty() && separator.contains(e->text().front()))
        {
            const QTextCursor tc = textCursor();
            emit changeCompletionModelRequested(document()->findBlockByLineNumber(tc.blockNumber()).text());
        }
    }

    QPlainTextEdit::keyPressEvent(e);

    completer->setCompletionPrefix(textUnderCursor());
}

void TextEditor::backTabOperation()
{
    QTextCursor tc = textCursor();

    if(tc.positionInBlock() == 0) return; //カーソルが行頭であるば無視

    constexpr int indent = 4;

    for(int i = 0; i < indent; ++i)
    {
        tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 1);
        const QString selectedText(tc.selectedText());

        if(selectedText == "\t")
        {
            if(i == 0)
            {   //すぐ左横がタブだった場合，そのタブ文字を削除して完了
                tc.removeSelectedText();
            }
            break;
        }
        else if(selectedText == " ")
        {   //左横が空白文字であった場合，空白を削除して横に移動する．最大，indent数だけ空白を削除する
            tc.removeSelectedText();
            continue;
        }
        else
        {   //文字であれば中断
            break;
        }

        tc.clearSelection();
        setTextCursor(tc);
    }
}

void TextEditor::insertBraketOperation(const QChar& c)
{
    QTextCursor tc = textCursor();

    if(braketList.values().contains(c))
    {
        if(tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor))
        {
            if(tc.selectedText() != QString(c))
            {
                tc.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
                tc.insertText(QString(c));
            }
        }
        else
        {
            if(braketList.contains(c))
            {
                tc.insertText(QString(c) + QString(braketList.value(c)));
                tc.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
            }
            else
                tc.insertText(QString(c));
        }
    }
    else if(braketList.contains(c))
    {
        tc.insertText(QString(c) + QString(braketList.value(c)));
        tc.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
    }

    tc.clearSelection();
    setTextCursor(tc);
}

void TextEditor::removeBraketOperation(QKeyEvent *e)
{
    QTextCursor tc = textCursor();

    if(!tc.selectedText().isEmpty() ||                                    //選択されている
       !tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor) ||    //左側に文字が存在しない
       !braketList.contains(tc.selectedText().front()))                   //左側の文字が対象の括弧でない
    {   //通常通り削除
        QPlainTextEdit::keyPressEvent(e);
        return;
    }

    const QChar leftChar = tc.selectedText().front();

    if(tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 2))   //右側に文字が存在しない
    {
        if(braketList.value(leftChar) == tc.selectedText().front())       //左側と右側が括弧のペアである．
        {
            const QString blockText(document()->findBlockByLineNumber(tc.blockNumber()).text());
            const int posInBlock = tc.positionInBlock();
            const QString leftText(blockText.first(posInBlock - 1));
            const QString rightText(blockText.last(blockText.size() - posInBlock + 1));

            int left, right;

            if(braketList.value(leftChar) == leftChar)
            {
                left = leftText.count(leftChar);
                right = rightText.count(leftChar);
            }
            else
            {
                left = leftText.count(leftChar) - leftText.count(braketList.value(leftChar));
                right = rightText.count(braketList.value(leftChar)) - rightText.count(leftChar);
            }

            if(left <= right)
            {
                tc.clearSelection();
                tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 2);
                tc.removeSelectedText();
            }
            else
            {
                QPlainTextEdit::keyPressEvent(e);
                return;
            }
        }
        else
        {
            QPlainTextEdit::keyPressEvent(e);
            return;
        }
    }
    else
    {   //移動できない == 一番右
        QPlainTextEdit::keyPressEvent(e);
        return;
    }

    tc.clearSelection();
    setTextCursor(tc);
}

void TextEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr(contentsRect());
    _lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), _lineNumberArea->lineNumberAreaWidth(), cr.height()));
}

void TextEditor::insertCompletion(const QString &completion)
{
    if(completer->widget() != this) return;

    const QString prefix = completer->completionPrefix();
    const int extra = completion.length() - prefix.length();

    QTextCursor tc = textCursor();
    tc.insertText(completion.right(extra));
    tc.movePosition(QTextCursor::EndOfWord, QTextCursor::MoveMode::MoveAnchor);
    setTextCursor(tc);
}

void TextEditor::highlight()
{
    if(isReadOnly()) return;

    QList<QTextEdit::ExtraSelection> extraSelections;

    highlightExtraSelections(extraSelections);

    setExtraSelections(extraSelections);
}

void TextEditor::highlightExtraSelections(QList<QTextEdit::ExtraSelection> &selections)
{
    QTextEdit::ExtraSelection cursorBlockSelection;

    cursorBlockSelection.format.setBackground(cursorLineColor);
    cursorBlockSelection.format.setProperty(QTextFormat::FullWidthSelection, true);
    cursorBlockSelection.cursor = textCursor();
    cursorBlockSelection.cursor.clearSelection();
    selections.append(cursorBlockSelection);
}

void TextEditor::updateLineNumberAreaWidth()
{
    setViewportMargins(_lineNumberArea->lineNumberAreaWidth(), 0, 0, 0);
}

void TextEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if(dy)
        _lineNumberArea->scroll(0, dy);
    else
        _lineNumberArea->update(0, rect.y(), _lineNumberArea->width(), rect.height());

    if(rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth();
}

void TextEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(_lineNumberArea);
    painter.fillRect(event->rect(), QColor(50, 50, 50));                               //LineNumberArea::QPaintEvent()の背景を塗りつぶす

    QTextBlock block(firstVisibleBlock());                                             //TextEditに表示されている最初の行で初期化(スクロールなどによって変更される)
    int blockNumber = block.blockNumber();                                             //TextEditに表示されている最初の行番号で初期化
    int top = qRound(blockBoundingRect(block).translated(contentOffset()).top());      //最初のTextBlockのtop位置で初期化
    int bottom = top + qRound(blockBoundingRect(block).height());                      //最初のTextBlockのbottom位置で初期化

    while(block.isValid() && top <= event->rect().bottom())                            //行番号表示エリアのbottomに到達するまでループ
    {
        if(block.isVisible() && top >= event->rect().top())
        {
            paintLineNumberArea(painter, QRect(0, top, _lineNumberArea->width(), fontMetrics().height()), blockNumber);
        }

        block = block.next();                                                          //次の行に移動
        top = bottom;                                                                  //topを変更(=以前のbottom)
        bottom = top + qRound(blockBoundingRect(block).height());                      //bottomを行の高さ分増やす
        ++blockNumber;                                                                 //行番号インクリメント
    }
}

void TextEditor::paintLineNumberArea(QPainter &painter, const QRect& rect, const int number)
{
    painter.setPen(QColor(180, 180, 180));
    painter.drawText(QRect(rect.left() + _lineNumberArea->leftMargin(), rect.top(),
                           rect.width(), rect.height()), Qt::AlignLeft, QString::number(number + 1));
}










TextEditor::LineNumberArea::LineNumberArea(TextEditor *editor)
    : QWidget(editor)
    , editor(editor)
{

}

QSize TextEditor::LineNumberArea::sizeHint() const
{
    return QSize(lineNumberAreaWidth() + _leftMargin, 0);
}

int TextEditor::LineNumberArea::lineNumberAreaWidth() const
{
    /* 桁数digitsを求める */
    int digits = 1;
    int max = qMax(1, editor->blockCount());
    while(max >= 10)
    {   //10で割れる回数 = 桁数
        max /= 10;
        ++digits;
    }

    int space = 3 + editor->fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space + _leftMargin;
}

void TextEditor::LineNumberArea::paintEvent(QPaintEvent *event)
{
    editor->lineNumberAreaPaintEvent(event);
}






















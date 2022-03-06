#include "consolewidget.h"

ConsoleWidget::ConsoleWidget(QWidget *parent)
    : QPlainTextEdit(parent)
{
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, Qt::GlobalColor::black);
    palette.setColor(QPalette::Text, Qt::GlobalColor::white);
    setPalette(palette);

    setFrameShape(QFrame::Shape::NoFrame);

    leftArea = new LeftArea(this);
    updateLeftAreaWidth(0);
    connect(this, &ConsoleWidget::blockCountChanged, this, &ConsoleWidget::updateLeftAreaWidth);
    connect(this, &ConsoleWidget::updateRequest, this, &ConsoleWidget::updateLeftArea);
}

void ConsoleWidget::setCompleter(QCompleter *completer)
{
    //新しくcompleterを設定
    if(c) c->disconnect(this);
    c = completer;

    if(!c) return;

    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setCaseSensitivity(Qt::CaseInsensitive);
}

QCompleter* ConsoleWidget::completer() const
{
    return c;
}

void ConsoleWidget::keyPressEvent(QKeyEvent *e)
{
    if(c && c->popup()->isVisible())
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

    bool isInvalidInput = false;
    const QTextCursor tc = textCursor();
    switch(e->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Home:
    case Qt::Key_End:
        break;
    case Qt::Key_Backspace:
        isInvalidInput = isInvalidInput || tc.selectedText().contains('?') || (tc.positionInBlock() == 0 && tc.selectedText().isEmpty()) || (tc.blockNumber() != blockCount() - 1);
        break;
    default:
        isInvalidInput = isInvalidInput || (tc.blockNumber() != blockCount() - 1);
        break;
    }

    const bool isShortcut = (e->modifiers().testFlag(Qt::ControlModifier) && e->key() == Qt::Key_E); //ctrl + E
    if((!c || !isShortcut) && !isInvalidInput) QPlainTextEdit::keyPressEvent(e);

    const bool ctr10rShift = e->modifiers().testFlag(Qt::ControlModifier) || e->modifiers().testFlag(Qt::ShiftModifier);
    if(!c || (ctr10rShift && e->text().isEmpty())) return;

    const QString eow("~!@#$%&^()_+{}|:[].,-=?'\\");
    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctr10rShift;
    const QString completionPrefix = textUnderCursor();

    if(!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 1 || eow.contains(e->text().right(1)))){
        c->popup()->hide();
        return;
    }

    //予測変換で参照する文字列(接頭辞)とポップアップの所恋インデックスの設定
    if(completionPrefix != c->completionPrefix()){
        c->setCompletionPrefix(completionPrefix);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }

    //ポップアップウィンドウのサイズ設定
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width());

    //ポップアップウィンドウの表示
    c->complete(cr);
}

void ConsoleWidget::focusInEvent(QFocusEvent *e)
{
    if(c) c->setWidget(this);

    QPlainTextEdit::focusInEvent(e);
}

void ConsoleWidget::insertCompletion(const QString &completion)
{
    if(c->widget() != this) return;

    QTextCursor tc = textCursor();

    const int extra = completion.length() - c->completionPrefix().length();

    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));

    setTextCursor(tc);
}

QString ConsoleWidget::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);

    return tc.selectedText();
}










int ConsoleWidget::leftAreaWidth()
{
    int space = 1 + fontMetrics().horizontalAdvance(QLatin1Char('>'));

    return space;
}

void ConsoleWidget::updateLeftAreaWidth(int)
{
    setViewportMargins(leftAreaWidth(), 0, 0, 0);
}

void ConsoleWidget::updateLeftArea(const QRect& rect, int dy)
{
    if(dy)
        leftArea->scroll(0, dy);
    else
        leftArea->update(0, rect.y(), leftArea->width(), rect.height());

    if(rect.contains(viewport()->rect()))
        updateLeftAreaWidth(0);
}

void ConsoleWidget::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    const QRect cr = contentsRect();
    leftArea->setGeometry(QRect(cr.left(), cr.top(), leftAreaWidth(), cr.height()));
}

void ConsoleWidget::leftAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(leftArea);
    painter.fillRect(event->rect(), QColor(0, 0, 0));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingRect(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while(block.isValid() && top <= event->rect().bottom())
    {
        if(block.isVisible() && top >= event->rect().top()){
            painter.setPen(Qt::cyan);
            painter.drawText(0, top, leftArea->width(), fontMetrics().height(), Qt::AlignLeft, "$");
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

















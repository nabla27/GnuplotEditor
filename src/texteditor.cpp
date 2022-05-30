#include "texteditor.h"
#include <QScrollBar>
#include <QToolTip>

TextEdit::TextEdit(QWidget *parent)
    : QPlainTextEdit(parent)
    , toolTipTimer(new QTimer(this))
{
    {
        connect(this, &TextEdit::blockCountChanged,
                this, &TextEdit::updateLineNumberAreaWidth);
        connect(this, &TextEdit::updateRequest,
                this, &TextEdit::updateLineNumberArea);
        connect(this, &TextEdit::cursorPositionChanged,
                this, &TextEdit::highlightLine);
    }
    {
        lineNumberArea = new ReLineNumberArea(this);
        updateLineNumberAreaWidth(0);
        highlightLine();
    }
    {//レイアウト色の設定
        QPalette palette = this->palette();
        palette.setColor(QPalette::Base, Qt::GlobalColor::black);
        palette.setColor(QPalette::Text, Qt::GlobalColor::white);
        setPalette(palette);
    }
    {
        gnuplotcpl = new gnuplot_cpl::GnuplotCompletionModel(nullptr);
        connect(this, &TextEdit::completionRequested, gnuplotcpl, &gnuplot_cpl::GnuplotCompletionModel::setCompletionList);
        connect(this, &TextEdit::currentFolderChanged, gnuplotcpl, &gnuplot_cpl::GnuplotCompletionModel::setParentFolder);
        connect(this, &TextEdit::toolTipRequested, gnuplotcpl, &gnuplot_cpl::GnuplotCompletionModel::setToolTip);
        connect(gnuplotcpl, &gnuplot_cpl::GnuplotCompletionModel::completionListSet, this, &TextEdit::setCompletionList);
        connect(gnuplotcpl, &gnuplot_cpl::GnuplotCompletionModel::toolTipSet, this, &TextEdit::setCompletionToolTip);
        connect(toolTipTimer, &QTimer::timeout, this, &TextEdit::requestToolTipForCursor);
        gnuplotcpl->moveToThread(&completionThread);
        completionThread.start();
        toolTipTimer->start(2000);
    }
    {//completerの初期設定
        setCompleter(new QCompleter());
    }
    {//ハイライトの初期設定
        textHighlight = new EditorSyntaxHighlighter(document());
    }
}

TextEdit::~TextEdit()
{
    completionThread.quit();
    completionThread.wait();
}

/* 右クリックしながらホイールで文字サイズ変更 */
void TextEdit::wheelEvent(QWheelEvent *event)
{
    if(event->buttons() == Qt::RightButton)
    {
        const int delta = event->angleDelta().y();
        QFont font = this->font();
        const int ps = (delta > 0) ? font.pointSize() + 1 : font.pointSize() - 1;

        font.setPointSize(ps);
        setFont(font);
        emit fontSizeChanged(ps);  //settingWidgetに変更を知らせる
    }

    QPlainTextEdit::wheelEvent(event);
}

void TextEdit::requestToopTipForCompletion(const QString &text)
{
    emit toolTipRequested(text, firstCmd);
}

void TextEdit::changeCompleterModel()
{
    const QString script = toPlainText().remove('\t');                                                //エディタの文字列をタブ文字を削除して取得
    const QList<QString> blockTextList = script.split('\n');                                          //各行ごとに文字列を取得(改行文字で分割)
    const int currentBlockNumber = textCursor().blockNumber();                                        //カーソル行の行数
    const QString currentBlockText = blockTextList.at(currentBlockNumber);                            //カーソル行の文字列
    const int positionInBlock = qMin(textCursor().positionInBlock(), currentBlockText.size());        //行頭からの位置(タブを消去しているので調整する)
    const QList<QString> textForwardCursor = (currentBlockText.isEmpty())
            ? QList<QString>()
            : currentBlockText.first(positionInBlock).split(' ');                                     //カーソル行のカーソル以前の文字列(空白で区切ってコマンドごとに分割される)
    QList<QString> firstCmdBlock;                                                                     //firstCmdを参照する行の文

    /* firstCmdを決定するために参照する行の文字列firstCmdBlockを決定する */
    if(currentBlockNumber == 0)                                                                         //エディタの行数が1のとき
        firstCmdBlock = blockTextList.at(0).split(' ');                                         //firstCmdを参照する行数は一列目
    else
        for(int block = currentBlockNumber - 1; block >= 0; --block)                            //カーソル行の手前の行から先頭まで順にさかのぼる
        {
            if(block == 0){                                                                     //先頭行にたどり着いたとき
                if(blockTextList.at(0).size() != 0 && blockTextList.at(0).back() == '\\'){      //その行に改行コマンドがあれば
                    firstCmdBlock = blockTextList.at(0).split(' '); break;                      //先頭行をfirstCmdBlockに
                }
                else{
                    firstCmdBlock = blockTextList.at(1).split(' '); break;
                }
            }
            if(blockTextList.at(block).size() == 0 || blockTextList.at(block).back() != '\\'){  //block行目の最後に改行コマンドがなければ、block+1行目をfirstCmdBlockにする
                firstCmdBlock = blockTextList.at(block + 1).split(' '); break;
            }
        }

    /* それぞれfirstCmd,beforeCmd,currentCmdを決定する */
    firstCmd = (firstCmdBlock.size() >= 2) ? firstCmdBlock.at(0) : "";
    const qsizetype currentCmdCount = textForwardCursor.size();
    beforeCmd = (currentCmdCount >= 2) ? textForwardCursor.at(currentCmdCount - 2) : "";
    currentCmd = (currentCmdCount > 0) ? textForwardCursor.at(currentCmdCount - 1) : "";

    /* コメント中では予測変換を無効にする */
    const qsizetype commentsStartPoint = currentBlockText.indexOf('#');                     //コメントのスタート位置
    if(commentsStartPoint != qsizetype(-1)){                                                //カーソル行にシャープ記号があれば
        if(positionInBlock >= commentsStartPoint){                                          //カーソル位置がコメントスタート位置より後であれば
            completer()->setModel(getEditCompleter_non()); return;                          //予測変換を無効に
        }
    }

    emit completionRequested(firstCmd, beforeCmd, textForwardCursor.size() - 1);
}


void TextEdit::setCompleter(QCompleter *completer)
{
    if(c) c->disconnect(this);

    c = completer;

    if(!c) return;

    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);   //変換候補をポップアップウィンドウで表示させる
    c->setCaseSensitivity(Qt::CaseInsensitive);          //マッチングのケース感度の設定

    QObject::connect(c, QOverload<const QString&>::of(&QCompleter::activated), this, &TextEdit::insertCompletion);
    QObject::connect(c, QOverload<const QString&>::of(&QCompleter::highlighted), this, &TextEdit::requestToopTipForCompletion);
}

QCompleter* TextEdit::completer() const
{
    return c;
}

void TextEdit::insertCompletion(QString completion)
{
    if(c->widget() != this) return;

    const QString prefix = c->completionPrefix().remove('"').remove('\'');
    completion.remove('"').remove('\'');
    const int extra = completion.length() - prefix.length();

    /* 予測変換入力後のカーソルの移動と挿入 */
    QTextCursor tc = textCursor();
    tc.movePosition(QTextCursor::Left);
    tc.setPosition(textCursor().position());
    tc.insertText(completion.right(extra));
    tc.movePosition(QTextCursor::EndOfWord, QTextCursor::MoveMode::MoveAnchor);
    setTextCursor(tc);
}

/* 予測玄関の候補を出すために参照するテキスト */
QString TextEdit::textUnderCursor() const
{
    return currentCmd;
}

void TextEdit::focusInEvent(QFocusEvent *e)
{
    if(c) c->setWidget(this);

    QPlainTextEdit::focusInEvent(e);
}

/* 括弧の補完 */
void TextEdit::bracketCompletion(QKeyEvent *e, const QChar beforeChar, const QChar nextChar)
{
    if(e->key() == Qt::Key_Backspace)
    {
        bracketDeletion(e, beforeChar, nextChar);
        return;
    }

    const QString keyText = e->text();

    if(keyText == "("){
            insertPlainText("()");
            moveCursor(QTextCursor::Left);
    }
    else if(keyText == "["){
            insertPlainText("[]");
            moveCursor(QTextCursor::Left);
    }
    else if(keyText == "\""){
            if(nextChar == '\"') { moveCursor(QTextCursor::Right); }
            else {
                insertPlainText("\"\"");
                moveCursor(QTextCursor::Left);
            }
    }
    else if(keyText == "'"){
            if(nextChar == '\''){ moveCursor(QTextCursor::Right); }
            else{
                insertPlainText("''");
                moveCursor(QTextCursor::Left);
            }
    }
    else if((keyText == ")" && nextChar == ')') ||
            (keyText == "]" && nextChar == ']') ||
            (keyText == "\"" && nextChar == '\\') ||
            (keyText == "'" && nextChar == '\'')){
        moveCursor(QTextCursor::Right);
    }
    else
        QPlainTextEdit::keyPressEvent(e);
}

void TextEdit::bracketDeletion(QKeyEvent *e, const QChar beforeChar, const QChar nextChar)
{
    auto removeBracket = [this]()
    {
        QTextCursor tc = textCursor();
        tc.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
        tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 2);
        tc.removeSelectedText();
        setTextCursor(tc);
    };

    if(beforeChar == '\'' && nextChar == '\'' &&
       textHighlight->singleQuotationLeft.size() && textHighlight->singleQuotationRight.size())
    {
        removeBracket();
    }
    else if(beforeChar == '\"' && nextChar == '\"' &&
            textHighlight->doubleQuotationLeft.size() && textHighlight->doubleQuotationRight.size())
    {
        removeBracket();
    }
    else if(beforeChar == '(' && nextChar == ')' &&
            textHighlight->bracketLeft == textHighlight->bracketRight)
    {
        removeBracket();
    }
    else if(beforeChar == '[' && nextChar == ']' &&
            textHighlight->squareBracketLeft == textHighlight->squareBracketRight)
    {
        removeBracket();
    }
    else
        QPlainTextEdit::keyPressEvent(e);

}

/* キーが入力された再の予測変換ボックスの設定 */
void TextEdit::keyPressEvent(QKeyEvent *e)
{
    /* 予測変換中でのキー入力無効。editorを変化させない */
    if(c && c->popup()->isVisible() && c->popup()->currentIndex().row() >= 0)
    {
        switch (e->key()) {
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

    const bool isShortcut = (e->modifiers().testFlag(Qt::ControlModifier) &&
                             e->key() == Qt::Key_E);

    /* 括弧類 [ ( ' " の補完 */
    if(!c || !isShortcut)
    {
        const QString text = toPlainText();
        const int cursorPos = textCursor().position();
        const QChar nextChar = (text.size() > cursorPos) ? text.at(cursorPos) : ' ';
        const QChar beforeChar = (cursorPos > 0) ? text.at(cursorPos - 1) : ' ';
        bracketCompletion(e, beforeChar, nextChar);
    }

    const bool ctr10rShift = (e->modifiers().testFlag(Qt::ControlModifier) ||
                              e->modifiers().testFlag(Qt::ShiftModifier));
    if(!c || (ctr10rShift && e->text().isEmpty())) return;

    static QString eow("~!@#$%{}|:<>?,./;\\"); //入力された時に予測変換を非表示にする文字
    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctr10rShift;

    /* 予測変換を非表示にする場合 */
    if(!isShortcut && (hasModifier ||
                       e->text().isEmpty() ||
                       eow.contains(e->text().right(1)))){
        return;
    }

    /* 予測変換候補を変更 */
    changeCompleterModel();
}

void TextEdit::setCompletionList(const QStringList& list)
{
    if(!c) return;

    QStringListModel *model = new QStringListModel();
    model->setStringList(list);

    completer()->setModel(model);

    c->setCompletionPrefix(textUnderCursor());
    c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));

    /* 予測変換ボックスのサイズ設定
     * popup()->sizeHintForColumn(0)でpopupの0列目、つまり変換候補の
     * 横幅(最大)を取得できる。+10しているのは、ぴったりであると、見えない場合があるため。*/
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0) + 10 + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr);
}

void TextEdit::setCompletionToolTip(const QString &text)
{
    if(text.isEmpty())
    {
        QToolTip::hideText();
        return;
    }

    if(c->popup()->isVisible())
    {
        //popup内でハイライトされている表示されている画面上でのインデックス(始点0)
        const int viewRowIndex = c->popup()->currentIndex().row() - c->popup()->verticalScrollBar()->value();

        if(viewRowIndex < 0) return; //ハイライトされたテキストがない場合、-1となって非表示とする

        /* 因子15はpopup()のtopLeftの座標とtoolTipのtopLeftの位置がずれを補正するたものもの
         * なんらかの正しい変換で改善できるかも*/
        const QPoint pos = c->popup()->pos() + QPoint(c->popup()->width(), viewRowIndex * c->popup()->sizeHintForRow(0) - 15);

        QToolTip::showText(pos, "-");  //toolTipが前と同じであれば、位置が変化しないので、一度リセットするために
        QToolTip::showText(pos, text);
    }
    else
    {
        QToolTip::showText(cursor().pos(), text);
    }
}

void TextEdit::requestToolTipForCursor()
{
    toolTipTimer->start(2000);

    if(isActiveWindow() && underMouse()) //表示させているものだけ
    {
        static QString previousFirstCmdForToolTip = "";
        static QString previousTextForToolTip = "";

        QTextCursor tc = cursorForPosition(mapFromGlobal(viewport()->cursor().pos()));

        tc.movePosition(QTextCursor::MoveOperation::WordLeft, QTextCursor::MoveAnchor);
        tc.movePosition(QTextCursor::MoveOperation::EndOfWord, QTextCursor::KeepAnchor);
        const QString textForToolTip = tc.selectedText();

        tc.movePosition(QTextCursor::MoveOperation::StartOfLine, QTextCursor::MoveAnchor);
        tc.movePosition(QTextCursor::MoveOperation::EndOfWord, QTextCursor::KeepAnchor);
        const QString firstCmdForToolTip = tc.selectedText();

        if(textForToolTip != previousTextForToolTip ||
           firstCmdForToolTip != previousFirstCmdForToolTip)
        {
            emit toolTipRequested(textForToolTip, firstCmdForToolTip);

            previousTextForToolTip = textForToolTip;
            previousFirstCmdForToolTip = firstCmdForToolTip;
        }
    }
}


























/* 行番号の表示幅を決定 */
int TextEdit::lineNumberAreaWidth()
{
    /* 桁数digitsを求める */
    int digits = 1;
    int max = qMax(1, blockCount());
    while(max >= 10){
        max /= 10;                     //10で割れる回数 = 桁数
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

/* 行番号の表示幅を設定 */
void TextEdit::updateLineNumberAreaWidth(int)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

/* スクロールなどに応じて行番号表示エリアを設定 */
void TextEdit::updateLineNumberArea(const QRect& rect, int dy)
{
    if(dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if(rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

/* TextEditのリサイズイベントに対応して行番号表示エリアを変更 */
void TextEdit::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

/* ハイライトする */
void TextEdit::highlightLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;        //ハイライトするblockリスト

    if(!isReadOnly())
    {
        QTextEdit::ExtraSelection currentSelection;          //カーソル行のハイライト

        currentSelection.format.setBackground(cursorLineColor);
        currentSelection.format.setProperty(QTextFormat::FullWidthSelection, true);
        currentSelection.cursor = textCursor();
        currentSelection.cursor.clearSelection();
        extraSelections.append(currentSelection);

        if(errorLineNumber >= 0)
        {
            QTextEdit::ExtraSelection errorSelection;        //エラー行のハイライト

            const QTextBlock errBlock = document()->findBlockByNumber(errorLineNumber);
            errorSelection.format.setUnderlineColor(Qt::red);
            errorSelection.format.setFontUnderline(true);
            errorSelection.format.setProperty(QTextFormat::FullWidthSelection, true);
            errorSelection.cursor = QTextCursor(errBlock);
            errorSelection.cursor.clearSelection();
            extraSelections.append(errorSelection);
        }
    }

    setExtraSelections(extraSelections);
}

/* 行表示エリアの番号表示 */
void TextEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor(50, 50, 50));                               //行表示番号エリアの背景色

    QTextBlock block = firstVisibleBlock();                                            //TextEditに表示されている最初の行で初期化(スクロールなどによって変更される)
    int blockNumber = block.blockNumber();                                             //TextEditに表示されている最初の行番号で初期化
    int top = qRound(blockBoundingRect(block).translated(contentOffset()).top());      //最初のTextBlockのtop位置で初期化
    int bottom = top + qRound(blockBoundingRect(block).height());                      //最初のTextBlockのbottom位置で初期化

    while(block.isValid() && top <= event->rect().bottom())                            //行番号表示エリアのbottomに到達するまでループ
    {
        if(block.isVisible() && top >= event->rect().top()){
            const QString number = QString::number(blockNumber + 1);                   //表示される行番号
            painter.setPen(QColor(180, 180, 180));                                     //行番号の色指定
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),  //行番号を表示させる
                             Qt::AlignLeft, number);
        }

        block = block.next();                                                          //次の行に移動
        top = bottom;                                                                  //topを変更(=以前のbottom)
        bottom = top + qRound(blockBoundingRect(block).height());                      //bottomを行の高さ分増やす
        ++blockNumber;                                                                 //行番号インクリメント
    }
}

#include "texteditor.h"
#include <QScrollBar>

TextEdit::TextEdit(QWidget *parent)
    : QPlainTextEdit(parent)
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
    {//completerの初期設定
        setCompleter(new QCompleter());
    }
    {//ハイライトの初期設定
        textHighlight = new EditorSyntaxHighlighter(document());
    }
}

TextEdit::~TextEdit()
{
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


void TextEdit::changeCompleterModel()
{
    firstCmd.clear(); beforeCmd.clear(); currentCmd.clear();

    const QString script = toPlainText().remove('\t');                                          //エディタの文字列をタブ文字を削除して取得
    const QList<QString> blockTextList = script.split('\n');                                    //各行ごとに文字列を取得(改行文字で分割)
    const qsizetype blockCount = blockTextList.size();                                          //行数
    const int currentBlockNumber = textCursor().blockNumber();                                  //カーソル行の行数
    const QString currentBlockText = blockTextList.at(currentBlockNumber);                      //カーソル行の文字列
    const int positionInBlock = qMin(textCursor().positionInBlock(), currentBlockText.size());  //行頭からの位置(タブを消去しているので調整する)
    const QList<QString> textForwardCursor = (currentBlockText.isEmpty())
            ? QList<QString>()
            : currentBlockText.first(positionInBlock).split(' ');                               //カーソル行のカーソル以前の文字列(空白で区切ってコマンドごとに分割される)
    QList<QString> firstCmdBlock;                                                               //firstCmdを参照する行の文字列(コマンドごとに分割される)

    /* firstCmdを決定するために参照する行の文字列firstCmdBlockを決定する */
    if(blockCount == 1)                                                                         //エディタの行数が1のとき
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

    /* ファイル名の予測変換 */
    if((firstCmd == "plot" ||
        firstCmd == "splot" ||
        firstCmd == "load" ||
        firstCmd == "cd") && currentCmd.size() >= 1 && currentCmd.front() == '\"')          //currenCmdがダブルクォーテーションから始まるとき
    {
        QDir dir(workingDirectory);
        dir.setNameFilters(QStringList() << "[a-zA-Z0-9]*");
        QFileInfoList fileList = dir.entryInfoList();                                       //workingDirectoryディレクトリのファイル情報を取得
        QStringList fileNameList;
        for(const QFileInfo& fileInfo : fileList){
            fileNameList << fileInfo.fileName();                                            //workingDirectoryPath内のファイル名をリストに格納
        }
        c->setModel(getCompleter(fileNameList));                                            //ファイル名一覧を予測変換候補に設定

        cursorMoveCount = 1;                                                                //予測変換決定後のカーソル移動数を1にする(ダブルクォーテーションをまたぐ)
        return;
    }

    /* カーソルが行頭にあるとき、予測変換を無効にする */
    if(positionInBlock == 0){
        completer()->setModel(getEditCompleter_non()); return;
    }
    else
        completer()->setModel(getEditCompleter_first());

    /* カーソル行に空白を含むとき、一旦予測変換を無効にする */
    if(currentBlockText.contains(' '))
        completer()->setModel(getEditCompleter_non());

    /* 先頭コマンドfirstCmdと直前のコマンドbeforeCmdをもとに予測変換を変更する */
    changeGnuplotCompleter(completer(), firstCmd, beforeCmd);
}


void TextEdit::setCompleter(QCompleter *completer)
{
    if(c) c->disconnect(this);

    c = completer;

    if(!c) return;

    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);   //変換候補をポップアップウィンドウで表示させる
    c->setCaseSensitivity(Qt::CaseInsensitive);          //マッチングのケース感度の設定

    QObject::connect(c, QOverload<const QString&>::of(&QCompleter::activated),
                     this, &TextEdit::insertCompletion);
}

QCompleter* TextEdit::completer() const
{
    return c;
}

void TextEdit::insertCompletion(const QString &completion)
{
    if(c->widget() != this) return;

    QTextCursor tc = textCursor();

    const int extra = completion.length() - c->completionPrefix().length();

    /* クォーテーションで囲まれる文字が入力される際には、カーソルを右に一つ移動させる */
    if(currentCmd.size() > 1 && (currentCmd.front() == '\'' || currentCmd.front() == '\"')){
        cursorMoveCount = 1;
    }

    /* 予測変換入力後のカーソルの移動と挿入 */
    tc.movePosition(QTextCursor::Left);
    tc.setPosition(textCursor().position());
    tc.insertText(completion.right(extra));
    tc.movePosition(QTextCursor::Right,
                    QTextCursor::MoveMode::MoveAnchor,
                    cursorMoveCount);
    setTextCursor(tc);
}

/* 予測玄関の候補を出すために参照するテキスト */
QString TextEdit::textUnderCursor() const
{
    QString text = currentCmd;
    return text.remove('\'').remove('\"');
}

void TextEdit::focusInEvent(QFocusEvent *e)
{
    if(c) c->setWidget(this);

    QPlainTextEdit::focusInEvent(e);
}

/* 括弧の補完 */
void TextEdit::bracketCompletion(QKeyEvent *e, const QChar nextChar)
{
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

/* キーが入力された再の予測変換ボックスの設定 */
void TextEdit::keyPressEvent(QKeyEvent *e)
{
    /* completer挿入時のカーソル移動数をリセット */
    if(!c->popup()->isVisible()) cursorMoveCount = 0;

    if(c && c->popup()->isVisible() && c->popup()->currentIndex().row() >= 0)
    {
        /* 予測変換中で以下のキー入力ではeditorを変化させない */
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
        bracketCompletion(e, nextChar);
    }

    const bool ctr10rShift = (e->modifiers().testFlag(Qt::ControlModifier) ||
                              e->modifiers().testFlag(Qt::ShiftModifier));
    if(!c || (ctr10rShift && e->text().isEmpty())) return;

    /* 予測変換候補を変更 */
    changeCompleterModel();

    static QString eow("~!@#$%{}|:<>?,./;\\"); //入力された時に予測変換を非表示にする文字
    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctr10rShift;
    const QString completionPrefix = textUnderCursor();

    /* 予測変換を非表示にする場合 */
    if(!isShortcut && (hasModifier ||
                       e->text().isEmpty() ||
                       eow.contains(e->text().right(1)))){
        return;
    }

    if(completionPrefix != c->completionPrefix()){
        c->setCompletionPrefix(completionPrefix);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }

    /* 予測変換ボックスのサイズ設定
     * popup()->sizeHintForColumn(0)でpopupの0列目、つまり変換候補の
     * 横幅(最大)を取得できる。+10しているのは、ぴったりであると、見えない場合があるため。*/
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0) + 10 + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr);
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

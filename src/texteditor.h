/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QPlainTextEdit>


class QAbstractItemModel;
class QCompleter;
class LineNumberArea;
class EditorManager;


/* --- マウスカーソルによるtextUnderCursor()のtoolTip表示 ---
 * QTimer::timeout(void) -> TextEditor::requestToolTip()
 *                       -> TextEditor::toolTipRequested(QString)
 *                       -> CompletionModel::requestToolTip(QString)
 *                       -> CompletionModel::toolTipSet(QString)
 *                       -> TextEditor::setCompletionToolTip(QString)
 *
 * --- 補完popupWindowのhighlightによるtoolTip表示 ---
 * QCompleter::highlighted(QString) -> TextEditor::toolTipRequested(QString)
 *                                  -> CompletionModel::requestToolTip(QString)
 *                                  -> CompletionModel::toolTipSet(QString)
 *                                  -> TextEditor::setCompletionToolTip(QString)
 *
 * --- 補完を決定する ---
 * QCompleter::activated(QString) -> TextEditor::insertCompletion(QString)
 *
 * --- keyboard入力による補完候補の決定 ---
 * TextEditor::keyPressEvent(QPressEvent*) -> TextEditor::changeCompletionModelRequested(QString)
 *                                         -> CompletionModel::requestModel(QString)
 *                                         -> CompletionModel::modelSet(QAbstractitemModel*)
 *                                         -> TextEditor::setCompletionModel(QAbstractItemModel*)
 *
 *
 * --- 行表示エリアのpaintEvent ----
 * TextEditor::blockCountChanged(int) -> TextEditor::updateLineNumberAreaWidth(void)
 * TextEditor::updateRequested(QRect,int) -> TextEditor::updateLineNumerArea(QRect, int)
 * LineNumberArea::paintEvent(QPaintEvent*) -> TextEditor::lineNumberAreaPaintEvent(QPaintEvent*)
 *                                          -> TextEditor::paintLineNumberArea(QPainter,QRect,int)
 *
 * --- ハイライト ---
 * TextEditor::cursorPositionChanged(void) -> TextEditor::highlight(void)
 *                                         -> TextEditor::highlightExtraSelections(QList<QTextEdit::ExtraSelection>)
 *                                         -> TextEditor::setExtraSelections(QList<QTextEdit::ExtraSelection>)
 *
 */



class TextEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit TextEditor(QWidget *parent);
    virtual ~TextEditor();

public slots:
    void setBackgroundColor(const QColor& color);
    void setTextColor(const QColor& color);
    void setWrap(const bool wrap);
    void setCursorLineColor(const QColor& color);

public:
    QString textUnderCursor();
    void setCompletionModel(QAbstractItemModel *model);
    void setCompletionToolTip(const QString& tooltip);

protected:
    virtual void keyPressEvent(QKeyEvent *e) override;
    virtual void resizeEvent(QResizeEvent *e) override;

    virtual void highlightExtraSelections(QList<QTextEdit::ExtraSelection>& selections);
    virtual void paintLineNumberArea(QPainter& painter, const QRect& rect, const int number);

    class LineNumberArea;
    TextEditor::LineNumberArea *const lineNumberArea() { return _lineNumberArea; }

protected slots:
    /* completion */
    void insertCompletion(const QString& completion);
    /* highlight */
    void highlight();

private slots:
    /* lineNumberArea */
    void updateLineNumberAreaWidth();
    void updateLineNumberArea(const QRect& rect, int dy);
    void lineNumberAreaPaintEvent(QPaintEvent *event);

private:
    void backTabOperation();
    void insertBraketOperation(const QChar& c);
    void removeBraketOperation(QKeyEvent *e);

public:
    static QHash<QChar, QChar> braketList;
    static QThread funcThread;

private:
    QTimer *toolTipTimer;

    QCompleter *completer;
    EditorManager *editorManager;
    QColor cursorLineColor;

    LineNumberArea *const _lineNumberArea;

signals:
    void toolTipRequested(const QString& text);
    void changeCompletionModelRequested(const QString& text);
};






class TextEditor::LineNumberArea : public QWidget
{
    Q_OBJECT
public:
    explicit LineNumberArea(TextEditor *editor);

public:
    QSize sizeHint() const override;
    int lineNumberAreaWidth() const;
    void setLeftMaring(int m) { _leftMargin = m; }
    int leftMargin() const { return _leftMargin; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    TextEditor *editor;
    int _leftMargin = 10;
};








#endif // TEXTEDITOR_H

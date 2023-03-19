/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QPlainTextEdit>
#include "editorsyntaxhighlighter.h"
#include <QThread>



class QCompleter;
namespace gnuplot_cpl { class GnuplotCompletionModel; }




class TextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    TextEdit(QWidget *parent = nullptr);
    ~TextEdit();

public slots:
    void setBackgroundColor(const QColor& color) { QPalette palette = this->palette(); palette.setColor(QPalette::Base, color); setPalette(palette); }
    void setTextColor(const QColor& color) { QPalette palette = this->palette(); palette.setColor(QPalette::Text, color); setPalette(palette); }
    void setTabSpace(const double& space) { setTabStopDistance(space); }
    void setWrap(const bool wrap) { setLineWrapMode(QPlainTextEdit::LineWrapMode(wrap)); }
    void setMainCmdColor(const QColor& color) { textHighlight->setFirstCmdColor(color); }
    void setCommentColor(const QColor& color) { textHighlight->setCommentColor(color); }
    void setStringColor(const QColor& color) { textHighlight->setStringColor(color); }
    void setParentFolderPath(const QString& path) { emit currentFolderChanged(path); }
    void insertToSelectedHeadBlock(const QString& text) const;
    void removeFromSelectedHeadBlock(const QString& text) const;
    void reverseSelectedCommentState() const;

public:
    QString textUnderCursor() const;

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    EditorSyntaxHighlighter *textHighlight;

    /* completer */
public:
    void setCompleter(QCompleter *c);
    QCompleter *completer() const;

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;

private:
    void insertCompletion(QString completion);           //予測変換で決定された文字をエディタに挿入
    void bracketCompletion(QKeyEvent *e, const QChar beforeChar, const QChar nextChar); //括弧の補完 [ ( ' "
    void bracketDeletion(QKeyEvent *e, const QChar beforeChar, const QChar nextChar);   //括弧の削除
    void changeCompleterModel();                                //入力コマンドから予測変換候補を変更

private:
    static const QChar tmlChar;
    QCompleter *c = nullptr;
    QString firstCmd = "";
    QString previousCmd = "";
    QString currentCmd = "";

    QTimer *toolTipTimer;

    gnuplot_cpl::GnuplotCompletionModel *gnuplotcpl;
    QThread completionThread;
    void requestToopTipForCompletion(const QString& text);
    void requestToolTipForCursor();

signals:
    void completionRequested(const QString& firstCmd, const QString& preCmd, const int index);
    void toolTipRequested(const QString& text, const QString& firstCmd, const QString& previousCmd);
    void currentFolderChanged(const QString& path);

private slots:
    void setCompletionList(const QStringList& list);
    void setCompletionToolTip(const QString& text);

    /* lineNumer */
public:
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    void resetErrorLineNumber() { errorLineNumber = -1; highlightLine(); }
    void setErrorLineNumber(const int num) { errorLineNumber = num - 1; highlightLine(); }

public slots:
    void highlightLine();
    void setCursorLineColor(const QColor& color) { cursorLineColor = color; highlightLine(); }

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;
    int errorLineNumber = -1;
    QColor cursorLineColor = QColor(50, 50, 50);

signals:
    void fontSizeChanged(const int ps);
};






class ReLineNumberArea : public QWidget
{
public:
    ReLineNumberArea(TextEdit *editor) : QWidget(editor), codeEditor(editor) {}
    ~ReLineNumberArea() {}
public:
    QSize sizeHint() const override{
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }
protected:
    void paintEvent(QPaintEvent *event) override{
        codeEditor->lineNumberAreaPaintEvent(event);
    }
private:
    TextEdit *codeEditor;
};







#endif // TEXTEIDT_H

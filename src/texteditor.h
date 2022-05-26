#ifndef TextEditOR_H
#define TextEditOR_H

#include <QPlainTextEdit>
#include <QCompleter>
#include <QAbstractItemView>
#include <QTextBlock>
#include <QPainter>
#include <QPalette>
#include <QDir>
#include "editorsyntaxhighlighter.h"
#include "gnuplotcpl.h"
#include "utility.h"

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
    void setParentFolderPath(const QString& path);

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
    void insertCompletion(const QString& completion);           //予測変換で決定された文字をエディタに挿入
    QString textUnderCursor() const;                            //予測変換を出すために参照するテキスト
    void bracketCompletion(QKeyEvent *e, const QChar beforeChar, const QChar nextChar); //括弧の補完 [ ( ' "
    void bracketDeletion(QKeyEvent *e, const QChar beforeChar, const QChar nextChar);   //括弧の削除
    void changeCompleterModel();                                //入力コマンドから予測変換候補を変更
    void getFilesRecursively(const QString& parentPath, const QString& folderPath, QStringList& list); //folderPath内のファイルについて、parentPathからの相対パスをlistに追加する。

private:
    QCompleter *c = nullptr;
    int cursorMoveCount = 0;
    QString firstCmd = "";
    QString beforeCmd = "";
    QString currentCmd = "";
    QStringList currentDirFileList;

    /* lineNumer */
public:
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    void setErrorLineNumber(const int num) { errorLineNumber = num;  }

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







#endif // TextEditOR_H

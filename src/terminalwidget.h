/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef TERMINALWIDGET_H
#define TERMINALWIDGET_H

#include <QPlainTextEdit>
#include <QTabWidget>
#include <QThread>
#include <QDialog>
#include "textcodec.h"

class QProcess;
namespace mlayout { class IconLabel; }



class TerminalWidget : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit TerminalWidget(QWidget *parent);
    virtual ~TerminalWidget();

    QSize minimumSizeHint() const override { return QSize(0, 0); }

public:
    virtual void setProcess(QProcess *process); //does not take ownership of the process
    QProcess *const process() const { return _process; }
    TextCodec::CharCode charCode() const { return _charCode; }
    QString programPath() const;
    QStringList startArguments() const;

public slots:
    void setProgramPath(const QString& path);
    void setStartArguments(const QStringList& arguments);
    void setCharCode(const TextCodec::CharCode& code);

    void setPromptString(const QString& text);
    void clearTerminal();

protected slots:
    virtual void readProcessStdOut();
    virtual void readProcessStdErr();

protected:
    virtual void write();
    QProcess *_process = nullptr;
    TextCodec::CharCode _charCode;

    virtual void keyPressEvent(QKeyEvent *e) override;

    void insertOutput(const QString& out, const QTextCharFormat& format);

protected:
    void insertCompletion(const QString& completion);
    QString textUnderCursor() const;

private:
    void insertHistory(const int stride);

private:
    QVector<QString> histories;
    int historyIndex = 0;





    /* left area paint */
public:
    void leftAreaPaintEvent(QPaintEvent *event);
    int leftAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;
    QSet<int> outputLines;

private slots:
    void updateLeftAreaWidth(int newBlockCount);
    void updateLeftArea(const QRect& rect, int dy);

private:
    QWidget *leftArea;
    QString _promptString;
    QChar _promptChar = '$';








    /* static detail option */
public slots:
protected:
    //inline static int maxCompletionList = 100;
    inline static QColor stdOutTextColor = Qt::green;
    inline static QColor stdErrTextColor = Qt::red;
};









class LeftArea : public QWidget
{
public:
    LeftArea(TerminalWidget *terminal) : QWidget(terminal), terminal(terminal) {}
    ~LeftArea() {}

public:
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    TerminalWidget *terminal;
};







namespace gnuplot_cpl { class GnuplotCompletionModel; }
class GnuplotTermimal : public TerminalWidget
{
    Q_OBJECT
public:
    explicit GnuplotTermimal(QWidget *parent);
    ~GnuplotTermimal();

public:
    void setProcess(QProcess *process) override;

protected slots:
    void readProcessStdOut() override;
    void readProcessStdErr() override;

protected:
    void write() override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void setCompletionList(const QStringList& list);

private:
    gnuplot_cpl::GnuplotCompletionModel *gnuplotcpl;
    QThread completionThread;

signals:
    void completionRequested(const QString& firstCmd, const QString& preCmd, const int index);
};










class LogBrowserWidget;
class TerminalTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TerminalTabWidget(QWidget *parent);
    QSize sizeHint() const override;

public:
    GnuplotTermimal *const gnuplotTerminal() const { return _gnuplotTerminal; }
    LogBrowserWidget *const logBrowser() const { return _logBrowser; }

public slots:
    /* from corner widget */
    void addTerminal();
    void settingTerminal();
    void clearDisplay();
    void killTerminal();

private slots:
    void setPageToLogBrowser();
    void setCornerWidgetOption(const int index);

private:
    GnuplotTermimal *const _gnuplotTerminal;
    LogBrowserWidget *const _logBrowser;

    class CornerWidget;
    CornerWidget *const _cornerWidget;

    class TerminalSettingDialog;
    TerminalSettingDialog *const _terminalSettingDialog;
};













class TerminalTabWidget::CornerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CornerWidget(QWidget *parent);

public:
    void enableAddTerminal(const bool enable);
    void enableSetTerminal(const bool enable);
    void enableClearDisplay(const bool enable);
    void enableKillTerminal(const bool enable);

private:
    mlayout::IconLabel *const plusLabel;
    mlayout::IconLabel *const settingLabel;
    mlayout::IconLabel *const clearLabel;
    mlayout::IconLabel *const trashCanLabel;

signals:
    void addTerminalRequested();
    void setTerminalRequested();
    void clearDisplayRequested();
    void killTerminalRequested();
};










class QLineEdit;
class QComboBox;
class TerminalTabWidget::TerminalSettingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TerminalSettingDialog(QWidget *parent);
    QSize sizeHint() const { return QSize(QDialog::sizeHint().width() * 1.5, QDialog::sizeHint().height()); }

public:
    void setProgramPath(const QString& path);
    void setProgramName(const QString& name);
    void setArguments(const QString& arguments);
    void setCharCode(const TextCodec::CharCode& code);

    QString programPath() const;
    QString programName() const;
    QString arguments() const;
    TextCodec::CharCode charCode() const;

private slots:
    void setProgramNameFromPath(const QString& path);
    void setPathFromDialog();

private:
    QLineEdit *pathEdit;
    QLineEdit *programNameEdit;
    QLineEdit *argumentsEdit;
    QComboBox *charCodeCombo;
};





#endif // TERMINALWIDGET_H

/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "terminalwidget.h"

#include <QProcess>
#include <QWidget>
#include <QScrollBar>
#include <QPainter>
#include <QTextBlock>
#include <QCursor>
#include <QKeyEvent>
#include <QStringList>
#include <QRegularExpression>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QToolButton>
#include <QFileDialog>

#include "layoutparts.h"
#include "standardpixmap.h"
#include "logger.h"
#include "gnuplot.h"
#include "gnuplotcompletion.h"
#include "utility.h"





TerminalWidget::TerminalWidget(QWidget *parent)
    : QPlainTextEdit(parent)
    , _charCode(TextCodec::CharCode::Shift_JIS)
    , leftArea(new LeftArea(this))
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    QFont font = this->font();
    font.setFamily("Consolas");
    setFont(font);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, Qt::GlobalColor::black);
    palette.setColor(QPalette::Text, Qt::GlobalColor::white);
    setPalette(palette);

    setFrameShape(QFrame::Shape::NoFrame);

    updateLeftAreaWidth(0);
    connect(this, &TerminalWidget::blockCountChanged, this, &TerminalWidget::updateLeftAreaWidth);
    connect(this, &TerminalWidget::updateRequest, this, &TerminalWidget::updateLeftArea);
}

TerminalWidget::~TerminalWidget()
{

}

void TerminalWidget::setProcess(QProcess *process)
{
    if(!process)
    {
        __LOGOUT__("that process is nullptr", Logger::LogLevel::Debug);
        return;
    }

    if(_process)
        _process->disconnect(this);

    _process = process;

    connect(process, &QProcess::readyReadStandardOutput, this, &TerminalWidget::readProcessStdOut);
    connect(process, &QProcess::readyReadStandardError, this, &TerminalWidget::readProcessStdErr);

    if(_process->state() != QProcess::Running)
    {
        _process->start();

        if(_process->state() != QProcess::Running)
        {
            __LOGOUT__("failed to start the process.", Logger::LogLevel::Warn);
        }
    }

    /* エラーなどが起こったとき，閉じられることがあるため.
     * 例えば，bash.exeで"plot sin(x) with lines"と入力すれば，QIODevice::finished()となってしまい，その後出力を読めない．
     * exitを呼び出した場合，process先のアプリによって正しく動作しない．
     * close()でも再び開始される事に注意(つまりcloseできない) */
    connect(process, &QProcess::finished, [process](){ process->start(); });
}

QString TerminalWidget::programPath() const
{
    return (_process) ? _process->program() : QString();
}

QStringList TerminalWidget::startArguments() const
{
    return (_process) ? _process->arguments() : QStringList();
}

void TerminalWidget::setProgramPath(const QString &path)
{
    if(!_process) return;

    if(_process->program() != path)
    {
        _process->blockSignals(true); //QProcess::close() -> QProcess::start() を防ぐ
        _process->close();
        _process->setProgram(path);
        _process->start();
        _process->blockSignals(false);
    }
}

void TerminalWidget::setStartArguments(const QStringList& arguments)
{
    if(!_process) return;

    if(_process->arguments() != arguments)
    {
        _process->blockSignals(true); //QProcess::close() -> QProcess::start() を防ぐ
        _process->close();
        _process->setArguments(arguments);
        _process->start();
        _process->blockSignals(false);
    }
}

void TerminalWidget::setCharCode(const TextCodec::CharCode &code)
{
    _charCode = code;
}

void TerminalWidget::setPromptString(const QString &text)
{
    _promptString = text;
}

void TerminalWidget::clearTerminal()
{
    clear();
    outputLines.clear();
}

void TerminalWidget::readProcessStdOut()
{
    if(!_process) return;

    const QString stdOut = TextCodec::QStringFrom(_process->readAllStandardOutput(), _charCode);

    if(stdOut.isEmpty()) return;

    QTextCharFormat format = textCursor().charFormat();
    format.setForeground(QBrush(stdOutTextColor));

    insertOutput(stdOut, format);
}

void TerminalWidget::readProcessStdErr()
{
    if(!_process) return;
    const QString stdErr = TextCodec::QStringFrom(_process->readAllStandardError(), _charCode);
    if(stdErr.isEmpty()) return;

    QTextCharFormat format = textCursor().charFormat();
    format.setForeground(QBrush(stdErrTextColor));

    insertOutput(stdErr, format);
}

void TerminalWidget::write()
{
    _process->write(QString(document()->lastBlock().text() + "\n").toUtf8().constData());
}

void TerminalWidget::keyPressEvent(QKeyEvent *e)
{
    const bool onLastBlock = textCursor().blockNumber() == blockCount() - 1;
    bool isInvalidInput = false;

    const QTextCursor tc = textCursor();
    switch(e->key())
    {
    //Key_UpとKey_Downでコマンドの履歴を表示
    case Qt::Key_Up:
    {
        if(e->modifiers() != Qt::NoModifier || !onLastBlock) break;

        insertHistory(1);

        return;
    }
    case Qt::Key_Down:
    {
        if(e->modifiers() != Qt::NoModifier || !onLastBlock) break;

        insertHistory(-1);

        return;
    }
    //カーソル位置に限らず有効な入力
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Home:
    case Qt::Key_End:
        break;
    case Qt::Key_Backspace:
    {
        if(tc.hasSelection())
        {
            const int selectionStart = tc.selectionStart();
            const int lastBlockStart = document()->lastBlock().position();
            //複数行またいで選択されていた場合，削除は無効
            isInvalidInput = isInvalidInput || (selectionStart < lastBlockStart);
        }
        else
        {
            isInvalidInput = (tc.positionInBlock() == 0) || (tc.blockNumber() < blockCount() - 1);
        }
        break;
    }
    //コマンドの送信
    case Qt::Key_Enter:
    case Qt::Key_Return:
    {
        if(onLastBlock)
        {
            historyIndex = 0;

            if(_process)
            {
                const QString cmd = document()->lastBlock().text();
                write();
                histories.append(cmd);

                //to do cmd
                //if(cmd == "history"); //show history
                //if(cmd == "clear"); //clear terminal
            }
            else
            {
                __LOGOUT__("terminal process is nullptr.", Logger::LogLevel::Warn);
            }
        }
        break;
    }
    //アルファベットなどその他キー入力．最後のブロック以外では入力不可．
    default:
        isInvalidInput = isInvalidInput || !onLastBlock;
        break;
    }

    if(!isInvalidInput)
        QPlainTextEdit::keyPressEvent(e);
}

void TerminalWidget::insertOutput(const QString &out, const QTextCharFormat& format)
{
    if(!_process) return;

    QTextCursor tc = textCursor();

    tc.movePosition(QTextCursor::End);

    if(tc.positionInBlock() != 0)
        tc.insertBlock();

    const QTextCharFormat prevFormat = textCursor().charFormat();

    const int startLine = tc.blockNumber();

    tc.insertText(out, format);

    tc.setBlockCharFormat(prevFormat);
    tc.setCharFormat(prevFormat);

    if(tc.positionInBlock() != 0)
        tc.insertBlock();

    const int endLine = tc.blockNumber() - 1;

    for(int i = startLine; i <= endLine; ++i)
        outputLines.insert(i);

    setTextCursor(tc);
}

void TerminalWidget::insertCompletion(const QString &completion)
{
    QTextCursor tc = textCursor();

    const int extra = completion.length() - textUnderCursor().length();

    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));

    setTextCursor(tc);
}

QString TerminalWidget::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);

    return tc.selectedText();
}

void TerminalWidget::insertHistory(const int stride)
{
    historyIndex += stride;

    const int historySize = histories.size();
    const int index = historySize - historyIndex;

    static QString currentLineText = QString();

    QString insertText;

    if(index < 0)
    {
        historyIndex = historySize;
        insertText = histories.at(0);
    }
    else if(index >= historySize)
    {
        historyIndex = 0;
        insertText = currentLineText;
    }
    else
        insertText = histories.at(index);

    if(historySize == 0) return;

    QTextCursor tc = textCursor();
    tc.movePosition(QTextCursor::StartOfBlock);
    tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

    //履歴を参照し始めた時に入力していたテキストを記憶する
    if(stride > 0 && historyIndex == 1)
        currentLineText = tc.selectedText();

    tc.removeSelectedText();
    tc.insertText(insertText);
    setTextCursor(tc);
}












int TerminalWidget::leftAreaWidth()
{
    int space = 1 + fontMetrics().horizontalAdvance(QString(_promptString + " " + _promptChar));

    return space;
}

void TerminalWidget::updateLeftAreaWidth(int)
{
    setViewportMargins(leftAreaWidth(), 0, 0, 0);
}

void TerminalWidget::updateLeftArea(const QRect& rect, int dy)
{
    if(dy)
        leftArea->scroll(0, dy);
    else
        leftArea->update(0, rect.y(), leftArea->width(), rect.height());

    if(rect.contains(viewport()->rect()))
        updateLeftAreaWidth(0);
}

void TerminalWidget::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    const QRect cr = contentsRect();
    leftArea->setGeometry(QRect(cr.left(), cr.top(), leftAreaWidth(), cr.height()));
}

void TerminalWidget::leftAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(leftArea);
    painter.fillRect(event->rect(), QColor(0, 0, 0));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingRect(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while(block.isValid() && top <= event->rect().bottom())
    {
        if(block.isVisible() && top >= event->rect().top() && !outputLines.contains(blockNumber))
        {
            painter.setPen(Qt::cyan);
            painter.drawText(0, top, leftArea->width(), fontMetrics().height(), Qt::AlignLeft, _promptString + " " + _promptChar);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}











QSize LeftArea::sizeHint() const
{
    return QSize(terminal->leftAreaWidth(), 0);
}

void LeftArea::paintEvent(QPaintEvent *event)
{
    terminal->leftAreaPaintEvent(event);
}








GnuplotTermimal::GnuplotTermimal(QWidget *parent)
    : TerminalWidget(parent)
    , gnuplotcpl(new gnuplot_cpl::GnuplotCompletionModel(nullptr))
{
    gnuplotcpl->moveToThread(&completionThread);
    completionThread.start();

    connect(this, &GnuplotTermimal::completionRequested, gnuplotcpl, &gnuplot_cpl::GnuplotCompletionModel::setCompletionList);
    connect(gnuplotcpl, &gnuplot_cpl::GnuplotCompletionModel::completionListSet, this, &GnuplotTermimal::setCompletionList);
}

GnuplotTermimal::~GnuplotTermimal()
{
    completionThread.quit();
    completionThread.wait();
    gnuplotcpl->deleteLater();
    gnuplotcpl = nullptr;
}

void GnuplotTermimal::setProcess(QProcess *process)
{
    GnuplotProcess *gp = static_cast<GnuplotProcess*>(process);
    if(!gp)
    {
        __LOGOUT__("that process is not gnuplot process or nullptr.", Logger::LogLevel::Debug);
        return;
    }

    if(_process)
        _process->disconnect(this);

    _process = process;

    connect(gp, &GnuplotProcess::readyReadStdOut, this, &GnuplotTermimal::readProcessStdOut);
    connect(gp, &GnuplotProcess::readyReadStdErr, this, &GnuplotTermimal::readProcessStdErr);
}

void GnuplotTermimal::readProcessStdOut()
{
    if(GnuplotProcess *p = static_cast<GnuplotProcess*>(_process))
    {
        const QString stdOut = p->stdOut();
        if(stdOut.isEmpty()) return;

        QTextCharFormat format = textCursor().charFormat();
        format.setForeground(QBrush(stdOutTextColor));
        insertOutput(stdOut, format);
    }
}

void GnuplotTermimal::readProcessStdErr()
{
    if(GnuplotProcess *p = static_cast<GnuplotProcess*>(_process))
    {
        const QString stdErr = p->stdErr();
        if(stdErr.isEmpty()) return;

        QTextCharFormat format = textCursor().charFormat();
        format.setForeground(QBrush(stdErrTextColor));

        insertOutput(stdErr, format);
    }
}

void GnuplotTermimal::write()
{
    gnuplotExecutor->execGnuplot(static_cast<GnuplotProcess*>(_process), QList<QString>() << document()->lastBlock().text(), false);
}

void GnuplotTermimal::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Tab:
    {
        const QStringList cmdList = document()->lastBlock().text().split(' ');
        const int cmdCount = cmdList.size();
        const QString firstCmd = (cmdCount > 1) ? cmdList.at(0) : "";
        const QString prevCmd = (cmdCount > 1) ? cmdList.at(cmdCount - 2) : "";

        emit completionRequested(firstCmd, prevCmd, cmdCount - 1);
        return;
    }
    default:
        break;
    }

    TerminalWidget::keyPressEvent(event);
}

void GnuplotTermimal::setCompletionList(const QStringList &list)
{
    //if(list.count() > maxCompletionList)
    //{
    //    __LOGOUT__("too many completion list.", Logger::LogLevel::Info);
    //
    //    return;
    //}

    const QStringList completionList = list.filter(QRegularExpression(QString("^" + textUnderCursor())));

    if(completionList.count() < 1)
        return;
    else if(completionList.count() > 1)
    {
        const QString currentLineText = textCursor().block().text();
        QTextCharFormat format = textCursor().charFormat();
        format.setForeground(QBrush(stdOutTextColor));
        insertOutput(completionList.join(" "), format);
        textCursor().insertText(currentLineText);
    }
    else
        insertCompletion(completionList.at(0));    //complete the command if list has only one completion.
}













TerminalTabWidget::TerminalTabWidget(QWidget *parent)
    : QTabWidget(parent)
    , _gnuplotTerminal(new GnuplotTermimal(this))
    , _logBrowser(new LogBrowserWidget(this))
    , _cornerWidget(new CornerWidget(this))
    , _terminalSettingDialog(new TerminalSettingDialog(this))
{
    setTabPosition(QTabWidget::TabPosition::South);
    setCornerWidget(_cornerWidget, Qt::BottomRightCorner);

    addTab(_gnuplotTerminal, "Gnuplot");
    addTab(_logBrowser, "&Output");

    _gnuplotTerminal->setProcess(gnuplotExecutor->defaultProcess());

    _logBrowser->addFilter(Logger::LogLevel::GnuplotStdOut);
    _logBrowser->addFilter(Logger::LogLevel::GnuplotStdErr);
    _logBrowser->setAutoScroll(true);

    connect(this, &TerminalTabWidget::currentChanged, this, &TerminalTabWidget::setCornerWidgetOption);
    connect(logger, &Logger::logPushed, _logBrowser, &LogBrowserWidget::appendLog);
    connect(_logBrowser, &LogBrowserWidget::textChanged, this, &TerminalTabWidget::setPageToLogBrowser);
    connect(_cornerWidget, &CornerWidget::addTerminalRequested, this, &TerminalTabWidget::addTerminal);
    connect(_cornerWidget, &CornerWidget::setTerminalRequested, this, &TerminalTabWidget::settingTerminal);
    connect(_cornerWidget, &CornerWidget::clearDisplayRequested, this, &TerminalTabWidget::clearDisplay);
    connect(_cornerWidget, &CornerWidget::killTerminalRequested, this, &TerminalTabWidget::killTerminal);

    setCornerWidgetOption(currentIndex());
}

QSize TerminalTabWidget::sizeHint() const
{
    return QSize(0, 0);
}

void TerminalTabWidget::setPageToLogBrowser()
{
    setCurrentWidget(_logBrowser);
}

void TerminalTabWidget::setCornerWidgetOption(const int index)
{
    QWidget *page = widget(index);

    if(page == _gnuplotTerminal || page == _logBrowser)
    {
        _cornerWidget->enableAddTerminal(true);
        _cornerWidget->enableSetTerminal(false);
        _cornerWidget->enableClearDisplay(true);
        _cornerWidget->enableKillTerminal(false);
    }
    else
    {
        _cornerWidget->enableAddTerminal(true);
        _cornerWidget->enableSetTerminal(true);
        _cornerWidget->enableClearDisplay(true);
        _cornerWidget->enableKillTerminal(true);
    }
}

void TerminalTabWidget::addTerminal()
{
    _terminalSettingDialog->setProgramPath("");
    _terminalSettingDialog->setProgramName("");
    _terminalSettingDialog->setArguments("");

    const int exitCode = _terminalSettingDialog->exec();

    switch(QDialog::DialogCode(exitCode))
    {
    case QDialog::Accepted: break;
    case QDialog::Rejected: return;
    default:
         __LOGOUT__("invalid DialogCode enum index.", Logger::LogLevel::Debug);
         return;
    }

    QProcess *process = new QProcess(nullptr);
    process->setProgram(_terminalSettingDialog->programPath());
    if(!_terminalSettingDialog->arguments().isEmpty())
        process->setArguments(_terminalSettingDialog->arguments().split(';'));

    TerminalWidget *terminal = new TerminalWidget(this);

    terminal->setProcess(process);
    terminal->setCharCode(_terminalSettingDialog->charCode());
    insertTab(count(), terminal, _terminalSettingDialog->programName());
    setCurrentWidget(terminal);

    connect(this, &TerminalTabWidget::destroyed, process, &QProcess::close);
    connect(this, &TerminalTabWidget::destroyed, process, &QProcess::deleteLater);
}

void TerminalTabWidget::settingTerminal()
{
    TerminalWidget *terminal = static_cast<TerminalWidget*>(currentWidget());
    if(!terminal) return;

    _terminalSettingDialog->setProgramPath(terminal->programPath());
    _terminalSettingDialog->setArguments(terminal->startArguments().join(';'));
    _terminalSettingDialog->setProgramName(tabText(currentIndex()));
    _terminalSettingDialog->setCharCode(terminal->charCode());

    const int exitCode = _terminalSettingDialog->exec();

    switch(QDialog::DialogCode(exitCode))
    {
    case QDialog::Accepted: break;
    case QDialog::Rejected: return;
    default:
        __LOGOUT__("invalid Dialog Code enum index.", Logger::LogLevel::Debug);
        return;
    }

    terminal->setProgramPath(_terminalSettingDialog->programPath());
    if(_terminalSettingDialog->arguments().isEmpty())
        terminal->setStartArguments(QStringList());
    else
        terminal->setStartArguments(_terminalSettingDialog->arguments().split(';'));
    terminal->setStartArguments(QStringList());
    setTabText(currentIndex(), _terminalSettingDialog->programName());
    terminal->setCharCode(_terminalSettingDialog->charCode());
}

void TerminalTabWidget::clearDisplay()
{
    QWidget *w = currentWidget();

    if(w == _logBrowser)
    {
        _logBrowser->clear();
    }
    else
    {
        TerminalWidget *terminal = static_cast<TerminalWidget*>(w);
        if(!terminal)
        {
            __LOGOUT__("could not cast.", Logger::LogLevel::Debug);
            return;
        }

        terminal->clearTerminal();
    }
}

void TerminalTabWidget::killTerminal()
{
    TerminalWidget *terminal = static_cast<TerminalWidget*>(currentWidget());
    if(!terminal)
    {
        __LOGOUT__("could not cast.", Logger::LogLevel::Debug);
        return;
    }

    QProcess *process = terminal->process();
    if(process)
    {
        process->close();
        process->deleteLater();
    }

    terminal->deleteLater();
}














TerminalTabWidget::CornerWidget::CornerWidget(QWidget *parent)
    : QWidget(parent)
    , plusLabel(new mlayout::IconLabel(this))
    , settingLabel(new mlayout::IconLabel(this))
    , clearLabel(new mlayout::IconLabel(this))
    , trashCanLabel(new mlayout::IconLabel(this))
{
    QHBoxLayout *hLayout = new QHBoxLayout(this);

    setLayout(hLayout);
    hLayout->addWidget(plusLabel);
    hLayout->addWidget(settingLabel);
    hLayout->addWidget(clearLabel);
    hLayout->addWidget(trashCanLabel);

    static constexpr int iconSize = 20;
    static const QPalette hoverPalette = QPalette(plusLabel->backgroundRole(), Qt::lightGray);
    plusLabel->setAutoFillBackground(true);
    settingLabel->setAutoFillBackground(true);
    clearLabel->setAutoFillBackground(true);
    trashCanLabel->setAutoFillBackground(true);
    plusLabel->setHoveredPalette(hoverPalette);
    settingLabel->setHoveredPalette(hoverPalette);
    clearLabel->setHoveredPalette(hoverPalette);
    trashCanLabel->setHoveredPalette(hoverPalette);
    plusLabel->setPixmap(StandardPixmap::Icon::plus().scaled(iconSize, iconSize));
    settingLabel->setPixmap(StandardPixmap::Icon::gear().scaled(iconSize, iconSize));
    clearLabel->setPixmap(StandardPixmap::Icon::broom().scaled(iconSize, iconSize));
    trashCanLabel->setPixmap(StandardPixmap::Icon::trashCan().scaled(iconSize, iconSize));

    plusLabel->setToolTip("Add New Terminal");
    settingLabel->setToolTip("Setting Terminal");
    clearLabel->setToolTip("Clear Display");
    trashCanLabel->setToolTip("Kill Terminal");

    setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(5);
    hLayout->setContentsMargins(0, 0, 0, 0);

    connect(plusLabel, &mlayout::IconLabel::released, this, &CornerWidget::addTerminalRequested);
    connect(settingLabel, &mlayout::IconLabel::released, this, &CornerWidget::setTerminalRequested);
    connect(clearLabel, &mlayout::IconLabel::released, this, &CornerWidget::clearDisplayRequested);
    connect(trashCanLabel, &mlayout::IconLabel::released, this, &CornerWidget::killTerminalRequested);
}

void TerminalTabWidget::CornerWidget::enableAddTerminal(const bool enable)
{
    plusLabel->setVisible(enable);
}

void TerminalTabWidget::CornerWidget::enableSetTerminal(const bool enable)
{
    settingLabel->setVisible(enable);
}

void TerminalTabWidget::CornerWidget::enableClearDisplay(const bool enable)
{
    clearLabel->setVisible(enable);
}

void TerminalTabWidget::CornerWidget::enableKillTerminal(const bool enable)
{
    trashCanLabel->setVisible(enable);
}










TerminalTabWidget::TerminalSettingDialog::TerminalSettingDialog(QWidget *parent)
    : QDialog(parent)
    , pathEdit(new QLineEdit(this))
    , programNameEdit(new QLineEdit(this))
    , argumentsEdit(new QLineEdit(this))
    , charCodeCombo(new QComboBox(this))
{
    QFormLayout *fLayout = new QFormLayout(this);
    QHBoxLayout *pathLayout = new QHBoxLayout;
    QToolButton *dialogButton = new QToolButton(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);

    setLayout(fLayout);
    fLayout->addRow("Program Path", pathLayout);
    pathLayout->addWidget(pathEdit);
    pathLayout->addWidget(dialogButton);
    fLayout->addRow("Program Name", programNameEdit);
    fLayout->addRow("Program Arguments", argumentsEdit);
    fLayout->addRow("Char Code", charCodeCombo);
    fLayout->addRow("", buttonLayout);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    pathEdit->setToolTip("Program path.");
    dialogButton->setText("...");
    dialogButton->setToolTip("Open file dialog.");
    programNameEdit->setToolTip("Program name. This name is used in tab title.");
    argumentsEdit->setToolTip("Program arguments when the process starts. It is separated by ';'");
    charCodeCombo->setToolTip("Specifying the character code on the program side.");
    charCodeCombo->addItems(mutility::enumToStrings(TextCodec::CharCode(0)));

    connect(pathEdit, &QLineEdit::textChanged, this, &TerminalSettingDialog::setProgramNameFromPath);
    connect(dialogButton, &QToolButton::released, this, &TerminalSettingDialog::setPathFromDialog);
    connect(okButton, &QPushButton::released, [this](){ done((int)QDialog::DialogCode::Accepted); });
    connect(cancelButton, &QPushButton::released, [this](){ done((int)QDialog::DialogCode::Rejected); });
}

void TerminalTabWidget::TerminalSettingDialog::setProgramPath(const QString &path)
{
    pathEdit->blockSignals(true);
    pathEdit->setText(path);
    pathEdit->blockSignals(false);
}

void TerminalTabWidget::TerminalSettingDialog::setProgramName(const QString &name)
{
    programNameEdit->setText(name);
}

void TerminalTabWidget::TerminalSettingDialog::setArguments(const QString &arguments)
{
    argumentsEdit->setText(arguments);
}

void TerminalTabWidget::TerminalSettingDialog::setCharCode(const TextCodec::CharCode &code)
{
    charCodeCombo->setCurrentIndex((int)code);
}

QString TerminalTabWidget::TerminalSettingDialog::programPath() const
{
    return pathEdit->text();
}

QString TerminalTabWidget::TerminalSettingDialog::programName() const
{
    return programNameEdit->text();
}

QString TerminalTabWidget::TerminalSettingDialog::arguments() const
{
    return argumentsEdit->text();
}

TextCodec::CharCode TerminalTabWidget::TerminalSettingDialog::charCode() const
{
    return TextCodec::CharCode(charCodeCombo->currentIndex());
}

void TerminalTabWidget::TerminalSettingDialog::setProgramNameFromPath(const QString &path)
{
    QFileInfo info(path);
    programNameEdit->setText(info.baseName());
}

void TerminalTabWidget::TerminalSettingDialog::setPathFromDialog()
{
    const QString path = QFileDialog::getOpenFileName(nullptr, "Program File", QString(), "*exe");

    if(path.isEmpty()) return;

    pathEdit->setText(path);
}







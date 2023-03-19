#include "gnuplottexteditor.h"


#include <QTextBlock>
#include <QToolTip>
#include "standardpixmap.h"



GnuplotTextEditor::GnuplotTextEditor(QWidget *parent)
    : TextEditor(parent)
    , errorLineNumber(-1)
    , errorLabel(new GnuplotTextEditor::ErrorLabel(this))
{
    QToolTip::setFont(QFont());
    connect(errorLabel, &ErrorLabel::cursorEntered, this, &GnuplotTextEditor::showErrorToolTip);
}

void GnuplotTextEditor::setErrorLineNumber(const int number)
{
    errorLineNumber = number;
    highlight();
}

void GnuplotTextEditor::setErrorMessage(const QString &message)
{
    errorMessage = message;
}

void GnuplotTextEditor::highlightExtraSelections(QList<QTextEdit::ExtraSelection> &selections)
{
    TextEditor::highlightExtraSelections(selections);

    if(errorLineNumber >= 0)
    {
        QTextEdit::ExtraSelection errorBlockSelection;

        const QTextBlock errorBlock = document()->findBlockByLineNumber(errorLineNumber);
        errorBlockSelection.format.setUnderlineColor(Qt::red);
        errorBlockSelection.format.setFontUnderline(true);
        errorBlockSelection.format.setProperty(QTextFormat::FullWidthSelection, true);
        errorBlockSelection.cursor = QTextCursor(errorBlock);
        errorBlockSelection.cursor.clearSelection();

        selections.append(errorBlockSelection);
    }
}

void GnuplotTextEditor::paintLineNumberArea(QPainter &painter, const QRect &rect, const int number)
{
    const int iconSize = rect.height();
    const int firstVisibleBlockNum = firstVisibleBlock().blockNumber();
    const int visibleBlockCount = lineNumberArea()->height() / rect.height();

    lineNumberArea()->setLeftMaring(iconSize);

    if(errorLabel->isVisible() &&
       (firstVisibleBlockNum > errorLineNumber ||
        errorLineNumber >= firstVisibleBlockNum + visibleBlockCount))
    {   //errorLineNumberが表示されている行に含まれない
        errorLabel->hide();
    }
    else if(number == errorLineNumber)
    {
        if(errorLabel->pixmap().size().width() != iconSize)
        {
            errorLabel->setPixmap(StandardPixmap::Icon::caution().scaled(iconSize, iconSize));
        }

        errorLabel->setGeometry(QRect(rect.left(), rect.top(), iconSize, rect.height()));

        errorLabel->show();
    }

    TextEditor::paintLineNumberArea(painter, rect, number);
}

void GnuplotTextEditor::showErrorToolTip()
{
    QToolTip::setFont(font());
    QToolTip::showText(cursor().pos(), errorMessage);
}









GnuplotTextEditor::ErrorLabel::ErrorLabel(QWidget *parent)
    : QLabel(parent)
{

}

void GnuplotTextEditor::ErrorLabel::enterEvent(QEnterEvent *)
{
    emit cursorEntered();
}












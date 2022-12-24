#ifndef GNUPLOTTEXTEDITOR_H
#define GNUPLOTTEXTEDITOR_H

#include "texteditor.h"
#include <QLabel>


class GnuplotTextEditor : public TextEditor
{
    Q_OBJECT
public:
    explicit GnuplotTextEditor(QWidget *parent);

public slots:
    void setErrorLineNumber(const int number);
    void setErrorMessage(const QString& message);

protected:
    void highlightExtraSelections(QList<QTextEdit::ExtraSelection>& selections) override;
    void paintLineNumberArea(QPainter& painter, const QRect& rect, const int number) override;

private slots:
    void showErrorToolTip();

private:
    int errorLineNumber;
    QString errorMessage;
    class ErrorLabel;
    ErrorLabel *errorLabel;
};






class GnuplotTextEditor::ErrorLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ErrorLabel(QWidget *parent);

protected:
    void enterEvent(QEnterEvent *event) override;

signals:
    void cursorEntered();
};






#endif // GNUPLOTTEXTEDITOR_H

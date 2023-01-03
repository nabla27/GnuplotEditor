#ifndef LAYOUTPARTS_H
#define LAYOUTPARTS_H

#include <QLabel>
#include <QPen>
#include <QDialog>
#include <QProgressBar>
#include <QPushButton>

class QFontDialog;
class QColorDialog;
class QListWidget;
class QSpinBox;
class QDoubleSpinBox;



namespace mlayout
{




class IconLabel : public QLabel
{
    Q_OBJECT
public:
    explicit IconLabel(const QString& text, QWidget *parent);
    explicit IconLabel(QWidget *parent);

    void setPaintFrameColor(const QColor& color) { paintFrameColor = color; update(); }
    void setPaintFrameWidth(const int width) { paintFrameWidth = width; update(); }

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *paintEvent) override;

public:
    void setHoveredPalette(const QPalette& p) { hoveredPalette = p; }
    void setHoveredFrameShape(const QFrame::Shape shape) { hoveredFrameShape = shape; }
    void setMenu(QMenu *menu) { this->menu = menu; }

private:
    QColor paintFrameColor;
    int paintFrameWidth = 0;

    QPalette normalPalette;
    QPalette hoveredPalette;
    QFrame::Shape normalFrameShape;
    QFrame::Shape hoveredFrameShape;

    QMenu *menu;

signals:
    void released();
};






class SeparatorLineWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SeparatorLineWidget(QWidget *parent,
                                 Qt::Orientation orient = Qt::Orientation::Horizontal,
                                 const int lineWidth = 1,
                                 const QColor& color = Qt::lightGray,
                                 const int lineSpace = 0);
    explicit SeparatorLineWidget(QWidget *parent,
                                 const QPen& pen,
                                 const int lineSpace = 0);

public:
    QPen linePen() const { return _linePen; }

public slots:
    void setLineOrientation(Qt::Orientation orient);
    void setLinePen(const QPen& pen);
    void setLineWidth(const int width);
    void setLineColor(const QColor& color);
    void setSpace(const int space);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Qt::Orientation orientation = Qt::Orientation::Horizontal;
    QPen _linePen;
    int lineSpace = 0;
};






class LoopProgressDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoopProgressDialog(QWidget *parent, Qt::WindowFlags f = Qt::WindowFlags());

public:
    bool isProgress() const;

public slots:
    void start();
    void stop();

    void setTitle(const QString& title);
    void setMessage(const QString& message);

    void setAutoDisplay(bool enable) { _autoDisplayFlag = enable; }

    void setLinearStartColor(const QColor& color);
    void setLinearEndColor(const QColor& color);
    void setBandHalfWidth(const float& halfWidth);

private:
    void updateStyleSheet();
    void adjustSize();

private:
    QLabel *titleLabel;
    QLabel *messageLabel;
    QProgressBar *progressBar;
    QTimer *timer;
    bool _autoDisplayFlag = false;

    QColor startColor;
    QColor endColor;
    float bandHalfWidth;

    inline static int defaultWidth = 400;
    inline static int d_count = 100;
};








class FontButton : public QPushButton
{
    Q_OBJECT
public:
    explicit FontButton(const QString& text, QWidget *parent);
    explicit FontButton(const QString& text, const QFont& font, QWidget *parent);

public:
    void setDialogFont(const QFont& font);

private:
    void setup();

private:
    QFontDialog *const fDialog;

signals:
    void fontChanged(const QFont& font);
};







class ColorButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ColorButton(const QString& text, const QPalette::ColorRole& role, QWidget *parent);
    explicit ColorButton(const QString& text, const QPalette::ColorRole& role, const QColor& color, QWidget *parent);

public:
    void setDialogColor(const QColor& color);
    void setColorRole(const QPalette::ColorRole& role);

private slots:
    void setButtonColor(const QColor& color);

private:
    void setup();

private:
    QColorDialog *cDialog;
    QPalette::ColorRole role;

signals:
    void colorChanged(const QColor& color);
};







class PenSampleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PenSampleWidget(const QPen& pen, QWidget *parent);

    enum class SampleShape { Jagged, Rect, Ellipse };
    Q_ENUM(SampleShape)

public:
    SampleShape sampleShape() const { return _sampleShape; }

public slots:
    void setSamplePen(const QPen& pen);
    void setSampleShape(const int shape);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPen pen;
    SampleShape _sampleShape = SampleShape::Jagged;
};










class PenDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PenDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    explicit PenDialog(const QPen& initPen, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    QSize sizeHint() const override { return QSize(QDialog::sizeHint().width(), QDialog::sizeHint().height() / 2); }

    void setPen(const QPen& pen);

private:
    void setup();

private:
    QPen pen;

    QListWidget *styleList;
    QListWidget *capStyleList;
    QListWidget *jointList;
    QSpinBox *widthSpinBox;
    QDoubleSpinBox *miterLimitSpinBox;
    QDoubleSpinBox *offsetSpinBox;
    ColorButton *colorButton;
    QListWidget *patternList;

signals:
    void penChanged(const QPen& pen);
};









class PenButton : public QPushButton
{
    Q_OBJECT
public:
    explicit PenButton(const QString& text, const QPen& pen, QWidget *parent);

public:
    void setDialogPen(const QPen& pen);

private:
    void setup();

private:
    PenDialog *pDialog;

signals:
    void penChanged(const QPen& pen);
};










}//namespace mlayout


#endif // LAYOUTPARTS_H

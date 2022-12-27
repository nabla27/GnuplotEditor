#ifndef LAYOUTPARTS_H
#define LAYOUTPARTS_H

#include <QLabel>
#include <QPen>
#include <QDialog>
#include <QProgressBar>




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








}//namespace mlayout


#endif // LAYOUTPARTS_H

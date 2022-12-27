#include "layoutparts.h"

#include <QPainter>
#include <QHBoxLayout>
#include <QMenu>
#include <QProgressBar>
#include <QTimer>
#include <QPushButton> //DEBUG"
#include "logger.h"


namespace mlayout
{



IconLabel::IconLabel(const QString& text, QWidget *parent)
    : QLabel(text, parent)
    , menu(nullptr)
{
    normalPalette = this->palette();
    normalFrameShape = this->frameShape();
    hoveredPalette = normalPalette;
    hoveredFrameShape = normalFrameShape;
}
IconLabel::IconLabel(QWidget *parent)
    : QLabel(parent)
    , menu(nullptr)
{
    normalPalette = this->palette();
    normalFrameShape = this->frameShape();
    hoveredPalette = normalPalette;
    hoveredFrameShape = normalFrameShape;
}

void IconLabel::enterEvent(QEnterEvent *event)
{
    normalPalette = palette();
    normalFrameShape = frameShape();

    setPalette(hoveredPalette);
    setFrameShape(hoveredFrameShape);

    QLabel::enterEvent(event);
}

void IconLabel::leaveEvent(QEvent *event)
{
    setPalette(normalPalette);
    setFrameShape(normalFrameShape);

    QLabel::leaveEvent(event);
}

void IconLabel::mousePressEvent(QMouseEvent*)
{
    setPalette(normalPalette);
    setFrameShape(normalFrameShape);
}

void IconLabel::mouseReleaseEvent(QMouseEvent *event)
{
    setPalette(hoveredPalette);
    setFrameShape(hoveredFrameShape);

    emit released();

    if(menu)
    {
        menu->exec(this->cursor().pos());
    }

    QLabel::mouseReleaseEvent(event);
}

void IconLabel::paintEvent(QPaintEvent *paintEvent)
{
    if(paintFrameWidth > 0)
    {
        QPainter painter(this);
        QPen pen;
        pen.setWidth(paintFrameWidth);
        pen.setColor(paintFrameColor);
        painter.setPen(pen);
        painter.drawRect(QRect(paintFrameWidth / 2 + frameWidth(),
                               paintFrameWidth / 2 + frameWidth(),
                               rect().width() - paintFrameWidth - frameWidth() * 2,
                               rect().height() - paintFrameWidth - frameWidth() * 2));
    }
    QLabel::paintEvent(paintEvent);
}









SeparatorLineWidget::SeparatorLineWidget(QWidget *parent, Qt::Orientation orient,
                                         const int lineWidth, const QColor& color,
                                         const int lineSpace)
    : QWidget(parent)
    , orientation(orient)
    , lineSpace(lineSpace)
{
    QPen pen;
    pen.setWidth(lineWidth);
    pen.setColor(color);
    this->_linePen = pen;

    setLayout(new QHBoxLayout(this)); //layoutを設定しないとpaintEvent()が呼び出されない
}

SeparatorLineWidget::SeparatorLineWidget(QWidget *parent, const QPen &pen,
                                         const int lineSpace)
    : QWidget(parent)
    , _linePen(pen)
    , lineSpace(lineSpace)
{
    setLayout(new QHBoxLayout(this));
}

void SeparatorLineWidget::setLinePen(const QPen &pen)
{
    this->linePen() = pen;
    update();
}

void SeparatorLineWidget::setLineWidth(const int width)
{
    _linePen.setWidth(width);
    update();
}

void SeparatorLineWidget::setSpace(const int space)
{
    this->lineSpace = space;
    update();
}

void SeparatorLineWidget::setLineColor(const QColor &color)
{
    _linePen.setColor(color);
    update();
}

void SeparatorLineWidget::setLineOrientation(Qt::Orientation orient)
{
    this->orientation = orient;
    update();
}

void SeparatorLineWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(_linePen);

    switch(orientation)
    {
    case Qt::Orientation::Horizontal:
    {
        painter.drawLine(lineSpace,
                         rect().height() / 2,
                         rect().width() - lineSpace,
                         rect().height() / 2);
    }
    case Qt::Orientation::Vertical:
    {
        painter.drawLine(rect().width() / 2,
                         lineSpace,
                         rect().width() / 2,
                         rect().height() - lineSpace);
    }
    default:
        break;
    }

    QWidget::paintEvent(event);
}







LoopProgressDialog::LoopProgressDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , titleLabel(new QLabel(this))
    , messageLabel(new QLabel(this))
    , progressBar(new QProgressBar(this))
    , timer(new QTimer(this))

    , startColor(Qt::green)
    , endColor(Qt::white)
    , bandWidth(0.4f)
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    setLayout(vLayout);

    vLayout->addWidget(titleLabel);
    vLayout->addWidget(messageLabel);
    vLayout->addWidget(progressBar);
    vLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));

    QFont font(titleLabel->font());
    font.setPointSize(font.pointSize() + 3);
    font.setBold(true);
    titleLabel->setFont(font);

    titleLabel->hide();
    messageLabel->hide();

    progressBar->setTextVisible(false);

    progressBar->setMaximum(100);
    progressBar->setValue(progressBar->maximum());
    updateStyleSheet();
    timer->setInterval(10);

    adjustSize();

    connect(timer, &QTimer::timeout, this, &LoopProgressDialog::updateStyleSheet);
}

void LoopProgressDialog::start()
{
    if(_autoDisplayFlag) show();
    timer->start();
}

void LoopProgressDialog::stop()
{
    if(_autoDisplayFlag) hide();
    timer->stop();
}

void LoopProgressDialog::setTitle(const QString &title)
{
    if(title.isEmpty())
        titleLabel->hide();
    else
        titleLabel->show();

    titleLabel->setText(title);
    adjustSize();
}

void LoopProgressDialog::setMessage(const QString &message)
{
    if(message.isEmpty())
        messageLabel->hide();
    else
        messageLabel->show();

    messageLabel->setText(message);
    adjustSize();
}

void LoopProgressDialog::setLinearStartColor(const QColor &color)
{
    startColor = color;
}

void LoopProgressDialog::setLinearEndColor(const QColor &color)
{
    endColor = color;
}

void LoopProgressDialog::setBandWidth(const float &band)
{
    bandWidth = band;
}

void LoopProgressDialog::updateStyleSheet()
{
    static qsizetype i = 0;

    /*
     *
     *              0                                                                       1
     *              |-----------------------------------------------------------------------|               :porgressBar
     *
     *      |                    |                    |                                                     :case l_pos < 0
     *    l_pos                m_pos                r_pos
     *          side_color   startColor            endColor              end_color       sideColor
     *
     *
     *
     *
     *                      |                    |                    |                                     :case r_pos > 1
     *                    l_pos                m_pos                r_pos
     *                   endColor            startColor            endColor
     *
     *
     *
     *
     *                                                      |                    |                    |     :case other
     *                                                    l_pos                m_pos                r_pos
     *          sideColor              endColor          endColor           startColor   sideColor
     *
     *
     */

    const float m_pos = float(i++ % (d_count + 1)) / d_count;
    const float l_pos = m_pos - bandWidth;
    const float r_pos = m_pos + bandWidth;

    QString style = "QProgressBar::chunk {"
                    "background-color: qlineargradient("
                    "x1:0, y1:0.5, x2:1, y2:0.5,";

    if(l_pos < 0)
    {
        const float ratio = - l_pos / (m_pos - l_pos);

        static QColor side_color;
        side_color.setRed(endColor.red() + ratio * (startColor.red() - endColor.red()));
        side_color.setGreen(endColor.green() + ratio * (startColor.green() - endColor.green()));
        side_color.setBlue(endColor.blue() + ratio * (startColor.blue() - endColor.blue()));

        style += "stop:0.0 " + side_color.name() + ",";
        style += "stop:" + QString::number(m_pos) + " " + startColor.name() + ",";
        style += "stop:" + QString::number(r_pos) + " " + endColor.name() + ",";
        style += "stop:" + QString::number(1.0f + l_pos) + " " + endColor.name() + ",";
        style += "stop:1.0 " + side_color.name() + ",";
    }
    else if(r_pos > 1)
    {
        const float ratio = (1.0f - m_pos) / (m_pos - l_pos);

        static QColor side_color;
        side_color.setRed(startColor.red() + ratio * (endColor.red() - startColor.red()));
        side_color.setGreen(startColor.green() + ratio * (endColor.green() - startColor.green()));
        side_color.setBlue(startColor.blue() + ratio * (endColor.blue() - startColor.blue()));

        style += "stop:0.0 " + side_color.name() + ",";
        style += "stop:" + QString::number(r_pos - 1.0f) + " " + endColor.name() + ",";
        style += "stop:" + QString::number(l_pos) + " " + endColor.name() + ",";
        style += "stop:" + QString::number(m_pos) + " " + startColor.name() + ",";
        style += "stop:1.0 " + side_color.name() + ",";
    }
    else
    {
        style += "stop:" + QString::number(l_pos) + " " + endColor.name() + ",";
        style += "stop:" + QString::number(m_pos) + " " + startColor.name() + ",";
        style += "stop:" + QString::number(r_pos) + " " + endColor.name() + ",";
    }

    style += ");"
             "}";

    progressBar->setStyleSheet(style);
}

void LoopProgressDialog::adjustSize()
{
    setFixedSize(qMax(defaultWidth, sizeHint().width()), sizeHint().height());
}



























}//namespace mlayout

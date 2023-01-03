#include "layoutparts.h"

#include <QPainter>
#include <QHBoxLayout>
#include <QMenu>
#include <QProgressBar>
#include <QTimer>
#include <QPushButton>
#include <QFontDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QSpinBox>
#include <QColorDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include "utility.h"
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
    , bandHalfWidth(0.4f)
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

    progressBar->setMaximum(d_count);
    progressBar->setValue(progressBar->maximum());
    updateStyleSheet();
    timer->setInterval(1000 / d_count); // period[msec] / dialogMaximum[int]

    adjustSize();

    connect(timer, &QTimer::timeout, this, &LoopProgressDialog::updateStyleSheet);
}

bool LoopProgressDialog::isProgress() const
{
    return timer->isActive();
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

void LoopProgressDialog::setBandHalfWidth(const float &halfWidth)
{
    if(halfWidth > 0.5f)
    {
        __LOGOUT__("invalid band width. it must be less than 0.5.", Logger::LogLevel::Debug);
        return;
    }

    bandHalfWidth = halfWidth;
}

void LoopProgressDialog::updateStyleSheet()
{
    static qsizetype i = 0;

    /*
     *              0                                                                    d_count
     *              0                                                                       1
     *              |-----------------------------------------------------------------------|               -> porgressBar
     *
     *      |                    |                    |                                                     -> case: l_pos < 0
     *    l_pos                m_pos                r_pos
     *          side_color   startColor            endColor              end_color       sideColor
     *
     *
     *
     *
     *                      |                    |                    |                                     -> case: r_pos > 1
     *                    l_pos                m_pos                r_pos
     *                   endColor            startColor            endColor
     *
     *
     *
     *
     *                                                      |                    |                    |     -> case: other
     *                                                    l_pos                m_pos                r_pos
     *          sideColor              endColor          endColor           startColor   sideColor
     *
     *
     * --- Linear Gradient Color Point [0 <= real <= 1] ---
     * m_pos, l_pos, r_pos
     *
     * --- Linear Gradient Color [QColor] ---
     * startColor, endColor
     *
     * --- Linear Gradient Band Width [0 <= real <= 1] ---
     * bandHalfWidth
     *
     * --- Both Ends Color Of The Progress Bar [QColor] ---
     * side_color (always left_side_color == right_side_color)
     *
     * --- Position Ratio of the Both Ends [0 <= real <= 1] ---
     * ratio (zero = l_pos, one = r_pos, when l_pos < 0: ratio of the progress pos = 0
     *                                   when r_pos > 1: ratio of the progress pos = 1)
     */

    const float m_pos = float(i++ % (d_count + 1)) / d_count;
    const float l_pos = m_pos - bandHalfWidth;
    const float r_pos = m_pos + bandHalfWidth;

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












FontButton::FontButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
    , fDialog(new QFontDialog(this))
{
    setup();
}

FontButton::FontButton(const QString &text, const QFont &font, QWidget *parent)
    : QPushButton(text, parent)
    , fDialog(new QFontDialog(font, this))
{
    setup();
}

void FontButton::setDialogFont(const QFont &font)
{
    fDialog->setCurrentFont(font);
}

void FontButton::setup()
{
    connect(this, &FontButton::released, fDialog, &QFontDialog::exec);
    connect(fDialog, &QFontDialog::currentFontChanged, this, &FontButton::fontChanged);
    connect(fDialog, &QFontDialog::currentFontChanged, this, &FontButton::setFont);
}










ColorButton::ColorButton(const QString &text, const QPalette::ColorRole &role, QWidget *parent)
    : QPushButton(text, parent)
    , cDialog(new QColorDialog(this))
    , role(role)
{
    setup();
}

ColorButton::ColorButton(const QString &text, const QPalette::ColorRole &role, const QColor &color, QWidget *parent)
    : QPushButton(text, parent)
    , cDialog(new QColorDialog(color, this))
    , role(role)
{
    setup();
    setButtonColor(color);
}

void ColorButton::setDialogColor(const QColor &color)
{
    cDialog->setCurrentColor(color);
}

void ColorButton::setButtonColor(const QColor &color)
{
    QPalette p(this->palette());
    p.setColor(role, color);
    setPalette(p);
}

void ColorButton::setColorRole(const QPalette::ColorRole &role)
{
    this->role = role;
}

void ColorButton::setup()
{
    setAutoFillBackground(true);

    connect(this, &ColorButton::released, cDialog, &QColorDialog::exec);
    connect(cDialog, &QColorDialog::currentColorChanged, this, &ColorButton::colorChanged);
    connect(cDialog, &QColorDialog::currentColorChanged, this, &ColorButton::setButtonColor);
}











PenSampleWidget::PenSampleWidget(const QPen &pen, QWidget *parent)
    : QWidget(parent)
    , pen(pen)
{

}

void PenSampleWidget::setSamplePen(const QPen &pen)
{
    this->pen = pen;
    repaint();
}

void PenSampleWidget::setSampleShape(const int shape)
{
    _sampleShape = SampleShape(shape);
    repaint();
}

void PenSampleWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(pen);

    static constexpr float mergin = 0.15f;

    switch(_sampleShape)
    {
    case SampleShape::Jagged:
    {
        static constexpr int lineCount = 6;

        int x = width() * mergin;
        const int y_u = height() * mergin;
        const int y_d = height() * (1.0f - mergin);
        for(int i = 0; i < lineCount; ++i)
        {
            const int next_x = x + width() * (1.0f - 2.0f * mergin) / lineCount;

            if(i % 2 == 0)
                painter.drawLine(x, y_u, next_x, y_d);
            else
                painter.drawLine(x, y_d, next_x, y_u);

            x = next_x;
        }
        break;
    }
    case SampleShape::Rect:
        painter.drawRect(QRect(width() * mergin, height() * mergin, width() * (1.0f - mergin * 2.0f), height() * (1.0f - mergin * 2.0f)));
        break;
    case SampleShape::Ellipse:
        painter.drawEllipse(QRect(width() * mergin, height() * mergin, width() * (1.0f - mergin * 2.0f), height() * (1.0f - mergin * 2.0f)));
        break;
    default:
        __LOGOUT__("invalid SampleShape Index.", Logger::LogLevel::Debug);
        break;
    }

    QWidget::paintEvent(event);
}
















PenDialog::PenDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , pen(QPen())

    , styleList(new QListWidget(this))
    , capStyleList(new QListWidget(this))
    , jointList(new QListWidget(this))
    , widthSpinBox(new QSpinBox(this))
    , miterLimitSpinBox(new QDoubleSpinBox(this))
    , offsetSpinBox(new QDoubleSpinBox(this))
    , colorButton(new mlayout::ColorButton("Sample", QPalette::ButtonText, pen.brush().color(), this))
    , patternList(new QListWidget(this))
{
    setup();
    setPen(pen);
}

PenDialog::PenDialog(const QPen &initPen, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , pen(initPen)

    , styleList(new QListWidget(this))
    , capStyleList(new QListWidget(this))
    , jointList(new QListWidget(this))
    , widthSpinBox(new QSpinBox(this))
    , miterLimitSpinBox(new QDoubleSpinBox(this))
    , offsetSpinBox(new QDoubleSpinBox(this))
    , colorButton(new mlayout::ColorButton("Sample", QPalette::ButtonText, initPen.brush().color(), this))
    , patternList(new QListWidget(this))
{
    setup();
    setPen(pen);
}

void PenDialog::setup()
{
    QGridLayout *gLayout = new QGridLayout(this);
    setLayout(gLayout);

    {
        QVBoxLayout *styleLayout = new QVBoxLayout;
        QLabel *styleLabel = new QLabel("Style", this);
        QLineEdit *styleLineEdit = new QLineEdit(mutility::enumToString(pen.style()), this);

        gLayout->addLayout(styleLayout, 0, 0, 1, 1);
        styleLayout->addWidget(styleLabel);
        styleLayout->addWidget(styleLineEdit);
        styleLayout->addWidget(styleList);

        styleLayout->setSpacing(0);
        styleLineEdit->setReadOnly(true);
        styleList->addItems(mutility::enumToStrings(Qt::PenStyle(0)));

        connect(styleList, &QListWidget::currentRowChanged, [=](const int& index)
        {
            styleLineEdit->setText(mutility::enumToString(Qt::PenStyle(index)));
            pen.setStyle(Qt::PenStyle(index));
            emit penChanged(pen);
        });
    }
    {
        QVBoxLayout *capStyleLayout = new QVBoxLayout;
        QLabel *capStyleLabel = new QLabel("Cap Style", this);
        QLineEdit *capStyleEdit = new QLineEdit(mutility::enumToString(pen.capStyle()), this);

        gLayout->addLayout(capStyleLayout, 0, 1, 1, 1);
        capStyleLayout->addWidget(capStyleLabel);
        capStyleLayout->addWidget(capStyleEdit);
        capStyleLayout->addWidget(capStyleList);

        capStyleLayout->setSpacing(0);
        capStyleEdit->setReadOnly(true);
        for(int i = 0; i < mutility::getEnumCount(Qt::PenCapStyle(0)); ++i)
        {
            capStyleList->addItem(mutility::enumToString(Qt::PenCapStyle(i * 16)));
        }

        connect(capStyleList, &QListWidget::currentRowChanged, [=](const int& index)
        {
            capStyleEdit->setText(mutility::enumToString(Qt::PenCapStyle(index * 16)));
            pen.setCapStyle(Qt::PenCapStyle(index * 16));
            emit penChanged(pen);
        });
    }
    {
        QVBoxLayout *jointLayout = new QVBoxLayout;
        QLabel *jointLabel = new QLabel("Joint Style", this);
        QLineEdit *jointLineEdit = new QLineEdit(mutility::enumToString(pen.joinStyle()), this);

        gLayout->addLayout(jointLayout, 0, 2, 1, 1);
        jointLayout->addWidget(jointLabel);
        jointLayout->addWidget(jointLineEdit);
        jointLayout->addWidget(jointList);

        jointLayout->setSpacing(0);
        jointLineEdit->setReadOnly(true);
        jointList->addItem(mutility::enumToString(Qt::MiterJoin));
        jointList->addItem(mutility::enumToString(Qt::BevelJoin));
        jointList->addItem(mutility::enumToString(Qt::RoundJoin));
        jointList->addItem(mutility::enumToString(Qt::SvgMiterJoin));

        connect(jointList, &QListWidget::currentRowChanged, [=](const int& index)
        {
            Qt::PenJoinStyle jointStyle;
            switch(index)
            {
            case 0:
                jointStyle = Qt::MiterJoin; break;
            case 1:
                jointStyle = Qt::BevelJoin; break;
            case 2:
                jointStyle = Qt::RoundJoin; break;
            case 3:
                jointStyle = Qt::SvgMiterJoin; break;
            default:
            {
                __LOGOUT__("invalid joint list index", Logger::LogLevel::Debug);
                return;
            }
            }

            jointLineEdit->setText(mutility::enumToString(jointStyle));
            pen.setJoinStyle(jointStyle);
            emit penChanged(pen);
        });
    }
    {
        QFormLayout *fLayout = new QFormLayout;

        gLayout->addLayout(fLayout, 1, 0, 1, 1);
        fLayout->addRow("Width", widthSpinBox);
        fLayout->addRow("Miter Limit", miterLimitSpinBox);
        fLayout->addRow("Dash Offset", offsetSpinBox);

        fLayout->setContentsMargins(1, 8, 0, 0);
        widthSpinBox->setMinimum(1);
        miterLimitSpinBox->setSingleStep(0.1);
        offsetSpinBox->setSingleStep(0.1);

        connect(widthSpinBox, &QSpinBox::valueChanged, [=](const int& value)
        {
            pen.setWidth(value);
            emit penChanged(pen);
        });
        connect(miterLimitSpinBox, &QDoubleSpinBox::valueChanged, [=](const double& value)
        {
            pen.setMiterLimit(value);
            emit penChanged(pen);
        });
        connect(offsetSpinBox, &QDoubleSpinBox::valueChanged, [=](const double& value)
        {
            pen.setDashOffset(value);
            emit penChanged(pen);
        });
    }
    {
        QGroupBox *groupBox = new QGroupBox("Brush", this);
        QFormLayout *fLayout = new QFormLayout;
        QLineEdit *patternLineEdit = new QLineEdit(mutility::enumToString(pen.brush().style()), this);

        gLayout->addWidget(groupBox, 1, 1, 1, 1);
        groupBox->setLayout(fLayout);
        fLayout->addRow("Color", colorButton);
        fLayout->addRow("Pattern", patternLineEdit);
        fLayout->addRow("", patternList);

        for(int i = 0; i < mutility::getEnumCount(Qt::BrushStyle(0)) - 1; ++i)
            patternList->addItem(mutility::enumToString(Qt::BrushStyle(i)));

        patternLineEdit->setReadOnly(true);

        connect(colorButton, &ColorButton::colorChanged, [=](const QColor& color)
        {
            pen.setColor(color);
            emit penChanged(pen);
        });
        connect(patternList, &QListWidget::currentRowChanged, [=](const int& index)
        {
            patternLineEdit->setText(mutility::enumToString(Qt::BrushStyle(index)));
            emit penChanged(pen);
        });
    }
    {
        QGroupBox *groupBox = new QGroupBox("Sample", this);
        QVBoxLayout *vLayout = new QVBoxLayout(groupBox);
        QHBoxLayout *comboLayout = new QHBoxLayout;
        QComboBox *shapeCombo = new QComboBox(groupBox);
        PenSampleWidget *sampleWidget = new PenSampleWidget(pen, groupBox);

        gLayout->addWidget(groupBox, 1, 2, 1, 1);
        groupBox->setLayout(vLayout);
        vLayout->addLayout(comboLayout);
        vLayout->addWidget(sampleWidget);
        comboLayout->addWidget(new QLabel("Sample Shape "));
        comboLayout->addWidget(shapeCombo);

        shapeCombo->setSizePolicy(QSizePolicy::Expanding, shapeCombo->sizePolicy().verticalPolicy());

        shapeCombo->addItems(mutility::enumToStrings(PenSampleWidget::SampleShape(0)));
        shapeCombo->setCurrentIndex((int)sampleWidget->sampleShape());

        connect(shapeCombo, &QComboBox::currentIndexChanged, sampleWidget, &PenSampleWidget::setSampleShape);
        connect(this, &PenDialog::penChanged, sampleWidget, &PenSampleWidget::setSamplePen);
    }
}

void PenDialog::setPen(const QPen& pen)
{
    this->blockSignals(true);

    this->pen = pen;

    styleList->setCurrentRow((int)pen.style());
    capStyleList->setCurrentRow((int)pen.capStyle() / 16);
    switch(pen.joinStyle())
    {
        case Qt::MiterJoin: jointList->setCurrentRow(0); break;
        case Qt::BevelJoin: jointList->setCurrentRow(0); break;
        case Qt::RoundJoin: jointList->setCurrentRow(0); break;
        case Qt::SvgMiterJoin: jointList->setCurrentRow(0); break;
    default:
        __LOGOUT__("invalid Qt::JointStyle enum index.", Logger::LogLevel::Debug);
        break;
    }
    widthSpinBox->setValue(pen.width());
    miterLimitSpinBox->setValue(pen.miterLimit());
    offsetSpinBox->setValue(pen.dashOffset());
    colorButton->setDialogColor(pen.brush().color());
    patternList->setCurrentRow((int)pen.brush().style());

    this->blockSignals(false);
}









PenButton::PenButton(const QString &text, const QPen &pen, QWidget *parent)
    : QPushButton(text, parent)
    , pDialog(new PenDialog(pen, this))
{
    setup();
}

void PenButton::setDialogPen(const QPen &pen)
{
    pDialog->setPen(pen);
}

void PenButton::setup()
{
    connect(this, &PenButton::released, pDialog, &PenDialog::exec);
    connect(pDialog, &PenDialog::penChanged, this, &PenButton::penChanged);
}













}//namespace mlayout

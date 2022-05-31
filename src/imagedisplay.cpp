#include "imagedisplay.h"
#include <QHBoxLayout>

PaintImage::PaintImage(QWidget *parent)
    : QGraphicsView(parent) {}

PaintImage::~PaintImage() {}


void PaintImage::setImage(const QImage &img)
{
    this->img = img;
    viewport()->update();
}

void PaintImage::paintEvent(QPaintEvent*)
{
    QPainter widgetPainter(viewport());
    const QImage img = this->img.scaled(viewport()->width(),
                                        viewport()->height(),
                                        Qt::KeepAspectRatio,
                                        Qt::FastTransformation);

    widgetPainter.drawImage(0, 0, img);

    emit imageResized(img.size());
}








ImageDisplay::ImageDisplay(QWidget *parent)
    : QWidget(parent)
    , painter(new PaintImage(this))
    , originalSize(new QLineEdit(this))
    , currentSize(new QLineEdit(this))
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QHBoxLayout *hLayout = new QHBoxLayout;
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    setLayout(vLayout);
    vLayout->addLayout(hLayout);
    hLayout->addWidget(originalSize);
    hLayout->addWidget(currentSize);
    hLayout->addItem(spacer);
    vLayout->addWidget(painter, 0);

    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);
    originalSize->setReadOnly(true);
    currentSize->setReadOnly(true);    //後々画像サイズを入力で指定できるようにするよいいかも!!
    originalSize->setFixedWidth(60);
    currentSize->setFixedWidth(60);

    connect(painter, &PaintImage::imageResized, this, &ImageDisplay::setCurrentSizeText);

    //ウィンドウ閉じたら自動でdelete
    setAttribute(Qt::WA_DeleteOnClose);
}

ImageDisplay::~ImageDisplay() {}


bool ImageDisplay::setImageFile(const QString &fullPath)
{
    QImage img(fullPath);
    painter->setImage(img);

    originalSize->setText(QString::number(img.size().width()) + ":" +
                          QString::number(img.size().height()));

    return !img.isNull();
}

bool ImageDisplay::isValidExtension(const QString &ext)
{
    if(ext.compare("BMP", Qt::CaseInsensitive) != 0 &&
       ext.compare("GIF", Qt::CaseInsensitive) != 0 &&
       ext.compare("JPG", Qt::CaseInsensitive) != 0 &&
       ext.compare("JPEG", Qt::CaseInsensitive) != 0 &&
       ext.compare("PNG", Qt::CaseInsensitive) != 0 &&
       ext.compare("PBM", Qt::CaseInsensitive) != 0 &&
       ext.compare("PGM", Qt::CaseInsensitive) != 0 &&
       ext.compare("PPM", Qt::CaseInsensitive) != 0 &&
       ext.compare("XBM", Qt::CaseInsensitive) != 0 &&
       ext.compare("XPM", Qt::CaseInsensitive) != 0)
    {
        return false;
    }

    return true;
}

void ImageDisplay::setCurrentSizeText(const QSize& size)
{
    currentSize->setText(QString::number(size.width()) + ":" +
                         QString::number(size.height()));
}

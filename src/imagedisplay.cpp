#include "imagedisplay.h"

PaintImage::PaintImage(QWidget *parent)
    : QGraphicsView(parent) {}

PaintImage::~PaintImage() {}



void PaintImage::paintEvent(QPaintEvent *event)
{
    QPainter widgetPainter(viewport());
    const QImage img = this->img.scaled(viewport()->width(),
                                        viewport()->height(),
                                        Qt::KeepAspectRatio,
                                        Qt::FastTransformation);

    widgetPainter.drawImage(0, 0, img);
}










ImageDisplay::ImageDisplay(QWidget *parent)
    : QWidget(parent)
    , painter(new PaintImage(this))
{
    QHBoxLayout *hLayout = new QHBoxLayout(this);

    setLayout(hLayout);
    hLayout->addWidget(painter, 0);

    //ウィンドウ閉じたら自動でdelete
    setAttribute(Qt::WA_DeleteOnClose);
}

ImageDisplay::~ImageDisplay() {}


bool ImageDisplay::setImageFile(const QString &fullPath)
{
    QImage img(fullPath);
    painter->setImage(img);

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

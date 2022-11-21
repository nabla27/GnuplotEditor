/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

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

void PaintImage::changeImageSize(const QSize& size)
{
    viewport()->resize(size);
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







#include <QFileSystemWatcher>
#include <QTimer>
#include <QLabel>
ImageDisplay::ImageDisplay(QWidget *parent)
    : QWidget(parent)
    , fileWatcher(new QFileSystemWatcher(this))
    , timer(new QTimer(this))
    , painter(new PaintImage(this))
    , originalSize(new QLineEdit(this))
    , currentHeight(new QLineEdit(this))
    , currentWidth(new QLineEdit(this))
{
    //対象の画像ファイルが変更されたら，画像画面が更新するようにする．
    //ファイルに変更があった瞬間に読み込んでも画像はNULLになるのでtimer->setInterval()だけ待つ．
    timer->setSingleShot(true);
    timer->setInterval(500);
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, timer, QOverload<>::of(&QTimer::start));
    connect(timer, &QTimer::timeout, this, &ImageDisplay::updateImage);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QHBoxLayout *hLayout = new QHBoxLayout;
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    setLayout(vLayout);
    vLayout->addLayout(hLayout);
    hLayout->addWidget(new QLabel(" Original size", this));
    hLayout->addWidget(originalSize);
    hLayout->addWidget(new QLabel("Width", this));
    hLayout->addWidget(currentWidth);
    hLayout->addWidget(new QLabel("Height", this));
    hLayout->addWidget(currentHeight);
    hLayout->addItem(spacer);
    vLayout->addWidget(painter, 0);

    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);
    hLayout->setSpacing(4);
    originalSize->setReadOnly(true);
    originalSize->setFixedWidth(60);
    currentHeight->setFixedWidth(30);
    currentWidth->setFixedWidth(30);

    connect(painter, &PaintImage::imageResized, this, &ImageDisplay::setCurrentSizeText);
    connect(currentWidth, &QLineEdit::editingFinished, this, &ImageDisplay::setImageWidth);
    connect(currentHeight, &QLineEdit::editingFinished, this, &ImageDisplay::setImageHeight);
    connect(this, &ImageDisplay::changeImageRequested, painter, &PaintImage::changeImageSize);
}

ImageDisplay::~ImageDisplay() {}

void ImageDisplay::setImagePath(const QString &fullPath)
{
    if(fileWatcher->files().count() > 0) fileWatcher->removePath(imagePath);
    fileWatcher->addPath(fullPath);
    imagePath = fullPath;

    updateImage();
}

void ImageDisplay::updateImage()
{
    QImage img(imagePath);
    painter->setImage(img);

    const QString widthStr = QString::number(img.size().width());
    const QString heightStr = QString::number(img.size().height());

    originalSize->setText(widthStr + ":" + heightStr);
}

void ImageDisplay::setImageWidth()
{
    const int width = currentWidth->text().toInt();
    const int height
            = (float)painter->img.size().height()
            / (float)painter->img.size().width()
            * (float)width;

    //画面の大きさを上部の(ラベル部+画像のViwer部)に設定する．
    this->resize(width, height + currentHeight->height());

    //viwerの大きさを設定する=画像の大きさを設定
    emit changeImageRequested(QSize(width, height));
}

void ImageDisplay::setImageHeight()
{
    const int height = currentHeight->text().toInt();
    const int width
            = (float)painter->img.size().width()
            / (float)painter->img.size().height()
            * (float)height;

    this->resize(width, height + currentWidth->height());

    emit changeImageRequested(QSize(width, height));
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
    currentWidth->setText(QString::number(size.width()));
    currentHeight->setText(QString::number(size.height()));
}

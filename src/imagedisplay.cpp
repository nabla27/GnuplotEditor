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
#include <QFileSystemWatcher>
#include <QTimer>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include "layoutparts.h"
#include "standardpixmap.h"
#include "imageviewer.h"




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








ImageDisplay::ImageDisplay(QWidget *parent)
    : QWidget(parent)
    , fileWatcher(new QFileSystemWatcher(this))
    , timer(new QTimer(this))
    , painter(new PaintImage(this))
    , imageEditor(nullptr)

    , hLayout(new QHBoxLayout)
    , originalSizeEdit(new QLineEdit(this))
    , currentHeightEdit(new QLineEdit(this))
    , currentWidthEdit(new QLineEdit(this))
{
    //対象の画像ファイルが変更されたら，画像画面が更新するようにする．
    //ファイルに変更があった瞬間に読み込んでも画像はNULLになるのでtimer->setInterval()だけ待つ．
    timer->setSingleShot(true);
    timer->setInterval(500);
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, timer, QOverload<>::of(&QTimer::start));
    connect(timer, &QTimer::timeout, this, &ImageDisplay::updateImage);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    mlayout::IconLabel *editImageLabel = new mlayout::IconLabel(this);

    setLayout(vLayout);
    vLayout->addLayout(hLayout);
    hLayout->addWidget(new QLabel(" Original size", this));
    hLayout->addWidget(originalSizeEdit);
    hLayout->addWidget(new QLabel("Width", this));
    hLayout->addWidget(currentWidthEdit);
    hLayout->addWidget(new QLabel("Height", this));
    hLayout->addWidget(currentHeightEdit);
    hLayout->addWidget(editImageLabel);
    hLayout->addItem(spacer);
    vLayout->addWidget(painter, 0);

    static constexpr int iconSize = 20;
    editImageLabel->setAutoFillBackground(true);
    editImageLabel->setHoveredPalette(QPalette(editImageLabel->backgroundRole(), Qt::lightGray));
    editImageLabel->setPixmap(StandardPixmap::Icon::editImage().scaled(iconSize, iconSize));

    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);
    hLayout->setSpacing(4);
    originalSizeEdit->setReadOnly(true);
    originalSizeEdit->setFixedWidth(60);
    currentHeightEdit->setFixedWidth(30);
    currentWidthEdit->setFixedWidth(30);

    connect(painter, &PaintImage::imageResized, this, &ImageDisplay::setCurrentSizeText);
    connect(currentWidthEdit, &QLineEdit::editingFinished, this, &ImageDisplay::setImageWidth);
    connect(currentHeightEdit, &QLineEdit::editingFinished, this, &ImageDisplay::setImageHeight);
    connect(this, &ImageDisplay::changeImageRequested, painter, &PaintImage::changeImageSize);
    connect(editImageLabel, &mlayout::IconLabel::released, this, &ImageDisplay::openImageEditor);
}

ImageDisplay::~ImageDisplay() {}

void ImageDisplay::setImagePath(const QString &fullPath)
{
    if(fileWatcher->files().count() > 0) fileWatcher->removePath(imagePath);
    fileWatcher->addPath(fullPath);
    imagePath = fullPath;

    if(imageEditor)
        imageEditor->setImagePath(imagePath);

    updateImage();
}

void ImageDisplay::updateImage()
{
    QImage img(imagePath);
    painter->setImage(img);

    const QString widthStr = QString::number(img.size().width());
    const QString heightStr = QString::number(img.size().height());

    originalSizeEdit->setText(widthStr + ":" + heightStr);
}

void ImageDisplay::setImageWidth()
{
    const int width = currentWidthEdit->text().toInt();
    const int height
            = (float)painter->img.size().height()
            / (float)painter->img.size().width()
            * (float)width;

    //画面の大きさを上部の(ラベル部+画像のViwer部)に設定する．
    this->resize(width, height + currentHeightEdit->height());

    //viwerの大きさを設定する=画像の大きさを設定
    emit changeImageRequested(QSize(width, height));
}

void ImageDisplay::setImageHeight()
{
    const int height = currentHeightEdit->text().toInt();
    const int width
            = (float)painter->img.size().width()
            / (float)painter->img.size().height()
            * (float)height;

    this->resize(width, height + currentWidthEdit->height());

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
    currentWidthEdit->setText(QString::number(size.width()));
    currentHeightEdit->setText(QString::number(size.height()));
}

void ImageDisplay::openImageEditor()
{
    if(!imageEditor)
    {
        imageEditor = new ImageViewWidget(this);
        imageEditor->setImagePath(imagePath);
        imageEditor->show();
    }
    else
    {
        imageEditor->resize(imageEditor->sizeHint());
        imageEditor->show();
    }
}






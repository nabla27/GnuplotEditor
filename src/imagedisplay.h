/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef IMAGEDISPLAY_H
#define IMAGEDISPLAY_H
#include <QGraphicsView>
#include <QLineEdit>

class PaintImage : public QGraphicsView
{
    Q_OBJECT
public:
    explicit PaintImage(QWidget *parent = nullptr);
    ~PaintImage();

public:
    void setImage(const QImage& img);
    QImage img;

public slots:
    void changeImageSize(const QSize& size);

private:
    void paintEvent(QPaintEvent *event);

signals:
    void imageResized(const QSize& size);
};




class QFileSystemWatcher;
class ImageDisplay : public QWidget
{
    Q_OBJECT
public:
    explicit ImageDisplay(QWidget *parent = nullptr);
    ~ImageDisplay();

public:
    void setImagePath(const QString& fullpath);
    static bool isValidExtension(const QString& ext);

public slots:
    void updateImage();

private slots:
    void setCurrentSizeText(const QSize& size);
    void setImageWidth();
    void setImageHeight();

private:
    QFileSystemWatcher *fileWatcher;
    QTimer *timer;
    QString imagePath;
    PaintImage *painter;
    QLineEdit *originalSize;
    QLineEdit *currentHeight;
    QLineEdit *currentWidth;

signals:
    void changeImageRequested(const QSize& size);
};

#endif // IMAGEDISPLAY_H

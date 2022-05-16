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

private:
    void paintEvent(QPaintEvent *event);

private:
    QImage img;

signals:
    void imageResized(const QSize& size);
};





class ImageDisplay : public QWidget
{
    Q_OBJECT
public:
    explicit ImageDisplay(QWidget *parent = nullptr);
    ~ImageDisplay();

public:
    bool setImageFile(const QString& fullpath);
    static bool isValidExtension(const QString& ext);

public slots:
    void setCurrentSizeText(const QSize& size);

private:
    PaintImage *painter;
    QLineEdit *originalSize;
    QLineEdit *currentSize;
};

#endif // IMAGEDISPLAY_H

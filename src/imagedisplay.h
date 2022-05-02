#ifndef IMAGEDISPLAY_H
#define IMAGEDISPLAY_H
#include <QGraphicsView>
#include <QHBoxLayout>

class PaintImage : public QGraphicsView
{
    Q_OBJECT
public:
    explicit PaintImage(QWidget *parent = nullptr);
    ~PaintImage();

public:
    void setImage(const QImage& img)
    { this->img = img; viewport()->update(); }

private:
    void paintEvent(QPaintEvent *event);

private:
    QImage img;
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

private:
    PaintImage *painter;
};

#endif // IMAGEDISPLAY_H

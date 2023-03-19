#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QScrollArea>
#include <QGraphicsItem>
#include <QDialog>


class QGraphicsPixmapItem;
class QFileSystemWatcher;
class GraphicsItemSettingWidget;
class QLabel;
class QLineEdit;
class QComboBox;
class QSpinBox;
class QCheckBox;
class GraphicsWidgetItem;
class QSplitter;



class ImageScene : public QGraphicsScene
{
    Q_OBJECT
public:
    ImageScene(QObject *parent, QGraphicsPixmapItem *pixmapItem);
    ~ImageScene();

public slots:
    void addGraphicsItem(QGraphicsItem *item);
    void addGraphicsWidget(GraphicsWidgetItem *item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private slots:
    void emitSelectedItems();

private:
    const QGraphicsItem *const pixmapItem; //ownership: ImageViewWidget

    QGraphicsRectItem *const dragRectItem; //ownership: this

signals:
    void currentItemChanged(const QList<QGraphicsItem*>& items);
};






class ImageView : public QGraphicsView
{
    Q_OBJECT
public:
    ImageView(QWidget *parent);
    ~ImageView();

public slots:
    void setAbsoluteScale(const double& scale);
};









class ImageViewWidget : public QScrollArea
{
    Q_OBJECT
public:
    ImageViewWidget(QWidget *parent);
    ~ImageViewWidget();

    QSize sizeHint() const override;

public slots:
    void setImagePath(const QString& path);
    void updateImage();

private slots:
    void adjustViewSize();
    void saveImage();

private:
    QWidget *contents;
    QSplitter *hSplitter;

    QGraphicsPixmapItem *pixmapItem; //ownership: this
    ImageView *imageView;
    ImageScene *imageScene;

    QFileSystemWatcher *watcher;
    QTimer *timer;
    QString imgPath;

    class ImageViewControlPanel;
    ImageViewControlPanel *controlPanel;

    GraphicsItemSettingWidget *settingWidget;
};








class ImageViewWidget::ImageViewControlPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ImageViewControlPanel(QWidget *parent);

signals:
    void scaleChanged(double scale);
    void saveRequested();
    void adjustSizeRequested();
};









class ImageSaveDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ImageSaveDialog(const QString& defaultSavePath = QString());

public:
    void setGrabWidget(QWidget *w);

private slots:
    void setBackgroundColorFromDialog();
    void setPathFromDialog();
    void changePathFromSuffix(const QString& suffix);
    void save();
    void setViewWidth(const int width);
    void setViewHeight(const int height);

private:
    QWidget *viewWidget;
    QLabel *imageLabel;
    QWidget *grabWidget;

    QSpinBox *viewWidthSpinBox;
    QSpinBox *viewHeightSpinBox;

    QLineEdit *pathEdit;
    QComboBox *formatCombo;
    QSpinBox *qualitySpinBox;
    QSpinBox *compressionSpinBox;
    QCheckBox *optimizedWriteCheck;
    QCheckBox *progressiveScanWrteCheck;
};








#endif // IMAGEVIEWER_H

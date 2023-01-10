#include "imageviewer.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QApplication>
#include <QPainterPath>
#include <QGraphicsSceneMouseEvent>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QFileDialog>
#include <QImageWriter>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QScrollBar>
#include <QCheckBox>
#include <QColorDialog>
//ImageScene::setSceneRect()

#include "graphicitems.h"
#include "graphicitemwidget.h"
#include "layoutparts.h"
#include "standardpixmap.h"
#include "logger.h"



ImageScene::ImageScene(QObject *parent, QGraphicsPixmapItem *item)
    : QGraphicsScene(parent)
    , pixmapItem(item)
    , dragRectItem(new QGraphicsRectItem)
{
    addItem(item);
    addItem(dragRectItem);

    dragRectItem->hide();
    dragRectItem->setPen(QPen(QBrush(Qt::black), 1, Qt::PenStyle::DashLine));

    /* ユーザーが追加したアイテムのZValueの値は 0 <= Z <= 99 とする */
    item->setZValue(-1);
    dragRectItem->setZValue(100);

    connect(this, &ImageScene::selectionChanged, this, &ImageScene::emitSelectedItems);
}

ImageScene::~ImageScene()
{
    if(dragRectItem) delete dragRectItem;
}

void ImageScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    dragRectItem->hide();

    //ここでアイテムの選択状態が決定される
    QGraphicsScene::mousePressEvent(event);

    if(event->button() == Qt::LeftButton)
    {
        if(selectedItems().count() == 0)
        {
            const QPointF dragStartPos = event->buttonDownScenePos(Qt::LeftButton);

            dragRectItem->setRect(dragStartPos.x(), dragStartPos.y(), 0, 0);
            dragRectItem->show();
        }
    }
}

void ImageScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(dragRectItem->isVisible())
    {
        QPainterPath path;
        path.addRect(dragRectItem->rect());
        setSelectionArea(path);

        dragRectItem->hide();
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

void ImageScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(dragRectItem->isVisible())
    {
        const QPointF dragStartPos = event->buttonDownScenePos(Qt::LeftButton);

        const QPointF size = event->scenePos() - dragStartPos;

        dragRectItem->setRect(dragStartPos.x(), dragStartPos.y(), size.x(), size.y());
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void ImageScene::addGraphicsItem(QGraphicsItem *item)
{
    //this scene take ownership of the item
    item->setFlag(QGraphicsItem::ItemIsMovable, true);
    item->setFlag(QGraphicsItem::ItemIsSelectable, true);
    addItem(item);
}

void ImageScene::addGraphicsWidget(GraphicsWidgetItem *item)
{
    //this scene take ownership of the item
    QGraphicsProxyWidget *proxyWidget = addWidget(item);
    item->setProxyWidget(proxyWidget);

    proxyWidget->setFlag(QGraphicsItem::ItemIsMovable);
    proxyWidget->setFlag(QGraphicsItem::ItemIsSelectable);
}

void ImageScene::emitSelectedItems()
{
    emit currentItemChanged(selectedItems());
}











ImageView::ImageView(QWidget *parent)
    : QGraphicsView(parent)
{
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

ImageView::~ImageView()
{

}

void ImageView::setAbsoluteScale(const double &scale)
{
    static double prevScale = 1.0;

    const double ratio = scale / prevScale;

    this->scale(ratio, ratio);

    prevScale = scale;
}







#include <QSplitter>

ImageViewWidget::ImageViewWidget(QWidget *parent)
    : QScrollArea(parent)
    , contents(new QWidget(this))
    , hSplitter(new QSplitter(Qt::Horizontal, this))

    , pixmapItem(new QGraphicsPixmapItem)
    , imageView(new ImageView(contents))
    , imageScene(new ImageScene(imageView, pixmapItem))

    , watcher(new QFileSystemWatcher(this))
    , timer(new QTimer(this))

    , controlPanel(new ImageViewControlPanel(contents))
    , settingWidget(new GraphicsItemSettingWidget(contents))
{
    setWindowFlag(Qt::Window);

    //DEBUG
    //GraphicsMathJaxItem *m = new GraphicsMathJaxItem(nullptr, nullptr);
    //imageScene->addGraphicsItem(m);
    //imageScene->addWidget(m->widget());
    //QGraphicsProxyWidget *w = m->createProxyForChildWidget(new QTreeWidget);
    //imageScene->addGraphicsItem(w);
    //QGraphicsProxyWidget *pw = imageScene->addWidget(new QTableWidget);
    //pw->setFlag(QGraphicsItem::ItemIsMovable);
    //pw->setFlag(QGraphicsItem::ItemIsSelectable);
    //pw->setFlag(QGraphicsItem::ItemIsFocusable);
    //connect(imageScene, &ImageScene::currentItemChanged, [=](const QList<QGraphicsItem*>& items)
    //{
    //    qDebug() << items;
    //});

    imageView->setScene(imageScene);

    timer->setInterval(500);
    timer->setSingleShot(true);

    QVBoxLayout *vLayout = new QVBoxLayout(contents);
    //QHBoxLayout *hLayout = new QHBoxLayout;

    setWidget(contents);
    contents->setLayout(vLayout);
    vLayout->addWidget(controlPanel);
    //vLayout->addLayout(hLayout);
    //hLayout->addWidget(imageView);
    //hLayout->addWidget(settingWidget);
    vLayout->addWidget(hSplitter);
    hSplitter->addWidget(imageView);
    hSplitter->addWidget(settingWidget);

    setWidgetResizable(true);

    controlPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    imageView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    settingWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);
    //hLayout->setSpacing(0);
    //hLayout->setContentsMargins(0, 0, 0, 0);
    hSplitter->setContentsMargins(0, 0, 0, 0);

    connect(watcher, &QFileSystemWatcher::fileChanged, timer, QOverload<void>::of(&QTimer::start));
    connect(timer, &QTimer::timeout, this, &ImageViewWidget::updateImage);

    connect(imageScene, &ImageScene::currentItemChanged, settingWidget, &GraphicsItemSettingWidget::setCurrentItems);
    connect(controlPanel, &ImageViewControlPanel::scaleChanged, imageView, &ImageView::setAbsoluteScale);
    connect(controlPanel, &ImageViewControlPanel::adjustSizeRequested, this, &ImageViewWidget::adjustViewSize);
    connect(controlPanel, &ImageViewControlPanel::saveRequested, this, &ImageViewWidget::saveImage);
    connect(settingWidget, &GraphicsItemSettingWidget::graphicsItemCreated, imageScene, &ImageScene::addGraphicsItem);
    connect(settingWidget, &GraphicsItemSettingWidget::graphicsWidgetCreated, imageScene, &ImageScene::addGraphicsWidget);
}

ImageViewWidget::~ImageViewWidget()
{
    if(pixmapItem)
    {
        imageScene->removeItem(pixmapItem);
        delete pixmapItem;
    }

    /* to avoid the runtime error
     * ""
     * ASSERT failure in ImageScene: "Called object is not of the correct type (class destructor may have already run)",
     * file C:\Qt\6.4.1\msvc2019_64\include\QtCore/qobjectdefs_impl.h, line 120
     * ""
     * ImageScene::currentItemChanged() -> GraphicsItemSettingWidget::setCurrentItem()
     */
    imageScene->blockSignals(true);
    imageScene->clearSelection();
}

QSize ImageViewWidget::sizeHint() const
{
    const int frameWidth = imageView->frameWidth();

    return imageView->sizeHint()
            + QSize(settingWidget->sizeHint().width() + frameWidth * 2 + hSplitter->handleWidth(),
                    controlPanel->sizeHint().height() + frameWidth * 2);
}

void ImageViewWidget::setImagePath(const QString &path)
{
    if(!watcher->files().isEmpty())
        watcher->removePath(path);

    watcher->addPath(path);

    imgPath = path;

    updateImage();
    resize(sizeHint());
}

void ImageViewWidget::updateImage()
{
    QPixmap pixmap(imgPath);
    pixmapItem->setPixmap(pixmap);

    /* sceneRect()が拡張されないようにする．
     * QGraphicsScene::sceneRect()は拡張されるが，QGraphicsView::sceneRect()
     * は拡張されない */
    imageView->setSceneRect(0, 0, pixmap.size().width(), pixmap.size().height());
}

void ImageViewWidget::adjustViewSize()
{
    /* this->width() = imageView->width()
     *               + settingWidget->width()
     *               + hSplitter->handleWidget()
     *               + frameWidth * 2               //left and right frame line width
     */

    for(int i = 0; i < 30/*max adjust count*/; ++i)
    {

        const int widthHint = imageView->sizeHint().width()
                            + settingWidget->width()
                            + hSplitter->handleWidth()
                            + imageView->frameWidth() * 2;
        const int heightHint = imageView->sizeHint().height()
                             + controlPanel->sizeHint().height()
                             + imageView->frameWidth() * 2;

        resize(widthHint, heightHint);
        hSplitter->setSizes(QList<int>() << imageView->sizeHint().width() - 1 << settingWidget->width() - 1);

        qDebug() << __FILE__ << __LINE__ << "adjust size count:" << i + 1;

        if(imageView->sizeHint() == imageView->size())
        {
            if(imageView->horizontalScrollBar()->isVisible())
                imageView->resize(imageView->sizeHint() + QSize(1, 1));
            else
                break;
        }
    }
}

void ImageViewWidget::saveImage()
{
    imageView->viewport()->update();
    imageScene->clearSelection();

    ImageSaveDialog dialog(imgPath);
    dialog.setGrabWidget(imageView->viewport());
    dialog.exec();
}











ImageViewWidget::ImageViewControlPanel::ImageViewControlPanel(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    QDoubleSpinBox *scaleSpinBox = new QDoubleSpinBox(this);
    mlayout::IconLabel *adjustSizeLabel = new mlayout::IconLabel(this);
    mlayout::IconLabel *saveAsLabel = new mlayout::IconLabel(this);

    setLayout(hLayout);
    hLayout->addWidget(new QLabel("Scale"));
    hLayout->addWidget(scaleSpinBox);
    hLayout->addWidget(adjustSizeLabel);
    hLayout->addWidget(saveAsLabel);
    hLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

    static constexpr int iconSize = 20;
    adjustSizeLabel->setAutoFillBackground(true);
    saveAsLabel->setAutoFillBackground(true);
    adjustSizeLabel->setPixmap(StandardPixmap::Icon::adjustSize().scaled(iconSize, iconSize));
    saveAsLabel->setPixmap(StandardPixmap::File::fileSaves().scaled(iconSize, iconSize));
    adjustSizeLabel->setHoveredPalette(QPalette(adjustSizeLabel->backgroundRole(), Qt::lightGray));
    saveAsLabel->setHoveredPalette(QPalette(saveAsLabel->backgroundRole(), Qt::lightGray));

    adjustSizeLabel->setToolTip("Adjust scene size");
    saveAsLabel->setToolTip("save scene");

    scaleSpinBox->setValue(1.0);
    scaleSpinBox->setSingleStep(0.1);
    scaleSpinBox->setMinimum(0.01);
    scaleSpinBox->setMaximum(99);

    static constexpr int space = 4;
    setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(space);
    hLayout->setContentsMargins(space, 0, 0, 0);

    connect(scaleSpinBox, &QDoubleSpinBox::valueChanged, this, &ImageViewControlPanel::scaleChanged);
    connect(adjustSizeLabel, &mlayout::IconLabel::released, this, &ImageViewControlPanel::adjustSizeRequested);
    connect(saveAsLabel, &mlayout::IconLabel::released, this, &ImageViewControlPanel::saveRequested);
}











ImageSaveDialog::ImageSaveDialog(const QString& defaultSavePath)
    : QDialog()
    , viewWidget(new QWidget(this))
    , imageLabel(new QLabel(viewWidget))
    , grabWidget(nullptr)

    , viewWidthSpinBox(new QSpinBox(this))
    , viewHeightSpinBox(new QSpinBox(this))
    , pathEdit(new QLineEdit(defaultSavePath, this))
    , formatCombo(new QComboBox(this))
    , qualitySpinBox(new QSpinBox(this))
    , compressionSpinBox(new QSpinBox(this))
    , optimizedWriteCheck(new QCheckBox("Optimized", this))
    , progressiveScanWrteCheck(new QCheckBox("Progressive Scan", this))
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QHBoxLayout *upperLayout = new QHBoxLayout;
    QVBoxLayout *viewLayout = new QVBoxLayout;
    QHBoxLayout *viewOptionLayout = new QHBoxLayout;
    mlayout::IconLabel *backgroundColorLabel = new mlayout::IconLabel(this);
    QVBoxLayout *optionLayout = new QVBoxLayout;
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *saveButton = new QPushButton("Save", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);

    viewWidget->setAutoFillBackground(true);

    setLayout(vLayout);
    vLayout->addLayout(upperLayout);
    vLayout->addLayout(buttonLayout);
    upperLayout->addWidget(viewWidget);
    upperLayout->addLayout(optionLayout);
    viewWidget->setLayout(viewLayout);
    viewLayout->addLayout(viewOptionLayout);
    viewLayout->addWidget(imageLabel);
    viewLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));
    viewOptionLayout->addWidget(backgroundColorLabel);
    viewOptionLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));
    buttonLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);

    static constexpr int iconSize = 20;
    backgroundColorLabel->setAutoFillBackground(true);
    backgroundColorLabel->setHoveredPalette(QPalette(backgroundColorLabel->backgroundRole(), Qt::lightGray));
    backgroundColorLabel->setPixmap(StandardPixmap::Edit::backgroundColor().scaled(iconSize, iconSize));

    pathEdit->setReadOnly(true);
    pathEdit->setMinimumWidth(150);
    setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);
    upperLayout->setSpacing(0);
    upperLayout->setContentsMargins(0, 0, 0, 0);
    viewOptionLayout->setSpacing(2);
    viewOptionLayout->setContentsMargins(0, 0, 0, 0);
    optionLayout->setSpacing(5);
    optionLayout->setContentsMargins(5, 0, 0, 5);
    buttonLayout->setSpacing(0);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *sizeLayout = new QHBoxLayout;
    QHBoxLayout *pathLayout = new QHBoxLayout;
    mlayout::IconLabel *openDialogLabel = new mlayout::IconLabel("...", this);
    QLabel *sizeLabel = new QLabel("Size", this);
    QLabel *pathLabel = new QLabel("Path", this);
    QLabel *formatLabel = new QLabel("Format", this);
    QLabel *qualityLabel = new QLabel("Quality", this);
    QLabel *compressionLabel = new QLabel("Compression", this);

    sizeLabel->setToolTip("image size");
    pathLabel->setToolTip("path for save.");
    pathEdit->setToolTip(defaultSavePath);
    openDialogLabel->setToolTip("open file dialog");
    formatLabel->setToolTip("image format.");
    qualityLabel->setToolTip("setting of the image format to quality.\n"
                             "The value range of quality depends on the image format.\n"
                             "For example, the \"jpeg\" format supports a quality range from 0 (low visual quality,\n"
                             "high compression) to 100 (high visual quality, low compression).");
    compressionLabel->setToolTip("This is an image format specific function that set the compression of an image.\n"
                                 "For image formats that do not support setting the compression, this value is ignored.\n"
                                 "The value range of compression depends on the image format. For example, the \"tiff\" format \n"
                                 "supports two values, 0(no compression) and 1(LZW-compression).");
    optimizedWriteCheck->setToolTip("This is an image format-specific function which sets the optimize flags when writing images. \n"
                                    "For image formats that do not support setting an optimize flag, this value is ignored.");
    progressiveScanWrteCheck->setToolTip("This is an image format-specific function which turns on progressive scanning when writing images. \n"
                                         "For image formats that do not support setting a progressive scan flag, this value is ignored.");

    optionLayout->addWidget(sizeLabel);
    optionLayout->addLayout(sizeLayout);
    sizeLayout->addWidget(viewWidthSpinBox);
    sizeLayout->addWidget(viewHeightSpinBox);
    optionLayout->addWidget(pathLabel);
    optionLayout->addLayout(pathLayout);
    pathLayout->addWidget(pathEdit);
    pathLayout->addWidget(openDialogLabel);
    optionLayout->addWidget(formatLabel);
    optionLayout->addWidget(formatCombo);
    optionLayout->addWidget(qualityLabel);
    optionLayout->addWidget(qualitySpinBox);
    optionLayout->addWidget(compressionLabel);
    optionLayout->addWidget(compressionSpinBox);
    optionLayout->addWidget(optimizedWriteCheck);
    optionLayout->addWidget(progressiveScanWrteCheck);
    optionLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));

    QPalette p = backgroundColorLabel->palette();
    p.setColor(backgroundColorLabel->backgroundRole(), palette().color(backgroundRole()));
    backgroundColorLabel->setPalette(p);

    pathLayout->setSpacing(0);
    openDialogLabel->setAutoFillBackground(true);
    openDialogLabel->setHoveredPalette(QPalette(openDialogLabel->backgroundRole(), Qt::lightGray));
    openDialogLabel->setFixedSize(20, 20);
    openDialogLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    for(const QByteArray& f : QImageWriter::supportedImageFormats())
        formatCombo->addItem(QString(f));
    if(defaultSavePath.isEmpty())
        formatCombo->setCurrentText("png");
    else
        formatCombo->setCurrentText(QFileInfo(defaultSavePath).suffix());
    qualitySpinBox->setMinimum(-1);
    qualitySpinBox->setMaximum(100000);
    qualitySpinBox->setValue(-1);
    compressionSpinBox->setMinimum(-1);
    compressionSpinBox->setValue(-1);
    viewWidthSpinBox->setMaximum(10000);
    viewHeightSpinBox->setMaximum(10000);

    connect(backgroundColorLabel, &mlayout::IconLabel::released, this, &ImageSaveDialog::setBackgroundColorFromDialog);
    connect(viewWidthSpinBox, &QSpinBox::valueChanged, this, &ImageSaveDialog::setViewWidth);
    connect(viewHeightSpinBox, &QSpinBox::valueChanged, this, &ImageSaveDialog::setViewHeight);
    connect(openDialogLabel, &mlayout::IconLabel::released, this, &ImageSaveDialog::setPathFromDialog);
    connect(formatCombo, &QComboBox::currentTextChanged, this, &ImageSaveDialog::changePathFromSuffix);
    connect(saveButton, &QPushButton::released, this, &ImageSaveDialog::save);
    connect(cancelButton, &QPushButton::released, this, &ImageSaveDialog::close);
}

void ImageSaveDialog::setGrabWidget(QWidget *w)
{
    if(!w) return;

    grabWidget = w;

    //シグナルによって画像がセットされる
    viewWidthSpinBox->setValue(w->width());
    viewHeightSpinBox->setValue(w->height());
}

void ImageSaveDialog::setViewWidth(const int width)
{
    if(grabWidget)
        imageLabel->setPixmap(grabWidget->grab(QRect(0, 0, width, viewHeightSpinBox->value())));
}

void ImageSaveDialog::setViewHeight(const int height)
{
    if(grabWidget)
        imageLabel->setPixmap(grabWidget->grab(QRect(0, 0, viewWidthSpinBox->value(), height)));
}

void ImageSaveDialog::setBackgroundColorFromDialog()
{
    QColorDialog dialog(palette().color(backgroundRole()), this);
    connect(&dialog, &QColorDialog::currentColorChanged, [this](const QColor& color)
    {
        viewWidget->setPalette(QPalette(backgroundRole(), color));
    });
    dialog.exec();
}

void ImageSaveDialog::setPathFromDialog()
{
    const QString savePath = QFileDialog::getSaveFileName(this, "Save Path");

    if(savePath.isEmpty()) return;

    pathEdit->setText(savePath);
    pathEdit->setToolTip(savePath);

    formatCombo->setCurrentText(QFileInfo(savePath).suffix());
}

void ImageSaveDialog::changePathFromSuffix(const QString &suffix)
{
    if(pathEdit->text().isEmpty()) return;

    QFileInfo info(pathEdit->text());
    const QString newName = info.absolutePath() + "/" + info.baseName() + "." + suffix;

    pathEdit->setText(newName);
    pathEdit->setToolTip(newName);
}

void ImageSaveDialog::save()
{
    QImageWriter writer(pathEdit->text());

    writer.setFormat(QByteArray(formatCombo->currentText().toUtf8()));
    writer.setQuality(qualitySpinBox->value());
    writer.setCompression(compressionSpinBox->value());
    writer.setOptimizedWrite(optimizedWriteCheck->isChecked());
    writer.setProgressiveScanWrite(progressiveScanWrteCheck->isChecked());

    writer.write(imageLabel->pixmap().toImage());

    close();
}










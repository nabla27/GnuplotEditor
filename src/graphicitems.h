#ifndef GRAPHICITEMS_H
#define GRAPHICITEMS_H

#include <QGraphicsPixmapItem>
#include <QGraphicsLineItem>
#include <QGraphicsItemGroup>

#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>

#include <QGraphicsSvgItem>
#include <QGraphicsTextItem>
#include <QGraphicsWidget>
//sin
//table
//らせん

#include <QPen>



class GraphicsItemInfo : public QObject
{
    Q_OBJECT
public:
    explicit GraphicsItemInfo(QObject *parent);

    enum class ItemType { Text,
                          Shape,
                          Graphics,
                          Widget,
                          Plugin };

    enum class GraphicsItem { SimpleText = 0, PlainText,
                              Arrow = 100, Ellipse, Polygon, Rect, Path,
                              Image = 200, Svg,
                              Table = 300,
                            };

    Q_ENUM(ItemType)
    Q_ENUM(GraphicsItem)
};

class GraphicsPixmapItem : public GraphicsItemInfo, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit GraphicsPixmapItem(QGraphicsItem *parent, QObject *obj);
};

class GraphicsArrowItem : public GraphicsItemInfo, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit GraphicsArrowItem(QGraphicsItem *parent, QObject *obj);

    enum class ArrowType { NoHead, OneSide, BothSides };
    enum class HeadType { Line, Triangle };
    Q_ENUM(ArrowType)
    Q_ENUM(HeadType)

public:
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    ArrowType arrowType() const { return _arrowType; }
    HeadType headType() const { return _headType; }
    Qt::BrushStyle brushStyle() const { return _brushStyle; }
    const QColor& brushColor() const { return _brushColor; }
    const QPen& pen() const { return _pen; }
    double length() const { return _length; }
    double headAngle() const { return _headAngle; }
    double headLength() const { return _headLength; }

public slots:
    void setArrowType(const int index);
    void setHeadType(const int index);
    void setBrushStyle(const int index);
    void setBrushColor(const QColor& color);
    void setPen(const QPen& pen);
    void setLength(const double& length);
    void setHeadAngle(const double& angle);
    void setHeadLength(const double& length);

private:
    ArrowType _arrowType;
    HeadType _headType;
    Qt::BrushStyle _brushStyle;
    QColor _brushColor;
    QPen _pen;
    double _length;
    double _headAngle;
    double _headLength;
};

class GraphicsItemGroup : public GraphicsItemInfo, public QGraphicsItemGroup
{
    Q_OBJECT
public:
    explicit GraphicsItemGroup(QGraphicsItem *parent, QObject *obj);
};

class GraphicsEllipseItem : public GraphicsItemInfo, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    explicit GraphicsEllipseItem(QGraphicsItem *parent, QObject *obj);
};

class GraphicsPathItem : public GraphicsItemInfo, public QGraphicsPathItem
{
    Q_OBJECT
public:
    explicit GraphicsPathItem(QGraphicsItem *parent, QObject *obj);
};

class GraphicsPolygonItem : public GraphicsItemInfo, public QGraphicsPolygonItem
{
    Q_OBJECT
public:
    explicit GraphicsPolygonItem(QGraphicsItem *parent, QObject *obj);
};

class GraphicsRectItem : public GraphicsItemInfo, public QGraphicsRectItem
{
    Q_OBJECT
public:
    explicit GraphicsRectItem(QGraphicsItem *parent, QObject *obj);
};

class GraphicsSimpleTextItem : public GraphicsItemInfo, public QGraphicsSimpleTextItem
{
    Q_OBJECT
public:
    explicit GraphicsSimpleTextItem(QGraphicsItem *parent, QObject *obj);

public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    bool isBackgroundEnable() const { return _isBackgroundEnable; }
    QColor backgroundColor() const { return _backgroundColor; }
    bool isFrameVisible() const { return _isFrameVisible; }
    Qt::PenStyle framePenStyle() const { return _framePenStyle; }
    int frameWidth() const { return _frameWidth; }
    QColor frameColor() const { return _frameColor; }

public slots:
    void setBackgroundEnable(bool enable);
    void setBackgroundColor(const QColor& color);
    void setFrameVisible(bool visible);
    void setFramePenStyle(const Qt::PenStyle& style);
    void setFrameWidth(const int& width);
    void setFrameColor(const QColor& color);

private:
    bool _isBackgroundEnable;
    QColor _backgroundColor;
    bool _isFrameVisible;
    Qt::PenStyle _framePenStyle;
    int _frameWidth;
    QColor _frameColor;
};

class GraphicsSvgItem : public GraphicsItemInfo, public QGraphicsSvgItem
{
    Q_OBJECT
public:
    explicit GraphicsSvgItem(QGraphicsItem *parent, QObject *obj);
};

class GraphicsTextItem : public GraphicsItemInfo, public QGraphicsTextItem
{
    Q_OBJECT
public:
    explicit GraphicsTextItem(QGraphicsItem *parent, QObject *obj);
};








#endif // GRAPHICITEMS_H

#include "graphicitems.h"

#include <QPainter>
#include "logger.h"
\

GraphicsItemInfo::GraphicsItemInfo(QObject *parent)
    : QObject(parent)
{

}

GraphicsPixmapItem::GraphicsPixmapItem(QGraphicsItem *parent, QObject *obj)
    : GraphicsItemInfo(obj)
    , QGraphicsPixmapItem(parent)
{

}

GraphicsArrowItem::GraphicsArrowItem(QGraphicsItem *parent, QObject *obj)
    : GraphicsItemInfo(obj)
    , QGraphicsItem(parent)
    , _pen(QPen())
    , _brushStyle(Qt::BrushStyle::SolidPattern)
    , _brushColor(Qt::black)
    , _arrowType(ArrowType::NoHead)
    , _headType(HeadType::Triangle)
    , _length(90.0)
    , _headAngle(30.0)
    , _headLength(10.0)
{
    _pen.setWidth(1);
    _pen.setColor(Qt::black);
}

QRectF GraphicsArrowItem::boundingRect() const
{
    const double halfHeight = qMax(_headLength * qSin(_headAngle / 180.0 * 3.1415), 5.0);

    return QRectF(0.0, - halfHeight, _length, halfHeight * 2);
}

QPainterPath GraphicsArrowItem::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void GraphicsArrowItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(isSelected())
    {
        static QPen pen = QPen(QBrush(Qt::black), 1, Qt::DashLine);
        painter->setPen(pen);
        painter->drawRect(boundingRect());
    }

    painter->setPen(_pen);
    static QBrush brush = QBrush();
    brush.setStyle(_brushStyle);
    brush.setColor(_brushColor);
    painter->setBrush(brush);

    painter->drawLine(0.0, 0.0, _length, 0.0);

    const double head_cos = qCos(_headAngle / 180.0 * 3.1415);
    const double head_sin = qSin(_headAngle / 180.0 * 3.1415);
    const double head_x_width = _headLength * head_cos;
    const double head_y_width = _headLength * head_sin;

    const QPointF left_points[3] = { QPointF(head_x_width, - head_y_width),
                                     QPointF(0.0, 0.0),
                                     QPointF(head_x_width, head_y_width) };
    const QPointF right_points[3] = { QPointF(_length - head_x_width, - head_y_width),
                                      QPointF(_length, 0.0),
                                      QPointF(_length - head_x_width, head_y_width) };

    switch(_headType)
    {
    case HeadType::Line:
    {
        switch(_arrowType)
        {
        case ArrowType::NoHead:
            break;
        case ArrowType::BothSides:
        {
            painter->drawPolyline(left_points, 3);
        }
        case ArrowType::OneSide:
        {
            painter->drawPolyline(right_points, 3);
            break;
        }
        default:
        {
            __LOGOUT__("invalid ArrowType enum index.", Logger::LogLevel::Debug);
            break;
        }
        }
        break;
    }
    case HeadType::Triangle:
    {
        switch(_arrowType)
        {
        case ArrowType::NoHead:
            break;
        case ArrowType::BothSides:
        {
            painter->drawConvexPolygon(left_points, 3);
        }
        case ArrowType::OneSide:
        {
            painter->drawConvexPolygon(right_points, 3);
            break;
        }
        }
        break;
    }
    default:
    {
        __LOGOUT__("invalid HeadType enum index.", Logger::LogLevel::Debug);
        break;
    }
    }
}

void GraphicsArrowItem::setArrowType(const int index)
{
    if((int)_arrowType != index)
    {
        _arrowType = ArrowType(index);
        update();
    }
}

void GraphicsArrowItem::setHeadType(const int index)
{
    if((int)_headType != index)
    {
        _headType = HeadType(index);
        update();
    }
}

void GraphicsArrowItem::setBrushStyle(const int index)
{
    if((int)_brushStyle != index)
    {
        _brushStyle = Qt::BrushStyle(index);
        update();
    }
}

void GraphicsArrowItem::setBrushColor(const QColor &color)
{
    if(_brushColor != color)
    {
        _brushColor = color;
        update();
    }
}

void GraphicsArrowItem::setPen(const QPen &pen)
{
    if(_pen != pen)
    {
        _pen = pen;
        update();
    }
}

void GraphicsArrowItem::setLength(const double &length)
{
    if(_length != length)
    {
        _length = length;
        update();
    }
}

void GraphicsArrowItem::setHeadAngle(const double &angle)
{
    if(_headAngle != angle)
    {
        _headAngle = angle;
        update();
    }
}

void GraphicsArrowItem::setHeadLength(const double &length)
{
    if(_headLength != length)
    {
        _headLength = length;
        update();
    }
}

GraphicsItemGroup::GraphicsItemGroup(QGraphicsItem *parent, QObject *obj)
    : GraphicsItemInfo(obj)
    , QGraphicsItemGroup(parent)
{

}

GraphicsEllipseItem::GraphicsEllipseItem(QGraphicsItem *parent, QObject *obj)
    : GraphicsItemInfo(obj)
    , QGraphicsEllipseItem(parent)
{

}

GraphicsPathItem::GraphicsPathItem(QGraphicsItem *parent, QObject *obj)
    : GraphicsItemInfo(obj)
    , QGraphicsPathItem(parent)
{

}

GraphicsPolygonItem::GraphicsPolygonItem(QGraphicsItem *parent, QObject *obj)
    : GraphicsItemInfo(obj)
    , QGraphicsPolygonItem(parent)
{

}

GraphicsRectItem::GraphicsRectItem(QGraphicsItem *parent, QObject *obj)
    : GraphicsItemInfo(obj)
    , QGraphicsRectItem(parent)
{

}

GraphicsSimpleTextItem::GraphicsSimpleTextItem(QGraphicsItem *parent, QObject *obj)
    : GraphicsItemInfo(obj)
    , QGraphicsSimpleTextItem(parent)
    , _isBackgroundEnable(false)
    , _backgroundColor(Qt::white)
    , _isFrameVisible(false)
    , _framePenStyle(Qt::SolidLine)
    , _frameWidth(1)
    , _frameColor(Qt::black)
{

}

void GraphicsSimpleTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(_isBackgroundEnable)
    {
        painter->fillRect(boundingRect(), QBrush(_backgroundColor));
    }

    if(_isFrameVisible)
    {
        static QPen framePen;
        framePen.setWidth(_frameWidth);
        framePen.setColor(_frameColor);
        framePen.setStyle(_framePenStyle);
        painter->setPen(framePen);
        painter->drawRect(boundingRect());
    }

    QGraphicsSimpleTextItem::paint(painter, option, widget);
}

void GraphicsSimpleTextItem::setBackgroundEnable(bool enable)
{
    if(_isBackgroundEnable != enable)
    {
        _isBackgroundEnable = enable;
        update();
    }
}

void GraphicsSimpleTextItem::setBackgroundColor(const QColor &color)
{
    if(_backgroundColor != color)
    {
        _backgroundColor = color;
        update();
    }
}

void GraphicsSimpleTextItem::setFrameVisible(bool visible)
{
    if(_isFrameVisible != visible)
    {
        _isFrameVisible = visible;
        update();
    }
}

void GraphicsSimpleTextItem::setFramePenStyle(const Qt::PenStyle &style)
{
    if(_framePenStyle != style)
    {
        _framePenStyle = style;
        update();
    }
}

void GraphicsSimpleTextItem::setFrameWidth(const int &width)
{
    if(_frameWidth != width)
    {
        _frameWidth = width;
        update();
    }
}

void GraphicsSimpleTextItem::setFrameColor(const QColor &color)
{
    if(_frameColor != color)
    {
        _frameColor = color;
        update();
    }
}

GraphicsSvgItem::GraphicsSvgItem(QGraphicsItem *parent, QObject *obj)
    : GraphicsItemInfo(obj)
    , QGraphicsSvgItem(parent)
{
    QGraphicsSvgItem::setParent(obj);
}

GraphicsTextItem::GraphicsTextItem(QGraphicsItem *parent, QObject *obj)
    : GraphicsItemInfo(obj)
    , QGraphicsTextItem(parent)
{
    QGraphicsTextItem::setParent(obj);
}

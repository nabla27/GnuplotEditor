#ifndef GRAPHICITEMWIDGET_H
#define GRAPHICITEMWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QScrollArea>
class QStackedWidget;
//class QGraphicsItem;
class GraphicsItemWidget;
class GraphicsWidgetItem;
#include <QGraphicsItem>


class GraphicsItemSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphicsItemSettingWidget(QWidget *parent);

public slots:
    void setCurrentItems(const QList<QGraphicsItem*>& items);

private slots:
    void createGraphicsItem(int type);
    void createGraphicsWidget(int type);

private:
    class GraphicsItemPanel;
    class GraphicsItemTree;
    class GraphicsTreeItem;
    class GraphicsItemSettingArea;

    GraphicsItemPanel *const panel;
    GraphicsItemTree *const tree;
    GraphicsItemSettingArea *const scrollArea;

    QHash<QGraphicsItem*, GraphicsItemWidget*> items;

signals:
    void graphicsItemCreated(QGraphicsItem *item);
    void graphicsWidgetCreated(GraphicsWidgetItem *widget);
};










class GraphicsItemSettingWidget::GraphicsItemPanel : public QWidget
{
    Q_OBJECT
public:
    explicit GraphicsItemPanel(QWidget *parent);

private slots:
    void execItemAddMenu();

private:
    void setupItemMenu();

private:
    QMenu *const itemMenu;

signals:
    void createItemRequested(int);
};










class GraphicsItemSettingWidget::GraphicsItemTree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit GraphicsItemTree(GraphicsItemSettingWidget *parent);

    QSize sizeHint() const override { return QSize(QTreeWidget::sizeHint().width(), 100); }

public:
    void setCurrentItems(const QList<QGraphicsItem*> items);

protected:
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void renameCurrentItem();
    void removeCurrentItem();

private:
    void setupContextMenu();
    void onCustomContextMenu(const QPoint& pos);

private:
    GraphicsItemSettingWidget *settingWidget;
    QMenu *contextMenu;

signals:
    void changeSettingPageRequested(QWidget* w);
};









class GraphicsItemWidget;
class GraphicsItemSettingWidget::GraphicsTreeItem : public QTreeWidgetItem
{
public:
    explicit GraphicsTreeItem(QTreeWidget *treeWidget, QGraphicsItem *item);
    explicit GraphicsTreeItem(GraphicsTreeItem *parent, QGraphicsItem *item);

public:
    QGraphicsItem *const graphicsItem() const { return _item; }

private:
    QGraphicsItem *const _item;
};









class GraphicsItemSettingWidget::GraphicsItemSettingArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit GraphicsItemSettingArea(QWidget *parent);

public slots:
    void setCurrentWidget(QWidget *w);
};












class QFormLayout;
class QCheckBox;
class QDoubleSpinBox;
class GraphicsItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphicsItemWidget(QWidget *parent, QGraphicsItem *item);
    ~GraphicsItemWidget();

public:
    virtual void setupValue();

private slots:
    void setItemVisible(const bool visible);
    void setXPos(const qreal& xpos);
    void setYPos(const qreal& ypos);
    void setZPos(const qreal& zpos);
    void setScale(const qreal& scale);
    void setRotation(const qreal& rotate);

protected:
    QFormLayout *const fLayout;

private:
    QGraphicsItem *const item; //ownership: QGraphicsScene

    QCheckBox *visibleCheckBox;
    QDoubleSpinBox *posXSpinBox;
    QDoubleSpinBox *posYSpinBox;
    QDoubleSpinBox *posZSpinBox;
    QDoubleSpinBox *scaleSpinBox;
    QDoubleSpinBox *rotationSpinBox;
};








class QAbstractGraphicsShapeItem;
namespace mlayout { class ColorButton; }
namespace mlayout { class PenButton; }
class QComboBox;
class AbstractGraphicsShapeWidget : public GraphicsItemWidget
{
    Q_OBJECT
public:
    explicit AbstractGraphicsShapeWidget(QWidget *parent, QAbstractGraphicsShapeItem *item);
    ~AbstractGraphicsShapeWidget();

public:
    void setupValue() override;

private slots:
    void setBrushColor(const QColor& color);
    void setBrushStyle(const int enumIndex);
    void setPen(const QPen& pen);

private:
    QAbstractGraphicsShapeItem *item;

    mlayout::ColorButton *brushColorButton;
    QComboBox *brushStyleCombo;
    mlayout::PenButton *penButton;
};












class GraphicsSimpleTextItem;
class QLineEdit;
class QSpinBox;
namespace mlayout { class FontButton; }
class GraphicsSimpleTextWidget : public AbstractGraphicsShapeWidget
{
    Q_OBJECT
public:
    explicit GraphicsSimpleTextWidget(QWidget *parent, GraphicsSimpleTextItem *item);

public:
    void setupValue() override;

private slots:
    void setText(const QString& text);
    void setItemFont(const QFont& font);

private:
    GraphicsSimpleTextItem *item;

    QLineEdit *textEdit;
    mlayout::FontButton *fontButton;
    QCheckBox *backgroundEnableCheck;
    mlayout::ColorButton *backgroundColorButton;
    QCheckBox *frameVisibleCheck;
    QComboBox *framePenStyleCombo;
    QSpinBox *frameWidthSpinBox;
    mlayout::ColorButton *frameColorButton;
};









class GraphicsArrowItem;
class GraphicsArrowWidget : public GraphicsItemWidget
{
    Q_OBJECT
public:
    explicit GraphicsArrowWidget(QWidget *parent, GraphicsArrowItem *item);

public:
    void setupValue() override;

private:
    GraphicsArrowItem *item;

    QComboBox *arrowTypeCombo;
    QComboBox *headTypeCombo;
    QComboBox *brushStyleCombo;
    mlayout::ColorButton *brushColorButton;
    mlayout::PenButton *penButton;
    QDoubleSpinBox *lengthSpinBox;
    QDoubleSpinBox *headAngleSpinBox;
    QDoubleSpinBox *headLengthSpinBox;
};









class GraphicsMathJaxItem;
class GraphicsMathJaxWidget : public GraphicsItemWidget
{
    Q_OBJECT
public:
    explicit GraphicsMathJaxWidget(QWidget *widget,                     //parentWidget
                                   GraphicsMathJaxItem *item,           //setting target item(QWidget). shared ownership with proxyWidget
                                   QGraphicsProxyWidget *proxyWidget);  //graphics item. scene has this ownership

public:
    void setupValue() override;

private:
    GraphicsMathJaxItem *item;

    QCheckBox *autoCompileCheck;
    QLineEdit *mathStringEdit;
    QSpinBox *widthSpin;
    QSpinBox *heightSpin;
    QDoubleSpinBox *fontSizeSpin;
    mlayout::ColorButton *colorButton;
};






#endif // GRAPHICITEMWIDGET_H

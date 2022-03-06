#ifndef WINDOWMENUBAR_H
#define WINDOWMENUBAR_H

#include <QMenuBar>
#include <QProxyStyle>
#include <QPainter>
#include <QStyleOption>
#include "menubar.h"

class MenuBarStyle : public QProxyStyle
{
    Q_OBJECT
public:
    MenuBarStyle(QStyle *style = nullptr) : QProxyStyle(style) {}
    void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p, const QWidget *w) const override;
};


class WindowMenuBar : public QMenuBar
{
    Q_OBJECT

public:
    explicit WindowMenuBar(QWidget *parent);
};

#endif // WINDOWMENUBAR_H

/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "windowmenubar.h"

/* 内部でウィンドウの生成時やリサイズ時などに自動で呼び出される */
/* 渡されたControlElementに対してスタイルを設定する */
void MenuBarStyle::drawControl(ControlElement element,
                               const QStyleOption *option,    //QStyleで描写するための情報
                               QPainter *painter,
                               const QWidget *widget) const
{

    switch (element)
    {
    case QStyle::CE_MenuBarEmptyArea:
        break;
    case QStyle::CE_MenuBarItem:
/*
//root/src/plugins/styles/windowsvista/qwindowsvistastyle_p_p.h
#define MBI_NORMAL                  1
#define MBI_HOT                     2
#define MBI_PUSHED                  3
#define MBI_DISABLED                4
#define MB_ACTIVE                   1
#define MB_INACTIVE                 2
#define MENU_BARBACKGROUND          7
#define MENU_BARITEM                8
//root/src/plugins/styles/windowsvista/qwindowsvistastyle.cpp
        QWindowsVistaStylePrivate *d = const_cast<QWindowsVistaStylePrivate*>(d_func());
        bool selected = option->state & State_Selected;
        bool pressed = option->state & State_Sunken;
        if (const QStyleOptionMenuItem *mbi = qstyleoption_cast<const QStyleOptionMenuItem *>(option))
        {
            if (mbi->menuItemType == QStyleOptionMenuItem::DefaultItem)
                break;

            QPalette::ColorRole textRole = disabled ? QPalette::Text : QPalette::ButtonText;
            QPixmap pix = mbi->icon.pixmap(proxy()->pixelMetric(PM_SmallIconSize, option, widget), QIcon::Normal);

            int alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
            if (!proxy()->styleHint(SH_UnderlineShortcut, mbi, widget))
                alignment |= Qt::TextHideMnemonic;

            if (widget && mbi->palette.color(QPalette::Window) != Qt::transparent) { // Not needed for QtQuick Controls
                //The rect adjustment is a workaround for the menu not really filling its background.
                XPThemeData theme(widget, painter,
                                  QWindowsXPStylePrivate::MenuTheme,
                                  MENU_BARBACKGROUND, 0, option->rect.adjusted(-1, 0, 2, 1));
                d->drawBackground(theme);
            }

            int stateId = MBI_NORMAL;
            if (disabled)
                stateId = MBI_DISABLED;
            else if (pressed)
                stateId = MBI_PUSHED;
            else if (selected)
                stateId = MBI_HOT;

            XPThemeData theme2(widget, painter,
                               QWindowsXPStylePrivate::MenuTheme,
                               MENU_BARITEM, stateId, option->rect);
            d->drawBackground(theme2);

            if (!pix.isNull())
                drawItemPixmap(painter, mbi->rect, alignment, pix);
            else
                drawItemText(painter, mbi->rect, alignment, mbi->palette, mbi->state & State_Enabled, mbi->text, textRole);
        }
        break;
        */

        /* QMenuからQStyleを描写するためのオプションをもつQStyleOptionMenuItem */
        /* ここの処理がなくても背景は塗りつぶされる */
        if (const QStyleOptionMenuItem *mbi = qstyleoption_cast<const QStyleOptionMenuItem*>(option))
        {
            if (mbi->menuItemType == QStyleOptionMenuItem::DefaultItem) break;

            /* widgetが無効になっているか */
            const bool disabled = !(option->state & QStyle::State_Enabled);

            /* 無効であればMenuのRoleはTextにする */
            const QPalette::ColorRole textRole = disabled ? QPalette::Text : QPalette::ButtonText;

            /* ふつうはnull */
            const QPixmap pix = mbi->icon.pixmap(proxy()->pixelMetric(PM_SmallIconSize, option, widget), QIcon::Normal);

            int alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
            if (!proxy()->styleHint(SH_UnderlineShortcut, mbi, widget))
                alignment |= Qt::TextHideMnemonic;

            if (!pix.isNull())
                drawItemPixmap(painter, mbi->rect, alignment, pix);
            else
                drawItemText(painter, mbi->rect, alignment, mbi->palette, mbi->state & State_Enabled, mbi->text, textRole);
        }
        break;
    default:
        QProxyStyle::drawControl(element, option, painter, widget);
        break;
    }
}

WindowMenuBar::WindowMenuBar(QWidget *parent)
    : QMenuBar(parent)
{
#if 0
    auto p = palette();
    p.setColor(QPalette::Window, Qt::transparent);
    p.setColor(QPalette::Button, Qt::white);
    p.setColor(QPalette::ButtonText, Qt::black);
    setPalette(p);
    setAutoFillBackground(true);
    auto s = style();
    s->setParent(nullptr);
    setStyle(new MenuBarStyle(s));
    s->setParent(this);
#endif
}










































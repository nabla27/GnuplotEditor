/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H

#include <QPixmap>


struct StandardPixmap
{
public:
    static void loadAll();

public:
    struct File
    {
        static void load();
        //static QPixmap *code;
        //static QPixmap *document;
        //static QPixmap *normal;
        //static QPixmap *image;
        //static QPixmap *fileAdd;
        //static QPixmap *fileReload;
        //static QPixmap *folderAdd;
        //static QPixmap *folderOpen;
        //static QPixmap *fileSave;
        //static QPixmap *fileSaveAs;
        static QPixmap category() { return QPixmap(":/icon/file_category"); }
        static QPixmap code() { return QPixmap(":/icon/file_code"); }
        static QPixmap document() { return QPixmap(":/icon/file_doc"); }
        static QPixmap normal() { return QPixmap(":/icon/file_normal"); }
        static QPixmap image() { return QPixmap(":/icon/file_image"); }
        static QPixmap pdf() { return QPixmap(":/icon/file_pdf"); }
        static QPixmap fileAdd() { return QPixmap(":/icon/file_fileadd"); }
        static QPixmap fileReload() { return QPixmap(":/icon/file_filereload"); }
        static QPixmap folderAdd() { return QPixmap(":/icon/file_folderadd"); }
        static QPixmap folderOpen() { return QPixmap(":/icon/file_folderopen"); }
        static QPixmap fileSave() { return QPixmap(":/icon/file_filesave"); }
        static QPixmap fileSaves() { return QPixmap(":/icon/file_filesaveas"); }

    };
    struct Edit
    {
        static void load();
        //static QPixmap *copy;
        //static QPixmap *cut;
        //static QPixmap *paste;
        //static QPixmap *backgroundcolor;
        //static QPixmap *textcolor;
        //static QPixmap *bold;
        //static QPixmap *italic;
        //static QPixmap *underline;
        //static QPixmap *alignJust;
        //static QPixmap *alignLeft;
        //static QPixmap *alignRight;
        //static QPixmap *alignCenter;
        //static QPixmap *alignBaseline;
        //static QPixmap *alignBottom;
        //static QPixmap *alignTop;
        //static QPixmap *alignVCenter;
        //static QPixmap *reverseRol;
        //static QPixmap *reverseRow;
        //static QPixmap *clear;
        //static QPixmap *mergeCells;
        //static QPixmap *splitCells;
        //static QPixmap *insertColLeft;
        //static QPixmap *insertColRight;
        //static QPixmap *insertRowAbove;
        //static QPixmap *insertRowBelow;
        static QPixmap copy() { return QPixmap(":/icon/edit_copy"); }
        static QPixmap cut() { return QPixmap(":/icon/edit_cut"); }
        static QPixmap paste() { return QPixmap(":/icon/edit_paste"); }
        static QPixmap backgroundColor() { return QPixmap(":/icon/edit_backgroundcolor"); }
        static QPixmap textColor() { return QPixmap(":/icon/edit_textcolor"); }
        static QPixmap bold() { return QPixmap(":/icon/edit_bold"); }
        static QPixmap italic() { return QPixmap(":/icon/edit_italic"); }
        static QPixmap underline() { return QPixmap(":/icon/edit_underline"); }
        static QPixmap alignJust() { return QPixmap(":/icon/edit_alignjust"); }
        static QPixmap alignLeft() { return QPixmap(":/icon/edit_alignleft"); }
        static QPixmap alignRight() { return QPixmap(":/icon/edit_alignright"); }
        static QPixmap alignHCenter() { return QPixmap(":/icon/edit_alignhcenter"); }
        static QPixmap alignBaseline() { return QPixmap(":/icon/edit_alignbaseline"); }
        static QPixmap alignBottom() { return QPixmap(":/icon/edit_alignbottom"); }
        static QPixmap alignTop() { return QPixmap(":/icon/edit_aligntop"); }
        static QPixmap alignVCenter() { return QPixmap(":/icon/edit_alignvcenter"); }
        static QPixmap reverseCol() { return QPixmap(":/icon/edit_reversecol"); }
        static QPixmap reverseRow() { return QPixmap(":/icon/edit_reverserow"); }
        static QPixmap clear() { return QPixmap(":/icon/edit_clear"); }
        static QPixmap mergeCells() { return QPixmap(":/icon/edit_mergecells"); }
        static QPixmap splitCells() { return QPixmap(":/icon/edit_splitcells"); }
        static QPixmap insertColLeft() { return QPixmap(":/icon/edit_insertcolleft"); }
        static QPixmap insertColRight() { return QPixmap(":/icon/edit_insertcolright"); }
        static QPixmap insertRowAbove() { return QPixmap(":/icon/edit_insertrowabove"); }
        static QPixmap insertRowBelow() { return QPixmap(":/icon/edit_insertrowbelow"); }
    };
    struct Icon
    {
        static void load();
        //static QPixmap *triangle;
        //static QPixmap *closeWidget;
        //static QPixmap *remove;
        //static QPixmap *separateH;
        //static QPixmap *separateV;
        //static QPixmap *execute;
        //static QPixmap *reload;
        //static QPixmap *find;
        //static QPixmap *openInWindow;
        //static QPixmap *page;
        static QPixmap triangle() { return QPixmap(":/icon/icon_triangle"); }
        static QPixmap closeWidget() { return QPixmap(":/icon/icon_closewidget"); }
        static QPixmap remove() { return QPixmap(":/icon/icon_remove"); }
        static QPixmap separateH() { return QPixmap(":/icon/icon_separateh"); }
        static QPixmap separateV() { return QPixmap(":/icon/icon_separatev"); }
        static QPixmap execute() { return QPixmap(":/icon/icon_execute"); }
        static QPixmap reload() { return QPixmap(":/icon/icon_reload"); }
        static QPixmap find() { return QPixmap(":/icon/icon_find"); }
        static QPixmap openInWindow() { return QPixmap(":/icon/icon_openinwindow"); }
        static QPixmap page() { return QPixmap(":/icon/icon_page"); }
        static QPixmap caution() { return QPixmap(":/icon/icon_caution"); }
        static QPixmap adjustSize() { return QPixmap(":/icon/icon_adjustsize"); }
        static QPixmap plus() { return QPixmap(":/icon/icon_plus"); }
        static QPixmap editImage() { return QPixmap(":/icon/icon_editimage"); }
        static QPixmap trashCan() { return QPixmap(":/icon/icon_trashcan"); }
        static QPixmap gear() { return QPixmap(":/icon/icon_gear"); }
        static QPixmap broom() { return QPixmap(":/icon/icon_broom"); }
    };
    struct Graph
    {
        static void load();
        //static QPixmap *bar;
        //static QPixmap *scatter;
        //static QPixmap *script;
        static QPixmap bar() { return QPixmap(":/icon/graph_bar"); }
        static QPixmap scatter() { return QPixmap(":/icon/graph_scatter"); }
        static QPixmap script() { return QPixmap(":/icon/graph_script"); }
    };

signals:
    void loadFinished();
};








#endif // RESOURCELOADER_H

#include "gnuplotcpl.h"



void changeGnuplotCompleter(QCompleter* completer ,const QString& first, const QString& current)
{
    if(first == "plot" || first == "splot" || first == "replot" || first == "p")
    {
        completer->setModel(getEditCompleter_Fplot());

        if(current == "plot" || current == "p"){ /* ファイル名や前方に定義された関数を参照 */ }
        else if(current == "with" || current == "w"){ completer->setModel(getEditCompleter_Cwith()); }
        else if(current == "linecolor" || current == "lc"){ completer->setModel(getEditCompleter_Clinecolor()); }
        else if(current == "rgb"){ completer->setModel(getEditCompleter_Clinecolor()); }
        else if(current == "smooth"){ completer->setModel(getEditCompleter_Csmooth()); }
    }
    else if(first == "set")
    {
        completer->setModel(getEditCompleter_Foption());

        if(current == "style"){ completer->setModel(getEditCompleter_Cstyle()); }
        else if(current == "clip"){ completer->setModel(getEditCompleter_Cclip()); }
        else if(current == "angles"){ completer->setModel(getEditCompleter_Cangles()); }
        else if(current == "arrow"){ completer->setModel(getEditCompleter_Carrow()); }
        else if(current == "terminal"){ completer->setModel(getEditCompleter_Cterminal()); }
        else if(current == "cntparam"){ completer->setModel(getEditCompleter_Ccntrparam()); }
        else if(current == "contour"){ completer->setModel(getEditCompleter_Ccontour()); }
        else if(current == "mapping"){ completer->setModel(getEditCompleter_Cmapping()); }
        else if(current == "format"){ completer->setModel(getEditCompleter_Cformat()); }
        else if(current == "datafile") { completer->setModel(getEditCompleter_Cdatafile()); }
        else if(current == "size"){ completer->setModel(getEditCompleter_Cc1()); }
        else if(current == "key"){ completer->setModel(getEditCompleter_Cc2()); }
        else if(current == "offset" ||
                current == "dummy" ||
                current == "view"){ completer->setModel(getEditCompleter_Cc3()); }
        else if(current == "rrange" ||
                current == "trange" ||
                current == "xrange" ||
                current == "yrange" ||
                current == "zrange"){ completer->setModel(getEditCompleter_Cr()); }
    }
    else if(first == "show")
    {
        completer->setModel(getEditCompleter_Foption());
    }
}



QAbstractItemModel* getEditCompleter_non()
{
    QStringList list;
    list.clear();
    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}

QAbstractItemModel* getEditCompleter_first()
{
    QStringList list;
    list
<<"break"
<<"cd"
<<"call"
<<"clear"
<<"continue"
<<"do"
<<"evaluate"
<<"exit"
<<"fit"
<<"help"
<<"history"
<<"if"
<<"for"
<<"import"
<<"load"
<<"lower"
<<"pause"
<<"plot"
<<"print"
<<"printerr"
<<"pwd"
<<"quit"
<<"raise"
<<"refresh"
<<"replot"
<<"reread"
<<"reset"
<<"save"
<<"set"
<<"show"
<<"shell"
<<"splot"
<<"stats"
<<"system"
<<"test"
<<"toggle"
<<"undefine"
<<"unset"
<<"update"
<<"while";


    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}

QAbstractItemModel* getEditCompleter_Cwith()
{
    QStringList list;
    list
<<"lines"
<<"dots"
<<"steps"
<<"errorbars"
<<"xerrorbars"
<<"xerrorbar"
<<"xyerrorlines"
<<"points"
<<"impulses"
<<"errorlines"
<<"xerrorlines"
<<"yerorbars"
<<"linespoints"
<<"labels"
<<"histeps"
<<"financebars"
<<"xyerrorbars"
<<"yerrorlines"
<<"surface"
<<"vectors"
<<"parallelaxes"
<<"boxes"
<<"boxplot"
<<"ellipses"
<<"histograms"
<<"rgbalpha"
<<"boxerrorbars"
<<"candlesticks"
<<"filledcurves"
<<"image"
<<"rgbimage"
<<"boxxyerror"
<<"circles"
<<"fillsteps"
<<"pm3d"
<<"zerrorfill"
<<"table"
<< "l"
<< "p"
<< "lp";

    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}

QAbstractItemModel* getEditCompleter_Clinecolor()
{
    QStringList list;
    list    << "grey10"
            << "gray20"
            << "grey20"
            << "gray30"
            << "grey30"
            << "gray40"
            << "grey40"
            << "gray50"
            << "grey50"
            << "gray60"
            << "grey60"
            << "gray70"
            << "grey70"
            << "gray80"
            << "grey80"
            << "gray90"
            << "grey90"
            << "gray100"
            << "grey100"
            << "gray"
            << "grey"
            << "light-gray"
            << "light-grey"
            << "dark-gray"
            << "dark-grey"
            << "red"
            << "light-red"
            << "dark-red"
            << "yellow"
            << "light-yello"
            << "dark-yellow"
            << "green"
            << "light-green"
            << "dark-green"
            << "spring-green"
            << "forest-green"
            << "sea-green"
            << "blue"
            << "light-blue"
            << "dark-blue"
            << "midnight-blue"
            << "navy"
            << "medium-blue"
            << "royalblue"
            << "skyblue"
            << "cyan"
            << "light-cyan"
            << "dark-cyan"
            << "magenta"
            << "light-magenta"
            << "dark-magenta"
            << "turquoise"
            << "light-turquoise"
            << "dark-turquoise"
            << "pink"
            << "light-pink"
            << "dark-pink"
            << "coral"
            << "light-coral"
            << "orange-red"
            << "salmon"
            << "light-salmon"
            << "dark-salmon"
            << "aquamarine"
            << "khaki"
            << "dark-khaki"
            << "goldenrod"
            << "light-goldenrod"
            << "dark-goldenrod"
            << "gold"
            << "beige"
            << "brown"
            << "orange"
            << "dark-orange"
            << "violet"
            << "dark-violet"
            << "plum"
            << "purple"
            << "brack"
            << "rgb";


    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}

QAbstractItemModel* getEditCompleter_Fplot()
{
    QStringList list;
    list    << "with"
            << "linewidth"
            << "pointsize"
            << "pointtype"
            << "title"
            << "notitle"
            << "linecolor"
            << "textcolor"
            << "fillcolor"
            << "using"
            << "smooth"
            << "w"
            << "lw"
            << "ps"
            << "pt"
            << "ti"
            << "noti"
            << "lc"
            << "tc"
            << "fc";

    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}


QAbstractItemModel* getEditCompleter_Csmooth()
{
    QStringList list;
    list
<<"unique"
<<"frequency"
<<"fnormal"
<<"cumulative"
<<"cnormal"
<<"bins"
<<"kdensity"
<<"kdensity bandwidth"
<<"csplines"
<<"acsplines"
<<"mcsplines"
<<"bezier"
<<"sbezier"
<<"unwrap";

    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}


QAbstractItemModel* getEditCompleter_Cstyle()
{
    QStringList list;
    list
<<"function"
<<"data"
<<"arrow"
<<"boxplot"
<<"fill"
<<"fill transparent"
<<"increment"
<<"line"
<<"circle"
<<"rectangle"
<<"ellipse"
<<"textbox";

    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}

QAbstractItemModel* getEditCompleter_Cclip()
{
    QStringList list;
    list
<<"points"
<<"one"
<<"two";

    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}

QAbstractItemModel* getEditCompleter_Cangles()
{
    QStringList list;
    list
<<"degrees"
<<"radians";

    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}

QAbstractItemModel* getEditCompleter_Carrow()
{
    QStringList list;
    list
<<"from , to ,";

    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}

QAbstractItemModel* getEditCompleter_Cc3()
{
    QStringList list;
    list
<<",,,";

    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}

QAbstractItemModel* getEditCompleter_Cc2()
{
    QStringList list;
    list
<<",,";

    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}

QAbstractItemModel* getEditCompleter_Cc1()
{
    QStringList list;
    list
<<",";

    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}

QAbstractItemModel* getEditCompleter_Cr()
{
    QStringList list;
    list
<<"[:]";

    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}

QAbstractItemModel* getEditCompleter_Cterminal()
{
    QStringList list;
    list
<<"caca"
<<"cairolatex"
<<"canvas"
<<"cgm"
<<"context"
<<"domterm"
<<"dumb"
<<"dxf"
<<"eepic"
<<"emf"
<<"emtex"
<<"epscairo"
<<"epslatex"
<<"fig"
<<"gif"
<<"hpgl"
<<"jpeg"
<<"latex"
<<"lua"
<<"mf"
<<"mp"
<<"pcl5"
<<"pdfcairo"
<<"postscript"
<<"pslatex"
<<"pstex"
<<"pstricks"
<<"qms"
<<"qt"
<<"sixelgd"
<<"svg"
<<"texdraw"
<<"tgif"
<<"tikz"
<<"tkcanvas"
<<"tpic"
<<"unknown"
<<"windows"
<<"wxt"
<<"push"
<<"pop";

    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}

QAbstractItemModel* getEditCompleter_Ccntrparam()
{
    QStringList list;
    list
<<"linear"
<<"cubicspline"
<<"bspline"
<<"points"
<<"order"
<<"levels"
<<"levels auto"
<<"levels discrete ,"
<<"levels incremental ,,";


    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}

QAbstractItemModel* getEditCompleter_Ccontour()
{
    QStringList list;
    list
<<"base"
<<"surface"
<<"both";

    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}

QAbstractItemModel* getEditCompleter_Cmapping()
{
    QStringList list;
    list
<<"cartesian"
<<"spherical"
<<"cylindrical";

    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}

QAbstractItemModel* getEditCompleter_Cformat()
{
    QStringList list;
    list
<<"x \"%g\""
<<"x \"%4. 2f\""
<<"x \"%4. 2e\""
<<"y \"%g\""
<<"y \"%4. 2f\""
<<"y \"%4. 2e\""
<<"xy \"%g\""
<<"xy \"%4. 2f\""
<<"xy \"%4. 2e\"";


    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}



QAbstractItemModel* getEditCompleter_Cdatafile()
{
    QStringList list;
    list
<<"separator";


    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}


QAbstractItemModel* getEditCompleter_Foption()
{
    QStringList list;
    list
<<"angles"
<<"arrow"
<<"autoscale"
<<"bind"
<<"bmargin"
<<"border"
<<"boxwidth"
<<"color"
<<"colorsequence"
<<"clabel"
<<"clip"
<<"cntrlabel"
<<"cntrparam"
<<"colorbox"
<<"colornames"
<<"contour"
<<"dashtype"
<<"datafile"
<<"desimalsign"
<<"dgrid3d"
<<"dummy"
<<"encording"
<<"errorbars"
<<"fit"
<<"fontpath"
<<"format"
<<"functions"
<<"grid"
<<"hidden3d"
<<"historysize"
<<"history"
<<"isosamples"
<<"jitter"
<<"key"
<<"label"
<<"linetype"
<<"link"
<<"lmargin"
<<"loadpath"
<<"locale"
<<"logscale"
<<"macros"
<<"mapping"
<<"margin"
<<"micro"
<<"minussign"
<<"monochrome"
<<"mouse"
<<"mttics"
<<"multiplot"
<<"mx2tics"
<<"mxtics"
<<"my2tics"
<<"mytics"
<<"mztics"
<<"nolinear"
<<"object"
<<"offsets"
<<"origin"
<<"output"
<<"parametric"
<<"paxis"
<<"plot"
<<"pm3d"
<<"palette"
<<"pointintervalbox"
<<"pointsize"
<<"polar"
<<"print"
<<"psdir"
<<"raxis"
<<"rgbmax"
<<"rlabel"
<<"rmargin"
<<"rrange"
<<"rtics"
<<"samples"
<<"size"
<<"style"
<<"surface"
<<"table"
<<"terminal"
<<"termoption"
<<"theta"
<<"tics"
<<"ticslevel"
<<"ticscale"
<<"timestamp"
<<"timefmt"
<<"title"
<<"tmargin"
<<"trange"
<<"ttics"
<<"urange"
<<"variables"
<<"version"
<<"view"
<<"vrange"
<<"x2data"
<<"x2dtics"
<<"x2label"
<<"x2mtics"
<<"x2range"
<<"x2tics"
<<"x2zeroaxis"
<<"xdata"
<<"xdtics"
<<"xlabel"
<<"xmtics"
<<"xrange"
<<"xtics"
<<"xyplane"
<<"xzeroaxis"
<<"y2data"
<<"y2dtics"
<<"y2label"
<<"y2mtics"
<<"y2range"
<<"y2tics"
<<"y2zeroaxis"
<<"ydata"
<<"ydtics"
<<"ylabel"
<<"ymtics"
<<"yrange"
<<"ytics"
<<"yzeroaxis"
<<"zdata"
<<"zdtics"
<<"zzeroaxis"
<<"cbdata"
<<"cbdtics"
<<"zero"
<<"zeroaxis"
<<"zlabel"
<<"zmtics"
<<"ztics"
<<"cblabel"
<<"cbmtics"
<<"cbrange"
<<"cbtics";



    QStringListModel *listModel = new QStringListModel();
    listModel->setStringList(list);

    return listModel;
}

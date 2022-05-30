#include "gnuplotcompletion.h"
#include <QDir>
#include <QFileInfo>

namespace gnuplot_cpl
{

void GnuplotCompletionModel::setCompletionList(const QString& firstCmd, const QString& preCmd, const int index)
{
    QStringList list;

    if(index == 0) list << mainCmd();
    else if(firstCmd == "cd")
    {
        if(index == 1) list << dirList;
    }
    else if(firstCmd == "call")
    {
        if(index == 1) list << fileList;
        else if(index >= 2) list << parameter();
    }
    else if(firstCmd == "do")
    {
        if(index == 1) list << "for";
    }
    else if(firstCmd == "exit")
    {
        if(index == 1) list << "message" << "status";
    }
    else if(firstCmd == "fit")
    {
        if(index == 1) list << expression();
        else if(index == 2) list << expression() << fileList;
        else if(index == 3) list << modifiers() << "unitweights" << "error" << "yerror" << "xyerror" << "zerror" << "via";
        else if(index >= 4)
        {
            if(preCmd == "binary") list << binary();
            else if(preCmd == "endian") list << endian();
            else if(preCmd == "filetype") list << filetype();
            else if(preCmd == "nonuniform") list << nonuniform();
            else if(preCmd == "smooth") list << smooth();
            else if(preCmd == "bandwidth") list << bandwidth();
            else if(preCmd == "errors" | preCmd == ",") list << variables();
            else if(preCmd == "via" | preCmd == ",") list << variables();
            else list << modifiers() << "unitweights" << "yerror" << "xyerror" << "zerror" << "error" << "errors" << "via";
        }
    }
    else if(firstCmd == "help")
    {
        if(index == 1) list << helpItems();
    }
    else if(firstCmd == "history")
    {
        if(index == 1) list << historyCmd();
    }
    else if(firstCmd == "load")
    {
        if(index == 1) list << fileList;
    }
    else if(firstCmd == "pause")
    {
        if(index == 1) list << "mouse";
    }
    else if(firstCmd == "plot")
    {
        if(index == 1) list << expression() << fileList << "keyentry";
        else if(index == 2) list << expression() << fileList << "keyentry" << titleSpec() << modifiers() << "matrix" << "name" << "nooutput" << "output" << "using" << "name";
        else if(index >= 3)
        {
            if(preCmd == "axes") list << axes();
            else if(preCmd == "with") list << d2style();
            else if(preCmd == "binary") list << binary();
            else if(preCmd == "endian") list << endian();
            else if(preCmd == "filetype") list << filetype();
            else if(preCmd == "nonuniform") list << nonuniform();
            else if(preCmd == "smooth") list << smooth();
            else if(preCmd == "bandwidth") list << bandwidth();
            else if(preCmd == "filledcurves") list << filledcurves();
            else if(preCmd == "closed") list << closed();
            else list << titleSpec() << modifiers() << style() << "matrix" << "name" << "nooutput" << "output" << "using" << "name";
        }
    }
    else if(firstCmd == "save")
    {
        if(index == 1) list << "functions" << "variables" << "terminal" << "set" << "fit";
    }
    else if(firstCmd == "set" | firstCmd == "show" | firstCmd == "unset")
    {
        if(index == 1) option();
        else if(index == 2)
        {
            if(preCmd == "angle") list << angle();
            else if(preCmd == "arrow") list << arrow();
            else if(preCmd == "autoscale") list << autoscale();
            else if(preCmd == "border") list << border();
            else if(preCmd == "boxwidth") list << boxwidth();
            else if(preCmd == "colorsequence") list << colorsequence();
            else if(preCmd == "clip") list << clip();
            else if(preCmd == "cntrlabel") list << cntrlabel();
            else if(preCmd == "natrparam") list << cntrparam();
            else if(preCmd == "levels") list << levels();
            else if(preCmd == "colorbox")  list << colorbox();
            else if(preCmd == "colornames") list << color();
            else if(preCmd == "contour") list << contour();
            else if(preCmd == "datafile") list << datafile();
            else if(preCmd == "desimalsign") list << desimalsign();
            else if(preCmd == "dgrid3d") list << dgrid3d();
            else if(preCmd == "encording") list << encoding();
            else if(preCmd == "errorbars") list << errorbars();
            else if(preCmd == "fit") list << fit();
            else if(preCmd == "format") list << format();
            else if(preCmd == "grid") list << grid();
            else if(preCmd == "hidden3d") list << hidden3d();
            else if(preCmd == "history") list << history();
            else if(preCmd == "jitter") list << jitter();
            else if(preCmd == "key") list << key_1() << key_2();
            else if(key_1().contains(preCmd)) list << key_2();
            else if(preCmd == "title") list << key_title();
            else if(preCmd == "box") list << key_box();
            else if(preCmd == "maxrc") list << key_maxrc();
            else if(preCmd == "label") list << label();
            else if(preCmd == "link") list << link();
            else if(preCmd == "logscale") list << logscale();
            else if(preCmd == "mapping") list << mapping();
            else if(preCmd == "margin") list << margin();
            else if(preCmd == "monochrome") list << monochrome();
            else if(preCmd == "mouse") list << mouse();
            else if(preCmd == "ruler" || "noruler") list << mouse_ruler();
            else if(preCmd == "multiplot") list << multiplot();
            else if(preCmd == "layout") list << multiplot_layout();
            else if(preCmd == "mxtics" |
                    preCmd == "mytics" |
                    preCmd == "mztics" |
                    preCmd == "mx2tics" |
                    preCmd == "my2tics" |
                    preCmd == "mrtics" |
                    preCmd == "mttics" |
                    preCmd == "mcbtics") list << mxtics();
            else if(preCmd == "nolinear") list << nolinear();
            else if(preCmd == "object") list << object();
            else if(preCmd == "paxis") list << paxis();
            else if(preCmd == "pm3d") list << pm3d();
            else if(preCmd == "flush") list << pm3d_flush();
            else if(preCmd == "corners2color") list << pm3d_corners2color();
            else if(preCmd == "palette") list << palette();
            else if(preCmd == "rgbformulae") list << palette_rgbformulae();
            else if(preCmd == "cubehelix") list << palette_cubehelix();
            else if(preCmd == "model") list << palette_model();
            else if(preCmd == "rgbmax") list << rgbmax();
            else if(preCmd == "size") list << size();
            else if(preCmd == "style") list << ostyle();
            else if(preCmd == "units") list << ostyle_units();
            else if(preCmd == "textbox") list << ostyle_textbox();
            else if(preCmd == "surface") list << surface();
            else if(preCmd == "table") list << table();
            else if(preCmd == "separator") list << table_separator();
            else if(preCmd == "terminal") list << terminal();
            else if(preCmd == "termoption") list << termoption();
            else if(preCmd == "theta") list << theta();
            else if(preCmd == "tics") list << tics();
            else if(preCmd == "rotate") list << tics_rotate();
            else if(preCmd == "scale") list << tics_scale();
            else if(preCmd == "timestamp") list << timestamp();
            else if(preCmd == "title") list << title();
            else if(preCmd == "view") list << view();
            else if(preCmd == "map") list << view_map();
            else if(preCmd == "equal") list << view_equal();
            else if(preCmd == "xlabel" |
                    preCmd == "x2label" |
                    preCmd == "ylabel" |
                    preCmd == "y2label" |
                    preCmd == "zlabel" |
                    preCmd == "cblabel") list << xlabel();
            else if(preCmd == "xrange" |
                    preCmd == "yrange" |
                    preCmd == "zrange" |
                    preCmd == "x2range" |
                    preCmd == "y2range" |
                    preCmd == "cbrange" |
                    preCmd == "rrange" |
                    preCmd == "trange" |
                    preCmd == "urange" |
                    preCmd == "vrange") list << xrange();
            else if(preCmd == "xtics" |
                    preCmd == "ytics" |
                    preCmd == "ztics" |
                    preCmd == "x2tics" |
                    preCmd == "y2tics" |
                    preCmd == "cbtics") list << xtics();
            else if (preCmd == "xyplane") list << xyplane();
        }
        else if(index >= 3)
        {
            if(preCmd == "background") list << color();
            else if(preCmd == "canvas") list << canvas();
            else if(preCmd == "cgm") list << cgm();
            else if(preCmd == "dumb") list << dumb();
            else if(preCmd == "caca") list << caca();
            else if(preCmd == "charset") list << caca_charset();
            else if(preCmd == "emf") list << emf();
            else if(preCmd == "fig") list << fig();
            else if(preCmd == "hpgl") list << hpgl();
            else if(preCmd == "png") list << png();
            else if(preCmd == "jpeg") list << jpeg();
            else if(preCmd == "gif") list << gif();
            else if(preCmd == "sixelgd") list << sixelgd();
            else if(preCmd == "epslatex") list << epslatex();
            else if(preCmd == "pslatex" || preCmd == "pstex") list << pslatex();
            else if(preCmd == "postscript") list << postscript();
            else if(preCmd == "svg") list << svg();
            else if(preCmd == "tgif") list << tgif();
            else if(preCmd == "tkcanvas") list << tkcanvas();
            else if(preCmd == "latex" || preCmd == "emtex") list << latex();
            else if(preCmd == "eepic") list << eepic();
            else if(preCmd == "pstricks") list << pstricks();
            else if(preCmd == "mp") list << mp();
            else if(preCmd == "context") list << context();
            else if(preCmd == "wxt") list << wxt();
            else if(preCmd == "cairolatex") list << cairolatex();
            else if(preCmd == "pdfcairo") list << pdfcairo();
            else if(preCmd == "pngcairo") list << pngcairo();
            else if(preCmd == "qt") list << qt();
        }
    }
    else if(firstCmd == "splot")
    {
        if(index == 1) list << expression() << fileList << "keyentry";
        else if(index == 2) list << expression() << fileList << "keyentry" << titleSpec() << modifiers() << "matrix" << "name" << "nooutput" << "output" << "using" << "with";
        else if(index >= 3)
        {
            if(preCmd == "with") list << d3style();
            else list << titleSpec() << "with" << modifiers() << "matrix" << "name" << "nooutput" << "output" << "using" << style();
        }
    }
    else if(firstCmd == "stats")
    {
        if(index == 1) list << fileList << "nooutput" << "output";
        else if(index == 2) list << fileList << "matrix" << "using" << "name";
    }
    else if(firstCmd == "test")
    {
        if(index == 1) list << "terminal" << "palette";
    }
    else if(firstCmd == "toggle")
    {
        if(index == 1) list << "all";
    }
    else if(firstCmd == "undefine")
    {
        list << variables();
    }
    else if(firstCmd == "update")
    {
        list << fileList;
    }

    emit completionListSet(list);
}



void GnuplotCompletionModel::setToolTip(const QString& command)
{
    QString toolTip;

    if(command == "break") toolTip =  "break";
    else if(command == "cd") toolTip =  "cd <\"DirName\">";
    else if(command == "call") toolTip =  "call <\"InputFile\"> <Parameter1> <Parameter2> ... <Parameter9>";
    else if(command == "clear") toolTip =  "clear";
    else if(command == "continue") toolTip =  "continue";
    else if(command == "do") toolTip =  "do for <InterationSpec> { }";
    else if(command == "eval") toolTip =  "eval <\"StringExpression\">";
    else if(command == "exit") toolTip =  "exit \n""exit message <\"ErrorMessage\"> \n""exit status <ErrorCode>";
    else if(command == "fit") toolTip =  "fit {<Ranges>} <Expression> <DataFile> {datafile-modifiers} {unitweights} via <Var1>{,<Var2>,...}\n"
                                         "fit {<Ranges>} <Expression> <DataFile> {datafile-modifiers} {{x|xy|z}error} via <Var1>{,<Var2>,...}\n"
                                         "fit {<Ranges>} <Expression> <DataFile> {datafile-modifiers} {error <Var1>{,<Var2>,...}} via <Var1>{,<Var2>,...}";
    else if(command == "help") toolTip =  "help <Contents>";
    else if(command == "history") toolTip =  "history\n"
                                         "history {<Count>} <\"OutFile\"> {append}\n"
                                         "history ?<\"Command\">\n"
                                         "history !<\"Command\">\n"
                                         "history !<Count>";
    else if(command == "if") toolTip =  "if (<Condition>) { } else if (<Condition>) { else {} }";
    else if(command == "for") toolTip =  "for [<IntVar> = <Start>:<End>{:<Increment>}]\n"
                                     "for [<StringVar> in \"Str1 Str2 ...\"]";
    else if(command == "import") toolTip =  "import <FuncName>(x{y,z,...}) from <\"ObjectFile\"{:<Symbol>}\">";
    else if(command == "load") toolTip =  "load <\"FileName\">";
    else if(command == "lower") toolTip =  "lower {<PlotWindowId>}";
    else if(command == "pause") toolTip =  "pause <Time> {<\"Message\">}";
    else if(command == "pause") toolTip =  "pause mouse {<EndCondition>}{, <EndCondition>...} {<\"Message\">}";
    else if(command == "plot") toolTip =  "plot {<Ranges>} <Definition> {ases <Axes>} {<TitleSpec>} {with <Style>}\n"
                                      "plot {<Ranges>} {sampling-range} <Function> {axes <Axes>} {<TitleSpec>} {with <Style>}\n"
                                      "plot {<Ranges>} <DataSource> {<Modifier>} {axes <Axes>} {<TitleSpec>} {with <Style>}\n"
                                      "plot {<Ranges>} keyentry {axes <Axes>} {<TitleSpec>} {with <Style>}";
    else if(command == "print") toolTip =  "print <Formula> {, <Formula>, ...}";
    else if(command == "printerr") toolTip =  "print <Formula> {, <Formula>, ...}";
    else if(command == "pwd") toolTip =  "pwd";
    else if(command == "quit") toolTip =  "quit";
    else if(command == "raise") toolTip =  "raise {<PlotWindowId>}";
    else if(command == "refresh") toolTip =  "refresh";
    else if(command == "replot") toolTip =  "replot";
    else if(command == "reread") toolTip =  "reread";
    else if(command == "reset") toolTip =  "reset";
    else if(command == "save") toolTip =  "save {<SaveCmd> {<SaveCmd>,}...} <\"FileName\">";
    else if(command == "set") toolTip =  "set <Option>";
    else if(command == "show") toolTip =  "show <Option>";
    else if(command == "shell") toolTip =  "shell";
    else if(command == "splot") toolTip =  "splot {<Ranges>} <Function> {<TitleSpec>} {with <Style>} {, {definitions} <Function> ...}\n"
                                           "splot {<Ranges>} <FileName> {datafile-modifiers} {<TitleSpec>} {with <Style>} {, {definitions} <Function> ...}\n"
                                           "splot {<Ranges>} <DataBolockName> {<Modifier>} {<TitleSpec>} {with <Style>} {, {definitions} <Function> ...}\n"
                                           "splot {<Ranges>} keyentry {<TitleSpec>} {with <Style>} {, {definitions} <Function> ...}";
    else if(command == "stats") toolTip =  "stats {<Ranges>} <\"FileName\"> {<StatsType>} {name <Prefix>} {{no}output}";
    else if(command == "system") toolTip =  "system <\"Command\">";
    else if(command == "test") toolTip =  "test <TestType>";
    else if(command == "toggle") toolTip =  "toggle <PlotNo>\n"
                                        "toggle <\"Title\">\n"
                                        "toggle all";
    else if(command == "undefine") toolTip =  "undefine <Variable>\n"
                                          "undefine <StrExpression>";
    else if(command == "unset") toolTip =  "unset <Option>";
    else if(command == "update") toolTip =  "update <\"FileName\"> {<\"FileName\">}";
    else if(command == "while") toolTip =  "while (<Expression>) {}";

    emit toolTipSet(toolTip);
}


void GnuplotCompletionModel::setParentFolder(const QString& folderPath)
{
    dirList.clear();
    fileList.clear();
    getFilesRecursively(folderPath, folderPath);
}

void GnuplotCompletionModel::getFilesRecursively(const QString &parentPath, const QString &folderPath)
{
    QDir parent(parentPath);
    QDir dir(folderPath);

    const QList<QFileInfo> infoList = dir.entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::Files | QDir::Filter::Dirs);

    for(const QFileInfo& info : infoList)
    {
        if(info.isFile())
            fileList << "\"" + parent.relativeFilePath(info.absoluteFilePath()) + "\"";
        else
        {
            dirList << "\"" + parent.relativeFilePath(info.absoluteFilePath()) + "\"";   //parentPathからのfolderPathの相対パス
            getFilesRecursively(parentPath, info.absoluteFilePath());                    //フォルダー内を再帰
        }
    }
}





}//gnuplot_cpl

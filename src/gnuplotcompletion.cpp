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
        if(index == 2)
        {
            if(preCmd.contains(']')) list << expression();
            else list << fileList;
        }
        else if(index == 3)
        {
            if(preCmd.contains(']')) list << fileList;
            else
            {
                if(!modifiers().contains(preCmd)) list << modifiers();
                if(preCmd != "unitweights") list << "unitweights";
                if(preCmd != "error") list << "error";
                if(preCmd != "xerror") list << "yerror";
                if(preCmd != "xyerror") list << "xyerror";
                if(preCmd != "zerror") list << "zerror";
                if(preCmd != "via") list << "via";
            }
        }
        else if(index >= 4)
        {
            if(preCmd == "binary") list << binary();
            else if(preCmd == "endian") list << endian();
            else if(preCmd == "filetype") list << filetype();
            else if(preCmd == "nonuniform") list << nonuniform();
            else if(preCmd == "smooth") list << smooth();
            else if(preCmd == "bandwidth") list << bandwidth();
            else if(preCmd == "errors" || preCmd == ",") list << variables();
            else if(preCmd == "via" || preCmd == ",") list << variables();
            else
            {
                if(!modifiers().contains(preCmd)) list << modifiers();
                if(preCmd != "unitweights") list << "unitweights";
                if(preCmd != "error") list << "error";
                if(preCmd != "xerror") list << "yerror";
                if(preCmd != "xyerror") list << "xyerror";
                if(preCmd != "zerror") list << "zerror";
                if(preCmd != "via") list << "via";
            }
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
        else if(index == 2)
        {
            if(preCmd != "axes") list << "axes";
            if(!titleSpec().contains(preCmd)) list << titleSpec();
            if(preCmd != "with") list << "with";
            if(!modifiers().contains(preCmd)) list << modifiers();
            if(preCmd.contains(']'))
            {   //<Ranges>が指定されていた場合のみ
                if(!expression().contains(preCmd)) list << expression();
                if(!fileList.contains(preCmd)) list << fileList;
                if(preCmd != "keyentry") list << "keyentry";
            }
        }
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
            else
            {
                if(preCmd != "axes") list << "axes";
                if(!titleSpec().contains(preCmd)) list << titleSpec();
                if(preCmd != "with") list << "with";
                if(!modifiers().contains(preCmd)) list << modifiers();
                if(!style().contains(preCmd)) list << style();
            }
        }
    }
    else if(firstCmd == "save")
    {
        if(index == 1) list << "functions" << "variables" << "terminal" << "set" << "fit";
    }
    else if(firstCmd == "set" || firstCmd == "show" || firstCmd == "unset")
    {
        if(index == 1) list << option();
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
            else if(preCmd == "ruler" || preCmd == "noruler") list << mouse_ruler();
            else if(preCmd == "multiplot") list << multiplot();
            else if(preCmd == "layout") list << multiplot_layout();
            else if(preCmd == "mxtics" ||
                    preCmd == "mytics" ||
                    preCmd == "mztics" ||
                    preCmd == "mx2tics" ||
                    preCmd == "my2tics" ||
                    preCmd == "mrtics" ||
                    preCmd == "mttics" ||
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
            else if(preCmd == "xlabel" ||
                    preCmd == "x2label" ||
                    preCmd == "ylabel" ||
                    preCmd == "y2label" ||
                    preCmd == "zlabel" ||
                    preCmd == "cblabel") list << xlabel();
            else if(preCmd == "xrange" ||
                    preCmd == "yrange" ||
                    preCmd == "zrange" ||
                    preCmd == "x2range" ||
                    preCmd == "y2range" ||
                    preCmd == "cbrange" ||
                    preCmd == "rrange" ||
                    preCmd == "trange" ||
                    preCmd == "urange" ||
                    preCmd == "vrange") list << xrange();
            else if(preCmd == "xtics" ||
                    preCmd == "ytics" ||
                    preCmd == "ztics" ||
                    preCmd == "x2tics" ||
                    preCmd == "y2tics" ||
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
        else if(index == 2)
        {
            if(preCmd.contains(']'))
            {
                if(!expression().contains(preCmd)) list << expression();
                if(!fileList.contains(preCmd)) list << fileList;
                if(preCmd != "keyentry") list << "keyentry";
            }
            else
            {
                if(!titleSpec().contains(preCmd)) list << titleSpec();
                if(preCmd != "with") list << "with";
                if(!modifiers().contains(preCmd)) list << modifiers();
            }
        }
        else if(index >= 3)
        {
            if(preCmd == "with") list << d3style();
            else
            {
                if(!titleSpec().contains(preCmd)) list << titleSpec();
                if(preCmd != "with") list << "with";
                if(!modifiers().contains(preCmd)) list << modifiers();
            }
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



void GnuplotCompletionModel::setToolTip(const QString& command, const QString& firstCmd)
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
    else if(command == "fit")
    {
        if(firstCmd.isEmpty()) toolTip =  "fit {<Ranges>} <Expression> <DataFile> {datafile-modifiers} {unitweights} via <Var1>{,<Var2>,...}\n"
                                          "fit {<Ranges>} <Expression> <DataFile> {datafile-modifiers} {{x|xy|z}error} via <Var1>{,<Var2>,...}\n"
                                          "fit {<Ranges>} <Expression> <DataFile> {datafile-modifiers} {error <Var1>{,<Var2>,...}} via <Var1>{,<Var2>,...}";
        else if(firstCmd == "save") toolTip = "fit <\"FileName\">\n"
                                              "-----\nSave fitting parameters to file.";
    }
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
    else if(command == "set")
    {
        if(firstCmd.isEmpty()) toolTip =  "set <Option>";
        else if(firstCmd == "set") toolTip = "set <\"FileName\">\n"
                                             "-----\nSave setting options to file.";
    }
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

    else if(command == "message") toolTip = "message <\"Message\">\n"
                                            "-----\nSpecify a error message.";
    else if(command == "status") toolTip = "status <ExitCode>\n"
                                           "-----\nSpecify an integer error code.";
    else if(command == "unitweights")
    {
        if(firstCmd == "fit") toolTip = "-----\n This assumes that all data points have equal weights.";
    }
    else if(command == "yerror" ||
            command == "xyerror" ||
            command == "zerror" ||
            command == "error")
    {
        if(firstCmd == "fit") toolTip = "-----\nConsider the standard deviation s of the variable and use it to calculate a weight of 1/s**2 for each piece of data.";
    }
    else if(command == "errors")
    {
        if(firstCmd == "fit") toolTip = "errors <Var1>{, <Var2>, ...}\n"
                                        "-----\nSpecify which variable error the input is.";
    }
    else if(command == "via")
    {
        if(firstCmd == "fit") toolTip = "via <\"ParameterFile\">\n"
                                        "via <Var1>{, <Var2>, ...}\n"
                                        "-----\nSpecify what to refer to to optimize the parameters.";
    }
    else if(command == "mouse")
    {
        if(firstCmd == "pause") toolTip = "mouse {<EndCondition>}{, <EndCondition>} {<\"<Message>\"}\n"
                                          "-----\nWait until there is a mouse click or ctrl-C is pressed.";
    }
    else if(command == "keyentry")
    {
        if(firstCmd == "plot" ||
           firstCmd == "splot") toolTip = "-----\n"
                                          "Replacing filenames or functions with keyentries allows you to add extra lines for legend customization.";
    }
    else if(command == "axes")
    {
        if(firstCmd == "plot" ||
           firstCmd == "splot") toolTip = "axes x1y1|x2y2|x1y2|x2y1\n"
                                          "-----\nSpecify which axis to scale to.";
    }
    else if(command == "with")
    {
        if(firstCmd == "plot" ||
           firstCmd == "splot") toolTip = "with <PlotStyle> {<Option>}\n"
                                          "-----\nChoose a style for displaying functions and data.";
    }
    else if(command == "functions")
    {
        if(firstCmd == "save") toolTip = "terminal <\"FileName\">\n"
                                         "-----\nSave functions to file.";
    }
    else if(command == "variables")
    {
        if(firstCmd == "save") toolTip = "variables <\"FileName\">\n"
                                         "-----\nSave variables to file.";
    }
    else if(command == "terminal")
    {
        if(firstCmd == "save") toolTip = "terminal <\"FileName\">\n"
                                         "-----\nSave the terminal information to file.";
        else if(firstCmd == "test") toolTip = "-----\nGenerates a line type, point type, or other drawing available for the terminal in use.";
    }
    else if(command == "matrix")
    {
        if(firstCmd == "stats") toolTip = "-----\nSpecify the all matrix elements of data file to get statistics.";
    }
    else if(command == "using")
    {
        if(firstCmd == "stats") toolTip = "using N{:M}\n"
                                          "-----\nSpecify the column to get statistics.";
    }
    else if(command == "name")
    {
        if(firstCmd == "stats") toolTip = "name <\"Prefix\">\n"
                                          "-----\nYou can replace the default prefix of variables, \"STATS\" to user option name.";
    }
    else if(command == "nooutput")
    {
        if(firstCmd == "stats") toolTip = "-----\nAvoid output.";
    }
    else if(command == "output")
    {
        if(firstCmd == "stats") toolTip = "-----\nOutput to a screen or file.";
    }
    else if(command == "palette")
    {
        if(firstCmd == "test") toolTip = "-----\nDraw the state of rgb.";
    }
    else if(command == "all")
    {
        if(firstCmd == "toggle") toolTip = "-----\nActs on all valid graphs.";
    }
    else if(command == "boxerrorbars")
    {
        toolTip = "-----\n2D box errorbars. (x,y,dy)or(x,y,dy,dx)or(x,y,low,high)or(x,y,low,high,dx)";
    }
    else if(command == "boxes")
    {
        toolTip = "-----\n2D boxes. (x,y)or(x,y,x_width)";
    }
    else if(command == "boxplot")
    {
        toolTip = "-----\n2D boxplot. (x,y)or(x,y,width)or(x,y,width,count)";
    }
    else if(command == "boxxyerror")
    {
        toolTip = "-----\n2D box xy error. (x,y,dx,dy)or(x,y,xlow,xhigh,ylow,yhigh)";
    }
    else if(command == "candlesticks")
    {
        toolTip = "-----\n2D candle sticks. (date,open,low,high,close)or(x,box_min,whisker_min,whisker_high,box_high)";
    }
    else if(command == "circles")
    {
        toolTip = "-----\nDraw circle to each points. (x,y,r,start_angle,end_angle,color). Minimum columnCount is 2.";
    }
    else if(command == "ellipses")
    {
        toolTip = "-----\nDrow ellipses to each points. (x,y,major_diam, minor_diam, angle). Minimum columnCount is 2.";
    }
    else if(command == "dots")
    {
        toolTip = "-----\n2D or 3D. Draw dots to (x,y,z)";
    }
    else if(command == "filledcurves")
    {
        toolTip = "filledcurves <Option>\n"
                  "-----\n2D. (x,y)or(x,y1,y2)";
    }
    else if(command == "financebars")
    {
        toolTip = "-----\n2D. (date,open,low,high,close)or(data,open,low,high,close,color)";
    }
    else if(command == "fsteps" ||
            command == "fillsteps" ||
            command == "histeps" ||
            command == "steps")
    {
        toolTip = "-----\n2D. (x,y)";
    }
    else if(command == "impulses")
    {
        toolTip = "-----\n2D or 3D. (y)or(x,y)or(x,y,z)";
    }
    else if(command == "labels")
    {
        toolTip = "-----\n2D or 3D. (x,y,string)or(x,y,z,string)";
    }
    else if(command == "lines")
    {
        toolTip = "-----\n2D or 3D. (y)or(x,y)or(z)or(x,y,z)";
    }
    else if(command == "parallelaxes")
    {
        toolTip = "-----\nVisualize multidimensional data correlations.";
    }
    else if(command == "vectors")
    {
        toolTip = "-----\n2D or 3D.(x,y,dx,dy)or(x,y,z,dx,dy,dz)";
    }
    else if(command == "xerrorbars")
    {
        toolTip = "-----\n2D. (x,y,dx)or(x,y,low,high)";
    }
    else if(command == "xyerrorbars")
    {
        toolTip = "-----\n2D. (x,y,dx,dy)or(x,y,xlow,xhigh,ylow,yhigh)";
    }
    else if(command == "yerrorbars")
    {
        toolTip = "-----\n2D. (y,dy)or(x,y,dy)or(x,y,low,high)";
    }
    else if(command == "xerrorlines")
    {
        toolTip = "-----\n2D. (x,y,dx)or(x,y,low,high)";
    }
    else if(command == "xyerrorlines")
    {
        toolTip = "-----\n2D. (x,y,dx,dy)or(x,y,xlow,xhigh,ylow,yhigh)";
    }
    else if(command == "yerrorlines")
    {
        toolTip = "-----\n2D. (x,y,dy)or(x,y,low,high)";
    }
    else if(command == "zerrorfill")
    {
        toolTip = "-----\n3D. (x,y,z,dz)or(x,y,z,low,high)";
    }

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

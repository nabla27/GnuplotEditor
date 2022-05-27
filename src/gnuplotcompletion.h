#ifndef GNUPLOTCOMPLETION_H
#define GNUPLOTCOMPLETION_H
#include <QStringList>
#include <QObject>

namespace gnuplot_cpl
{

inline QStringList mainCmd()
{
    return QStringList() << "break"
                         << "cd"
                         << "call"
                         << "clear"
                         << "continue"
                         << "do"
                         << "evaluate"
                         << "exit"
                         << "fit"
                         << "help"
                         << "history"
                         << "if"
                         << "for"
                         << "import"
                         << "load"
                         << "lower"
                         << "pause"
                         << "plot"
                         << "print"
                         << "printerr"
                         << "pwd"
                         << "quit"
                         << "raise"
                         << "refresh"
                         << "replot"
                         << "reread"
                         << "reset"
                         << "save"
                         << "set"
                         << "show"
                         << "shell"
                         << "splot"
                         << "stats"
                         << "system"
                         << "test"
                         << "toggle"
                         << "undefine"
                         << "unset"
                         << "update"
                         << "terminal";
} //commands

class GnuplotCompletionModel : public QObject
{
    Q_OBJECT
public:
    explicit GnuplotCompletionModel(QObject *parent) : QObject(parent) {}

public slots:
    void setCompletionList(const QString& firstCmd, const QString& preCmd, const int index);

private:
    inline QStringList dirName() { return QStringList(); }
    inline QStringList fileName() { return QStringList(); }
    inline QStringList parameter() { return QStringList() << "ARG1"
                                                          << "ARG2"
                                                          << "ARG3"
                                                          << "ARG4"
                                                          << "ARG5"
                                                          << "ARG6"
                                                          << "ARG7"
                                                          << "ARG8"
                                                          << "ARG9"
                                                          << "ARGC"; }
    inline QStringList expression() { return QStringList(); }
    inline QStringList modifiers() { return QStringList() << "binary"
                                                          << "nonuniform"
                                                          << "matrix"
                                                          << "index"
                                                          << "every"
                                                          << "skip"
                                                          << "using"
                                                          << "smooth"
                                                          << "bins"
                                                          << "volatile"
                                                          << "noautoscale";}
        inline QStringList binary() { return QStringList() << "array="
                                                                 << "record="
                                                                 << "skip="
                                                                 << "format="
                                                                 << "endian="
                                                                 << "filetype="
                                                                 << "scan="
                                                                 << "transpose="
                                                                 << "dx="
                                                                 << "dy="
                                                                 << "dz="
                                                                 << "flipx"
                                                                 << "flipy"
                                                                 << "flipz"
                                                                 << "origin="
                                                                 << "center="
                                                                 << "rotate="
                                                                 << "perpendicular=";}
            inline QStringList endian() { return QStringList() << "little"
                                                                 << "big"
                                                                 << "default"
                                                                 << "swap"
                                                                 << "swab";}
            inline QStringList filetype() { return QStringList() << "avs"
                                                                       << "bin"
                                                                       << "edf"
                                                                       << "ehf"
                                                                       << "gif"
                                                                       << "gpbin"
                                                                       << "jpeg"
                                                                       << "jpg"
                                                                       << "png"
                                                                       << "raw"
                                                                       << "rgb"
                                                                       << "auto";}
        inline QStringList nonuniform() { return QStringList() << "matrix"; }
        inline QStringList smooth() { return QStringList() << "unique"
                                                                 << "frequency"
                                                                 << "fnormal"
                                                                 << "comulative"
                                                                 << "cnormal"
                                                                 << "bins"
                                                                 << "kdensity"
                                                                 << "kdensity bandwidth"
                                                                 << "csplines"
                                                                 << "acsplines"
                                                                 << "mcsplines"
                                                                 << "bezier"
                                                                 << "sbezier"
                                                                 << "unwrap";}
            inline QStringList bandwidth() { return QStringList(); }
    inline QStringList variables() { return QStringList(); }
    inline QStringList helpItems() { return QStringList(); }
    inline QStringList historyCmd() { return QStringList(); }
    inline QStringList style() { return QStringList() << "linestyle" << "ls"
                                                      << "linetype" << "lt"
                                                      << "linewidth" << "lw"
                                                      << "linecolor" << "lc"
                                                      << "pointtype" << "pt"
                                                      << "pointsize" << "ps"
                                                      << "fill" << "fs"
                                                      << "fillcolor" << "fc"
                                                      << "nohidden3d"
                                                      << "nocontours"
                                                      << "nosurface"
                                                      << "palette"
                                                      << "pointinterval" << "pi"
                                                      << "pointnumber" << "pn"
                                                      << "dashtype" << "dt";}
    inline QStringList d2style() { return QStringList() << "boxerrorbars"
                                                        << "boxes"
                                                        << "boxplot"
                                                        << "boxxyerror"
                                                        << "candlesticks"
                                                        << "circles"
                                                        << "ellipses"
                                                        << "dots"
                                                        << "filledcurces"
                                                        << "financebars"
                                                        << "fsteps"
                                                        << "fillsteps"
                                                        << "histeps"
                                                        << "histograms"
                                                        << "image"
                                                        << "impulses"
                                                        << "labels"
                                                        << "lines"
                                                        << "linespoints"
                                                        << "parallelaxes"
                                                        << "points"
                                                        << "steps"
                                                        << "surface"
                                                        << "table"
                                                        << "rgbalpha"
                                                        << "rgbimage"
                                                        << "vectors"
                                                        << "xerrorbars"
                                                        << "xyerrorbars"
                                                        << "yerrorbars"
                                                        << "xerrorlines"
                                                        << "xyerrorlines"
                                                        << "yerrorlines"
                                                        << "zerrorfill";}
    inline QStringList d3style() { return QStringList() << "points"
                                                        << "images"
                                                        << "impulse"
                                                        << "labels"
                                                        << "vector"; }
        inline QStringList filledCurves() { return QStringList() << "closed"
                                                                       << "xy="
                                                                       << "above"
                                                                       << "below"
                                                                       << "x1"
                                                                       << "x2"
                                                                       << "y"
                                                                       << "r";}
            inline QStringList ab() { return QStringList() << "above"
                                                           << "below";}
    inline QStringList axes() { return QStringList() << "x1y1" << "x1y2" << "x2y1" << "x2y2"; }
    inline QStringList titleSpec() { return QStringList() << "offset"
                                                          << "font"
                                                          << "textcolor" << "tc"
                                                          << "noenhanced"
                                                          << "enhanced";}
    inline QStringList testType() { return QStringList() << "terminal" << "palette"; }
    inline QStringList option() { return QStringList() << "angles"
                                                       << "arrow"
                                                       << "autoscale"
                                                       << "bind"
                                                       << "bmargin"
                                                       << "border"
                                                       << "boxwidth"
                                                       << "color"
                                                       << "colorsequence"
                                                       << "clabel"
                                                       << "clip"
                                                       << "cntrlabel"
                                                       << "cntrparam"
                                                       << "colorbox"
                                                       << "colornames"
                                                       << "contour"
                                                       << "dashtype"
                                                       << "data style"
                                                       << "datafile"
                                                       << "desimalsign"
                                                       << "dgrid3d"
                                                       << "dummy"
                                                       << "encoding"
                                                       << "errorbars"
                                                       << "fit"
                                                       << "fontpath"
                                                       << "format"
                                                       << "function style"
                                                       << "functions"
                                                       << "grid"
                                                       << "hidden3d"
                                                       << "historysize"
                                                       << "history"
                                                       << "isosamples"
                                                       << "jitter"
                                                       << "key"
                                                       << "label"
                                                       << "linetype"
                                                       << "link"
                                                       << "lmargin"
                                                       << "loadpath"
                                                       << "locale"
                                                       << "logscale"
                                                       << "macros"
                                                       << "mapping"
                                                       << "margin"
                                                       << "micro"
                                                       << "minussign"
                                                       << "monochrome"
                                                       << "mouse"
                                                       << "mttics"
                                                       << "multiplot"
                                                       << "mx2tics"
                                                       << "mxtics"
                                                       << "my2tics"
                                                       << "mytics"
                                                       << "mztics"
                                                       << "nonlinear"
                                                       << "object"
                                                       << "offsets"
                                                       << "origin"
                                                       << "output"
                                                       << "parametric"
                                                       << "paxis"
                                                       << "plot"
                                                       << "pm3d"
                                                       << "palette"
                                                       << "pointintervalbox"
                                                       << "pointsize"
                                                       << "polar"
                                                       << "print"
                                                       << "psdir"
                                                       << "raxis"
                                                       << "rgbmax"
                                                       << "rlabel"
                                                       << "rmargin"
                                                       << "rrange"
                                                       << "rtics"
                                                       << "samples"
                                                       << "size"
                                                       << "style"
                                                       << "surface"
                                                       << "table"
                                                       << "terminal"
                                                       << "termoption"
                                                       << "theta"
                                                       << "tics"
                                                       << "ticslevel"
                                                       << "ticscale"
                                                       << "timestamp"
                                                       << "timefmt"
                                                       << "title"
                                                       << "tmargin"
                                                       << "trange"
                                                       << "ttics"
                                                       << "urange"
                                                       << "variables"
                                                       << "version"
                                                       << "view"
                                                       << "vrange"
                                                       << "x2data"
                                                       << "x2dtics"
                                                       << "x2label"
                                                       << "x2mtics"
                                                       << "x2range"
                                                       << "x2tics"
                                                       << "x2zerozxis"
                                                       << "xdata"
                                                       << "xdtics"
                                                       << "xlabel"
                                                       << "xmtics"
                                                       << "xrange"
                                                       << "xtics"
                                                       << "xyplane"
                                                       << "xzeroaxis"
                                                       << "y2data"
                                                       << "y2dtics"
                                                       << "y2label"
                                                       << "y2mtics"
                                                       << "y2range"
                                                       << "y2tics"
                                                       << "y2zeroaxis"
                                                       << "ydata"
                                                       << "ydtics"
                                                       << "ylabel"
                                                       << "ymtics"
                                                       << "yrange"
                                                       << "ytics"
                                                       << "yzeroaxis"
                                                       << "zdata"
                                                       << "zdtics"
                                                       << "zzeroaxis"
                                                       << "cbdata"
                                                       << "cbdtics"
                                                       << "zero"
                                                       << "zeroaxis"
                                                       << "zlabel"
                                                       << "zmtics"
                                                       << "zrange"
                                                       << "ztics"
                                                       << "cblabel"
                                                       << "cbmtics"
                                                       << "cbrange"
                                                       << "cbtics";}
        inline QStringList angle() { return QStringList() << "degrees" << "radians"; }
        inline QStringList arrow() { return QStringList() << "nohead"
                                                          << "head"
                                                          << "backhead"
                                                          << "heads"
                                                          << "size"
                                                          << "fixed"
                                                          << "filled"
                                                          << "empty"
                                                          << "nofilled"
                                                          << "noborder"
                                                          << "front"
                                                          << "back"
                                                          << "linestyle" << "ls"
                                                          << "linetype" << "lt"
                                                          << "linewidth" << "lw"
                                                          << "linecolor" << "lc"
                                                          << "dashtype" << "dt";}
        inline QStringList autoscale() { return QStringList() << "noextend" << axes(); }
        inline QStringList border() { return QStringList() << "font"
                                                           << "back"
                                                           << "behind"
                                                           << "linestyle" << "ls"
                                                           << "linetype" << "lt"
                                                           << "linewidth" << "lw"
                                                           << "linecolor" << "lc"
                                                           << "dashtype" << "dt"
                                                           << "polar";}
        inline QStringList boxwidth() { return QStringList() << "absolute"
                                                             << "relative"; }
        inline QStringList colorsequence() { return QStringList() << "default"
                                                                  << "classic"
                                                                  << "podo";}
        inline QStringList clip() { return QStringList() << "points" << "one" << "two"; }
        inline QStringList cntrlabel() { return QStringList() << "format"
                                                              << "font"
                                                              << "start"
                                                              << "interval";}
        inline QStringList cntrparam() { return QStringList() << "linear"
                                                              << "cubicspline"
                                                              << "bspline"
                                                              << "points"
                                                              << "order"
                                                              << "levels"
                                                              << "sorted"
                                                              << "firstlinetype";}
        inline QStringList levels() { return QStringList() << "auto"
                                                           << "discrete"
                                                           << "incremental";}
        inline QStringList colorbox() { return QStringList() << "vertical"
                                                             << "horizontal"
                                                             << "noinvert"
                                                             << "invert"
                                                             << "default"
                                                             << "user"
                                                             << "origin x,y"
                                                             << "size x,y"
                                                             << "front"
                                                             << "back"
                                                             << "noborder"
                                                             << "bdefault"
                                                             << "border";}
        inline QStringList contour() { return QStringList() << "base"
                                                            << "surface"
                                                            << "both";}
        inline QStringList datafile() { return QStringList() << "fortran"
                                                             << "nofpe_trap"
                                                             << "missing"
                                                             << "separator"
                                                             << "commentschars"
                                                             << "binary";}
        inline QStringList desimalsign() { return QStringList() << "local"; }
        inline QStringList dgrid3d() { return QStringList() << "splines"
                                                            << "qnorm"
                                                            << "gauss"
                                                            << "cauchy"
                                                            << "exp"
                                                            << "box"
                                                            << "hann"
                                                            << "kdensity";}
        inline QStringList encoding() { return QStringList() << "locale"; }
        inline QStringList errorbars() { return QStringList() << "small"
                                                              << "large"
                                                              << "fullwidth"
                                                              << "front"
                                                              << "back"
                                                              << "line-properties";}
        inline QStringList fit() { return QStringList() << "nolog"
                                                        << "logfile"
                                                        << "noquiet" << "quiet"
                                                        << "noresults" << "results"
                                                        << "nobrief" << "brief"
                                                        << "noverbose" << "verbose"
                                                        << "noerrorvariables" << "errorvariables"
                                                        << "noconvariancevariables" << "convariancevariables"
                                                        << "noerrorscaling" << "errorscaling"
                                                        << "noprescale" << "prescale"
                                                        << "maxiter" << "default"
                                                        << "limit"
                                                        << "limit_abs"
                                                        << "start-lambda"
                                                        << "lambda-factor"
                                                        << "script"
                                                        << "v4" << "v5";}




    inline QStringList color() { return QStringList() << "grey10"
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
                                                      << "rgb"
                                                      << "bgnd"
                                                      << "default";}


signals:
    void completionListSet(const QStringList& list);
    void toolTipSet(const QString& text);
};

/* break
 * cd <"DirName">
 * call <"InputFile"> <Parameter1> <Parameter2> ... <Parameter9>
 * clear
 * continue
 * do for [<InterationSpec>] { }                                                                            //
 * eval <"StringExpression">
 * exit
 * exit message <"ErrorMessage">
 * exit status <ErrorCode>
 * fit {<Ranges>} <Expression> <DataFile> {<Modifier>} {unitweights}                via <Var1>{,<Var2>,...}  //modifier
 * fit {<Ranges>} <Expression> <DataFile> {<Modifier>} {{x|xy|z}error}              via <Var1>{,<Var2>,...}  //
 * fit {<Ranges>} <Expression> <DataFile> {<Modifier>} {error <Var1>{,<Var2>,...}}  via <Var1>{,<Var2>,...}  //
 * help <Items>
 * history
 * history {<Count>} <"OutFile"> {append}
 * history ?<"Command">
 * histroy !<"Command">
 * history !<Count>
 * if (<Condition>) { } else if (<Condition>) { else {} }
 * for [<IntVar> = <Start>:<End>{:<Increment>}]
 * for [<StringVar> in "Str1 Str2 ..."]
 * import <FuncName>(x{x,t,z,...}) from <"ObjFile{:<Symbol>}">
 * load <"FileName">
 * lower {<PlotWindowId>}
 * pause <Time> {<"String">}
 * pause mouse {<EndCondition>}{, <EndCondition>} {<"String">}
 * plot {<Ranges>} <Definition> {axes <Axes>} {<TitleSpec>} {with <Style>}
 * plot {<Ranges>} <Function> {axes <Axes>} {<TitleSpec>} {with <Style>}           //sampling-range
 * plot {<Ranges>} <DataSource> {<Modifier>} {axes <Axes>} {<TitleSpec>} {with <Style>}
 * plot {<Ranges>} keyentry {axes <Axes>} {<TitleSpec>} {with <Style>}
 * print <Formula> {, <Formula>, ...}
 * printerr <Formula> {, <Formula>, ...}
 * pwd
 * quit
 * raise {<PlotWindowId>}
 * refresh
 * replot
 * reread
 * reset
 * save {<Command> {<Command>} ...} <"FileName">
 * set <Option>
 * show <Option>
 * shell
 * splot {<Ranges>} <Function> {<TitleSpec>} {with <Style>} {, {definitions} <Function> ...}
 * splot {<Ranges>} <FileName> {<Modifier>} {<TitleSpec>} {with <Style>} {, {definitions} <Function> ...} //<Modifier>
 * splot {<Ranges>} <DataBolockName> {<Modifier>} {<TitleSpec>} {with <Style>} {, {definitions} <Function> ...}
 * splot {<Ranges>} keyentry {<TitleSpec>} {with <Style>} {, {definitions} <Function> ...}
 * stats {<Ranges>} <"FileName"> {matrix} {name <Prefix>} {{no}output}  //
 * stats {<Ranges>} <"FileName"> {using <Range>} {name <Prefix>} {{no}output}
 * system <"Command">
 * test <TestType>
 * toggle <PlotNo>
 * toggle <"Title">
 * toggle all
 * undefine <Variable>
 * undefine <StrExpression>
 * unset <Option>
 * update <"FileName"> {<"FileName">}
 * while (<Expression>) { }
 */



enum class CommandType
{
    MainCmd,
    DirName,
    InputFileName,    //存在するファイル名
    Parameter,
    InterationSpec,
    Condition,        //文字式による条件
    Message,          //文字式によるメッセージ
    Int,
    Ranges,           //[]やforによる範囲指定
    Expresstion,      //数式
    Variable,         //変数
    HelpItem,         //helpコマンドの項目
    OutFileName,      //出力ファイル名
    HistoryCommand,   //過去のコマンド
    Definition,
    Axes,
    TitleSpec,
    Style,            //with
    Option,
    DataBlockName,
    StatsType,
    TestType,
    PlotNo,
};

void GnuplotCompletionModel::setCompletionList(const QString& firstCmd, const QString& preCmd, const int index)
{


}



QString toolTipForCommand(const QString& command)
{
    if(command == "break") return "break";
    else if(command == "cd") return "cd <\"DirName\">";
    else if(command == "call") return "call <\"InputFile\"> <Parameter1> <Parameter2> ... <Parameter9>";
    else if(command == "clear") return "clear";
    else if(command == "continue") return "continue";
    else if(command == "do") return "do for <InterationSpec> { }";
    else if(command == "eval") return "eval <\"StringExpression\">";
    else if(command == "exit") return "exit \n""exit message <\"ErrorMessage\"> \n""exit status <ErrorCode>";
    else if(command == "fit") return "fit {<Ranges>} <Expression> <DataFile> {datafile-modifiers} {unitweights} via <Var1>{,<Var2>,...}\n"
                                     "fit {<Ranges>} <Expression> <DataFile> {datafile-modifiers} {{x|xy|z}error} via <Var1>{,<Var2>,...}\n"
                                     "fit {<ranges>} <Expression> <DataFile> {datafile-modifiers} {error <Var1>{,<Var2>,...}} via <Var1>{,<Var2>,...}";
    else if(command == "help") return "help <Contents>";
    else if(command == "history") return "history\n"
                                         "history {<Count>} <\"OutFile\"> {append}\n"
                                         "history ?<\"Command\">\n"
                                         "history !<\"Command\">\n"
                                         "history !<Count>";
    else if(command == "if") return "if (<Condition>) { } else if (<Condition>) { else {} }";
    else if(command == "for") return "for [<IntVar> = <Start>:<End>{:<Increment>}]\n"
                                     "for [<StringVar> in \"Str1 Str2 ...\"]";
    else if(command == "import") return "import <FuncName>(x{y,z,...}) from <\"ObjectFile\"{:<Symbol>}\">";
    else if(command == "load") return "load <\"FileName\">";
    else if(command == "lower") return "lower {<PlotWindowId>}";
    else if(command == "pause") return "pause <Time> {<\"Message\">}";
    else if(command == "pause") return "pause mouse {<EndCondition>}{, <EndCondition>...} {<\"Message\">}";
    else if(command == "plot") return "plot {<Ranges>} <Definition> {ases <Axes>} {<TitleSpec>} {with <Style>}\n"
                                      "plot {<Ranges>} {sampling-range} <Function> {axes <Axes>} {<TitleSpec>} {with <Style>}\n"
                                      "plot {<Ranges>} <DataSource> {<Modifier>} {axes <Axes>} {<TitleSpec>} {with <Style>}\n"
                                      "plot {<Ranges>} keyentry {axes <Axes>} {<TitleSpec>} {with <Style>}";
    else if(command == "print") return "print <Formula> {, <Formula>, ...}";
    else if(command == "printerr") return "print <Formula> {, <Formula>, ...}";
    else if(command == "pwd") return "pwd";
    else if(command == "quit") return "quit";
    else if(command == "raise") return "raise {<PlotWindowId>}";
    else if(command == "refresh") return "refresh";
    else if(command == "replot") return "replot";
    else if(command == "reread") return "reread";
    else if(command == "reset") return "reset";
    else if(command == "save") return "save {<SaveCmd> {<SaveCmd>,}...} <\"FileName\">";
    else if(command == "set") return "set <Option>";
    else if(command == "show") return "show <Option>";
    else if(command == "shell") return "shell";
    else if(command == "splot") return "splot {<Ranges>} <Function> {<TitleSpec>} {with <Style>} {, {definitions} <Function> ...}\n"
                                       "splot {<Ranges>} <FileName> {datafile-modifiers} {<TitleSpec>} {with <Style>} {, {definitions} <Function> ...}\n"
                                       "splot {<Ranges>} <DataBolockName> {<Modifier>} {<TitleSpec>} {with <Style>} {, {definitions} <Function> ...}\n"
                                       "splot {<Ranges>} keyentry {<TitleSpec>} {with <Style>} {, {definitions} <Function> ...}";
    else if(command == "stats") return "stats {<Ranges>} <\"FileName\"> {<StatsType>} {name <Prefix>} {{no}output}";
    else if(command == "system") return "system <\"Command\">";
    else if(command == "test") return "test <TestType>";
    else if(command == "toggle") return "toggle <PlotNo>\n"
                                        "toggle <\"Title\">\n"
                                        "toggle all";
    else if(command == "undefine") return "undefine <Variable>\n"
                                          "undefine <StrExpression>";
    else if(command == "unset") return "unset <Option>";
    else if(command == "update") return "update <\"FileName\"> {<\"FileName\">}";
    else if(command == "while") return "while (<Expression>) {}";
    else return QString();
}












}
#endif // GNUPLOTCOMPLETION_H

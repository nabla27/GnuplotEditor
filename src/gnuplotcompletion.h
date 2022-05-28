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
                                                        << "vector"
                                                        << "pm3d"
                                                        << "coloring"
                                                        << "surfaces"
                                                        << "contours";}
        inline QStringList filledcurves() { return QStringList() << "closed"
                                                                       << "xy="
                                                                       << "above"
                                                                       << "below"
                                                                       << "x1"
                                                                       << "x2"
                                                                       << "y"
                                                                       << "r";}
            inline QStringList closed() { return QStringList() << "above"
                                                               << "below";}
    inline QStringList axes() { return QStringList() << "x1y1" << "x1y2" << "x2y1" << "x2y2"; }
    inline QStringList titleSpec() { return QStringList() << "offset"
                                                          << "font"
                                                          << "textcolor" << "tc"
                                                          << "noenhanced"
                                                          << "enhanced";}
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
                                                       << "margins"
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
        inline QStringList format() { return QStringList() << "x"
                                                           << "y"
                                                           << "xy"
                                                           << "x2"
                                                           << "y2"
                                                           << "z"
                                                           << "cb";}
        inline QStringList grid() { return QStringList() << "nomxtics"
                                                         << "nomytics"
                                                         << "nomztics"
                                                         << "nomx2tics"
                                                         << "nomy2tics"
                                                         << "nomrtics"
                                                         << "nomcbtics"
                                                         << "polar"
                                                         << "layerdefault"
                                                         << "front"
                                                         << "back"
                                                         << "novertical"
                                                         << "mxtics"
                                                         << "mytics"
                                                         << "mztics"
                                                         << "mx2tics"
                                                         << "my2tics"
                                                         << "mrtics"
                                                         << "mcbtics"
                                                         << "noxtics"
                                                         << "noytics"
                                                         << "noztics"
                                                         << "nox2tics"
                                                         << "noy2tics"
                                                         << "nortics"
                                                         << "nocbtics"
                                                         << "vertical";}
        inline QStringList hidden3d() { return QStringList() << "default"
                                                             << "front"
                                                             << "back"
                                                             << "offset"
                                                             << "nooffset"
                                                             << "trianglepattern"
                                                             << "undefined"
                                                             << "noundefined"
                                                             << "noaltdiagonal"
                                                             << "altdiagonal"
                                                             << "nobentover"
                                                             << "bentover";}
        inline QStringList history() { return QStringList() << "size"
                                                            << "quiet"
                                                            << "numbers"
                                                            << "full"
                                                            << "trim"
                                                            << "default";}
        inline QStringList jitter() { return QStringList() << "overlap"
                                                           << "spread"
                                                           << "wrap"
                                                           << "swarm"
                                                           << "square"
                                                           << "vertical";}
        inline QStringList key_1() { return QStringList() << "on"
                                                        << "off"
                                                        << "default";}
        inline QStringList key_2() { return QStringList () << "inside"
                                                           << "outside"
                                                           << "fixed"
                                                           << "lmargin"
                                                           << "rmargin"
                                                           << "tmargin"
                                                           << "bmargin"
                                                           << "at"
                                                           << "left"
                                                           << "right"
                                                           << "center"
                                                           << "top"
                                                           << "bottom"
                                                           << "center"
                                                           << "vertical"
                                                           << "horizontal"
                                                           << "Left"
                                                           << "Right"
                                                           << "noenhanced" << "enhanced"
                                                           << "noopaque" << "opaque"
                                                           << "noreverse" << "reverse"
                                                           << "noinvert" << "invert"
                                                           << "samplen"
                                                           << "spacing"
                                                           << "width"
                                                           << "height"
                                                           << "noautotitle"
                                                           << "autotitle"
                                                           << "title"
                                                           << "font"
                                                           << "textcolor"
                                                           << "nobox" << "box"
                                                           << "maxcols"
                                                           << "maxrows";}
            inline QStringList key_title() { return QStringList() << "noenhanced" << "enhanced"
                                                              << "center"
                                                              << "left"
                                                              << "right";}
            inline QStringList key_box() { return QStringList() << "linestyle"
                                                                << "linetype"
                                                                << "linewidth";}
            inline QStringList key_maxrc() { return QStringList() << "auto"; }
        inline QStringList label() { return QStringList() << "at"
                                                              << "left"
                                                              << "center"
                                                              << "right"
                                                              << "norotate" << "rotate"
                                                              << "noenhanced"
                                                              << "front"
                                                              << "back"
                                                              << "textcolor"
                                                              << "point" << "nopoint"
                                                              << "offset"
                                                              << "boxed"
                                                              << "hypertext";}
        inline QStringList link() { return QStringList() << "x2"
                                                         << "y2"
                                                         << "via   inverse";}
        inline QStringList logscale() { return QStringList() << "x"
                                                             << "x2"
                                                             << "y"
                                                             << "y2"
                                                             << "z"
                                                             << "cb"
                                                             << "r";}
        inline QStringList mapping() { return QStringList() << "cartesian"
                                                            << "spherical"
                                                            << "cylindrical";}
        inline QStringList margin() { return QStringList() << "at screen";}
        inline QStringList monochrome() { return QStringList() << "linetype";}
        inline QStringList mouse() { return QStringList() << "doubleclick" << "nodoubleclick"
                                                          << "nozoomcoordinates" << "zoomcoordinates"
                                                          << "zoomfactors"
                                                          << "noruler"
                                                          << "polardistance" << "polardistancedeg" << "polardistancetan" << "nopolardistance"
                                                          << "format"
                                                          << "mouseformat"
                                                          << "nolabels" << "labels"
                                                          << "nozoomjump" << "zoomjump"
                                                          << "noverbose" << "verbose";}
            inline QStringList mouse_ruler() { return QStringList() << "at"; }
        inline QStringList multiplot() { return QStringList() << "title"
                                                              << "layout";}
            inline QStringList multiplot_layout() const { return QStringList() << "rowsfirst"
                                                                           << "columnsfirst"
                                                                           << "downwards"
                                                                           << "upwards"
                                                                           << "scale"
                                                                           << "margins"
                                                                           << "spacing";}
        inline QStringList mxtics() const { return QStringList() << "default"; }
        inline QStringList nolinear() const { return QStringList() << "x2 via   inverse"
                                                                   << "y2 via   inverse";}
        inline QStringList object() const { return QStringList() << "rectangle"
                                                                 << "ellipse"
                                                                 << "polygon"
                                                                 << "front" << "back" << "behind"
                                                                 << "clip" << "noclip"
                                                                 << "fc" << "fillcolor"
                                                                 << "fs"
                                                                 << "default"
                                                                 << "lw" << "linewidth"
                                                                 << "dt" << "dashtype"
                                                                 << "from  to";}
        inline QStringList paxis() const { return QStringList() << "range"
                                                                << "tics"
                                                                << "range"
                                                                << "tics";}
        inline QStringList pm3d() const { return QStringList() << "at"
                                                               << "interpolate"
                                                               << "scansautomatic"
                                                               << "scansforward"
                                                               << "scansbackward"
                                                               << "depthorder"
                                                               << "flush"
                                                               << "ftriangles" << "noftriangles"
                                                               << "clip1in" << "clip4in"
                                                               << "noclipcb" << "clipcb"
                                                               << "corners2color";}
            inline QStringList pm3d_flush() const { return QStringList() << "begin"
                                                                     << "center"
                                                                     << "end";}
            inline QStringList pm3d_corners2color() const { return QStringList() << "mean"
                                                                             << "geomean"
                                                                             << "harmean"
                                                                             << "rms"
                                                                             << "median"
                                                                             << "min"
                                                                             << "max"
                                                                             << "c1"
                                                                             << "c2"
                                                                             << "c3"
                                                                             << "c4";}
        inline QStringList palette() const { return QStringList() << "gray" << "color"
                                                                  << "gamma"
                                                                  << "rgbformulae ,,"
                                                                  << "cubehelix"
                                                                  << "model"
                                                                  << "positive" << "negative"
                                                                  << "nops_allcF" << "ps_allcF"
                                                                  << "maxcolors"
                                                                  << "palette"
                                                                  << "gradient"
                                                                  << "fit2rgbformulae"
                                                                  << "rgbformulae";}
            inline QStringList palette_rgbformulae() const { return QStringList() << "defined"
                                                                              << "file"
                                                                              << "functions ,,";}
            inline QStringList palette_cubehelix() const { return QStringList() << "start"
                                                                                << "cycles"
                                                                                << "saturation";}
            inline QStringList palette_model() const { return QStringList() << "RGB"
                                                                            << "HSV"
                                                                            << "CMY"
                                                                            << "XYZ";}
        inline QStringList rgbmax() const { return QStringList() << "1.0" << "255"; }
        inline QStringList size() const { return QStringList() << "nosquare" << "square"
                                                               << "ratio" << "noratio";}
        inline QStringList ostyle() const { return QStringList() << "function"
                                                                 << "data"
                                                                 << "arrow"
                                                                 << "boxplot"
                                                                 << "circle"
                                                                 << "ellipse size"
                                                                 << "fill"
                                                                 << "histogram"
                                                                 << "line"
                                                                 << "rectangle"
                                                                 << "textbox";}
            inline QStringList ostyle_units() const { return QStringList() << "xy" << "xx" << "yy"
                                                                           << "clip" << "noclip";}
            inline QStringList ostyle_textbox() const { return QStringList() << "opaque"
                                                                             << "transparent"
                                                                             << "noborder" << "border"
                                                                             << "fillcolor";}
        inline QStringList surface() const { return QStringList() << "implicit" << "explicit"; }
        inline QStringList table() const { return QStringList() << "append" << "separator"; }
            inline QStringList table_separator() const { return QStringList() << "whitespace"
                                                                              << "tab"
                                                                              << "comma";}
        inline QStringList termoption() const { return QStringList() << "noenhanced" << "enhanced"
                                                                     << "font"
                                                                     << "fontscale"
                                                                     << "linewidth";}
        inline QStringList theta() const { return QStringList() << "right"
                                                                << "top"
                                                                << "left"
                                                                << "bottom"
                                                                << "clockwise"
                                                                << "cw"
                                                                << "counterclockwise"
                                                                << "ccw";}
        inline QStringList tics() const { return QStringList() << "axis"
                                                               << "border"
                                                               << "nomirror" << "mirror"
                                                               << "in" << "out"
                                                               << "front" << "back"
                                                               << "norotate" << "rotate"
                                                               << "offset" << "nooffset"
                                                               << "left" << "right" << "center" << "autojustify"
                                                               << "format"
                                                               << "font"
                                                               << "noenhanced" << "enhanced"
                                                               << "textcolor"
                                                               << "scale";}
            inline QStringList tics_rotate() const { return QStringList() << "by"; }
            inline QStringList tics_scale() const { return QStringList() << " default"; }
        inline QStringList timestamp() const { return QStringList() << "top"
                                                                    << "bottom"
                                                                    << "norotate" << "rotate"
                                                                    << "offset"
                                                                    << "font"
                                                                    << "textcolor";}
        inline QStringList title() const { return QStringList() << "offset"
                                                                << "font"
                                                                << "textcolor" << "tc"
                                                                << "default"
                                                                << "noenhanced" << "enhanced"; }
        inline QStringList view() const { return QStringList() << "map"
                                                               << "noequal" << "equal"
                                                               << "azimuth";}
            inline QStringList view_map() const { return QStringList() << "scale";}
            inline QStringList view_equal() const { return QStringList() << "xy" << "xyz"; }
        inline QStringList xlabel() const { return QStringList() << "offset"
                                                                 << "font"
                                                                 << "textcolor"
                                                                 << "noenhanced" << "enhanced"
                                                                 << "rotate by"
                                                                 << "rotate parallel"
                                                                 << "norotate";}
        inline QStringList xrange() const { return QStringList() << "noreverse" << "reverse"
                                                                 << "nowriteback" << "writeback"
                                                                 << "noextend" << "extend"
                                                                 << "restore";}
        inline QStringList xtics() const { return QStringList() << "axis" << "border"
                                                                << "nomirror" << "mirror"
                                                                << "in" << "out"
                                                                << "scale"
                                                                << "norotate" << "rotate"
                                                                << "offset" << "nooffset"
                                                                << "left" << "right" << "center" << "autojustify"
                                                                << "add"
                                                                << "autofreq"
                                                                << "format"
                                                                << "font"
                                                                << "noenhanced" << "enhanced"
                                                                << "numeric" << "timedata" << "geographic"
                                                                << "nologscale" << "logscale"
                                                                << "rangelimited"
                                                                << "textcolor";}
        inline QStringList xyplane() const { return QStringList() << "at"
                                                                  << "relative";}
    inline QStringList terminal() const { return QStringList() <<"caca"
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
                                                               <<"pop"; }
        inline QStringList windows() const { return QStringList() << "color" << "monochrome"
                                                                  << "solid" << "dashed"
                                                                  << "rounded" << "butt"
                                                                  << "enhanced" << "noenhanced"
                                                                  << "font"
                                                                  << "fontscale"
                                                                  << "linewidth"
                                                                  << "pointscale"
                                                                  << "background"
                                                                  << "title"
                                                                  << "size" << "wsize"
                                                                  << "position"
                                                                  << "docked"
                                                                  << "layout"
                                                                  << "standalone"
                                                                  << "close"; }
        inline QStringList canvas() const { return QStringList() << "size"
                                                                 << "background"
                                                                 << "font"
                                                                 << "fsize"
                                                                 << "noenhanced" << "enhanced"
                                                                 << "linewidth"
                                                                 << "rounded" << "butt" << "square"
                                                                 << "dashlength"
                                                                 << "standalone" << "standalone mousing"
                                                                 << "name"
                                                                 << "jsdir"
                                                                 << "title";}
        inline QStringList cgm() const { return QStringList() << "color" << "monochrome"
                                                              << "colid" << "dashed"
                                                              << "norotate" << "rotate"
                                                              << "width"
                                                              << "linewidth"
                                                              << "font"
                                                              << "background"
                                                              << "landscape"
                                                              << "portrait"
                                                              << "default";}
        inline QStringList dumb() const { return QStringList() << "size"
                                                               << "nofeed" << "feed"
                                                               << "aspect"
                                                               << "noenhanced" << "enhanced"
                                                               << "mono" << "ansi" << "ansi256" << "ansirgb";}
        inline QStringList caca() const { return QStringList() << "driver"
                                                               << "format"
                                                               << "default"
                                                               << "list"
                                                               << "color"
                                                               << "monochrome"
                                                               << "noinverted" << "inverted"
                                                               << "enhanced" << "noenhanced"
                                                               << "background"
                                                               << "title"
                                                               << "size"
                                                               << "charset";}
            inline QStringList caca_charset() const { return QStringList() << "ascii"
                                                                           << "blocks"
                                                                           << "unicode";}
        inline QStringList emf() const { return QStringList() << "color" << "monochrome"
                                                              << "enhanced" << "enhanced noproportional"
                                                              << "rounded" << "butt"
                                                              << "linewidth"
                                                              << "dashlength"
                                                              << "size"
                                                              << "background"
                                                              << "font"
                                                              << "fontscale";}
        inline QStringList fig() const { return QStringList() << "monochrome" << "color"
                                                              << "small" << "big" << "size"
                                                              << "landscape" << "portrait"
                                                              << "font"
                                                              << "fontsize"
                                                              << "textnormal"
                                                              << "textspecial"
                                                              << "texthidden"
                                                              << "textrigid"
                                                              << "linewidth" << "lw"
                                                              << "multiplier";}
        inline QStringList hpgl() const { return QStringList() << "eject"; }
        inline QStringList png() const { return QStringList() << "noenhanced" << "enhanced"
                                                              << "notransparent" << "transparent"
                                                              << "nointerlace" << "interlace"
                                                              << "notruecolor" << "truecolor"
                                                              << "rounded" << "butt"
                                                              << "linewidth" << "dashlength"
                                                              << "tiny" << "small" << "medium" << "large" << "giant"
                                                              << "font"
                                                              << "fontscale"
                                                              << "size"
                                                              << "nocrop" << "crop"
                                                              << "background";}
        inline QStringList jpeg() const { return QStringList() << "noenhanced" << "enhanced"
                                                               << "nointerlace" << "interlace"
                                                               << "linewidth" << "dashlength"
                                                               << "rounded" << "butt"
                                                               << "tiny" << "small" << "large" << "giant"
                                                               << "font"
                                                               << "fontscale"
                                                               << "size"
                                                               << "nocrop" << "crop"
                                                               << "bakcround";}
        inline QStringList gif() const { return QStringList() << "noenhanced" << "enhanced"
                                                              << "notransparent" << "transparent"
                                                              << "rounded" << "butt"
                                                              << "linewidth"
                                                              << "dashlength"
                                                              << "tiny" << "small" << "medium" << "large" << "giant"
                                                              << "font"
                                                              << "fontscale"
                                                              << "size"
                                                              << "nocrop" << "crop"
                                                              << "animate" << "animate delay"
                                                              << "loop"
                                                              << "nooptimize" << "optimize"
                                                              << "background";}
        inline QStringList sixelgd() const { return QStringList() << "noenhanced" << "enhanced"
                                                                  << "notransparent" << "transparent"
                                                                  << "rounded" << "butt"
                                                                  << "linewidth"
                                                                  << "dashlength"
                                                                  << "tiny" << "small" << "medium" << "large" << "giant"
                                                                  << "font" << "fontscale"
                                                                  << "size"
                                                                  << "nocrop" << "crop"
                                                                  << "animate"
                                                                  << "background";}
        inline QStringList epslatex() const { return QStringList() << "default"
                                                                   << "standalone" << "input"
                                                                   << "oldstyle" << "newstyle"
                                                                   << "level1" << "leveldefault" << "level3"
                                                                   << "color" << "colour" << "monochrome"
                                                                   << "background" << "nobackground"
                                                                   << "dashlength" << "dl"
                                                                   << "linewidth" << "lw"
                                                                   << "pointscale" << "ps"
                                                                   << "rounded" << "butt"
                                                                   << "clip" << "noclip"
                                                                   << "palfunsparam"
                                                                   << "size"
                                                                   << "header" << "noheader"
                                                                   << "blacktext" << "colortext" << "colourtext"
                                                                   << "font"
                                                                   << "fontscale";}
         inline QStringList pslatex() const { return QStringList() << "default"
                                                                  << "rotate" << "norotate"
                                                                  << "oldstyle" << "newstyle"
                                                                  << "auxfile" << "noauxfile"
                                                                  << "level1" << "leveldefault" << "level3"
                                                                  << "color" << "colour" << "monochrome"
                                                                  << "background" << "nobackground"
                                                                  << "dashlength" << "dl"
                                                                  << "linewidth" << "lw"
                                                                  << "pointscale" << "ps"
                                                                  << "rounded" <<" butt"
                                                                  << "clip" << "noclip"
                                                                  << "palfuncparam"
                                                                  << "size";}
        inline QStringList postscript() const { return QStringList() << "default"
                                                                     << "landscape" << "portrait" << "eps"
                                                                     << "enhanced" << "noenhanced"
                                                                     << "defaultplex" << "simplex" << "duplex"
                                                                     << "fontfile add" << "fontfile delete"
                                                                     << "level1" << "leveldefault" << "level3"
                                                                     << "color" << "colour" << "monochrome"
                                                                     << "background" << "nobackground"
                                                                     << "dashlength" << "dl"
                                                                     << "linewidth" << "lw"
                                                                     << "pointscale" << "ps"
                                                                     << "rounded" <<" butt"
                                                                     << "clip" << "noclip"
                                                                     << "palfuncparam"
                                                                     << "size"
                                                                     << "blacktext" << "colortext" << "colourtext"
                                                                     << "font"
                                                                     << "fontscale";}
        inline QStringList svg() const { return QStringList() << "size"
                                                              << "mouse"
                                                              << "standalone" << "jsdir"
                                                              << "name"
                                                              << "font"
                                                              << "noenhanced" << "enhanced"
                                                              << "fontscale"
                                                              << "rounded" << "butt" << "square"
                                                              << "solid" << "dashed"
                                                              << "linewidth"
                                                              << "background";}
        inline QStringList tgif() const { return QStringList() << "portrait" << "landscape" << "default"
                                                               << "monochrome" << "color"
                                                               << "linewidth" << "lw"
                                                               << "solid" << "dashed"
                                                               << "font";}
        inline QStringList tkcanvas() const { return QStringList() << "tcl" << "perl" << "perltkx" << "python" << "ruby" << "rexx"
                                                                   << "standalone" << "input"
                                                                   << "interactive"
                                                                   << "rounded" << "butt"
                                                                   << "nobackground" <<" background"
                                                                   << "norottext" << "rottext"
                                                                   << "size"
                                                                   << "noenhanced" << "enhanced"
                                                                   << "externalimages" <<" pixels"; }
         inline QStringList latex() const { return QStringList() << "default"
                                                                << "courier" << "roman"
                                                                << "size"
                                                                << "rotate"
                                                                << "norotate";}
        inline QStringList eepic() const { return QStringList() << "default"
                                                                << "color" << "dashed"
                                                                << "rotate"
                                                                << "size"
                                                                << "small" << "tiny";}
        inline QStringList pstricks() const { return QStringList() << "hacktext" << "nohacktext"
                                                                   << "unit" << "nounit";}
        inline QStringList mp() const { return QStringList() << "color" << "colour" << "monochrome"
                                                             << "solid" << "dashed"
                                                             << "notex" << "tex" << "latex"
                                                             << "magnification"
                                                             << "psnfss" << "psnfss-version7" << "nopsnfss"
                                                             << "prologues"
                                                             << "a4paper"
                                                             << "amstex";}
        inline QStringList context() const { return QStringList() << "default"
                                                                  << "defaultsize" <<" size"
                                                                  << "input" << "standalone"
                                                                  << "timestamp" << "notimestamp"
                                                                  << "noheader" << "header"
                                                                  << "color" << "colour" << "monochrome"
                                                                  << "rounded" << "mitered" << "beveled"
                                                                  << "round" <<" butt" << "squared"
                                                                  << "dashed" << "solid"
                                                                  << "dashlength" << "dl"
                                                                  << "linewidth" << "lw"
                                                                  << "fontscale"
                                                                  << "mppoints" << "texpoints"
                                                                  << "inlineimages" << "externalimages"
                                                                  << "defaultfont" << "font";}
        inline QStringList wxt() const { return QStringList() << "size" << "position"
                                                              << "background"
                                                              << "noenhanced" << "enhanced"
                                                              << "font" << "fontscale"
                                                              << "title"
                                                              << "linewidth"
                                                              << "dashlength"
                                                              << "nopersist" << "persist"
                                                              << "noraise" << "raise"
                                                              << "noctrl" << "ctrl"
                                                              << "close";}
        inline QStringList cairolatex() const { return QStringList() << "eps" << "pdf"
                                                                     << "standalone" << "input"
                                                                     << "blacktext" << "colortext" << "colourtext"
                                                                     << "header" << "noheader"
                                                                     << "mono" << "color"
                                                                     << "notransparent" << "transparent"
                                                                     << "nocrop" << "crop"
                                                                     << "background"
                                                                     << "font"
                                                                     << "fontscale"
                                                                     << "linewidth"
                                                                     << "rounded" <<" butt" << "square"
                                                                     << "dashlength"
                                                                     << "size";}
        inline QStringList pdfcairo() const { return QStringList() << "noenhanced" << "enhanced"
                                                                   << "mono" <<" color"
                                                                   << "font"
                                                                   << "fontscale"
                                                                   << "linewidth"
                                                                   << "rounded" << "butt" << "square"
                                                                   << "dashlength"
                                                                   << "background"
                                                                   << "size";}
        inline QStringList pngcairo() const { return QStringList() << "noenhanced" << "enhanced"
                                                                   << "mono" << "color"
                                                                   << "notransparent" << "transparent"
                                                                   << "background"
                                                                   << "font"
                                                                   << "fontscale"
                                                                   << "linewidth"
                                                                   << "rounded" << "butt" << "square"
                                                                   << "dashlength"
                                                                   << "pointscale"
                                                                   << "size";}
        inline QStringList qt() const { return QStringList() << "size"
                                                             << "position"
                                                             << "title"
                                                             << "font"
                                                             << "noenhanced" << "enhanced"
                                                             << "linewidth"
                                                             << "dashlength"
                                                             << "nopersist" << "persist"
                                                             << "noraise" << "raise"
                                                             << "noctrl" << "ctrl"
                                                             << "close"
                                                             << "widget";}


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
    QStringList list;

    if(firstCmd == "cd")
    {
        if(index == 1) list << dirName();
    }
    else if(firstCmd == "call")
    {
        if(index == 1) list << fileName();
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
        else if(index == 2) list << expression() << fileName();
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
        if(index == 1) list << fileName();
    }
    else if(firstCmd == "pause")
    {
        if(index == 1) list << "mouse";
    }
    else if(firstCmd == "plot")
    {
        if(index == 1) list << expression() << fileName() << "keyentry";
        else if(index == 2) list << expression() << fileName() << "keyentry" << titleSpec() << modifiers() << "matrix" << "name" << "nooutput" << "output" << "using" << "name";
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
        if(index == 1) list << expression() << fileName() << "keyentry";
        else if(index == 2) list << expression() << fileName() << "keyentry" << titleSpec() << modifiers() << "matrix" << "name" << "nooutput" << "output" << "using" << "with";
        else if(index >= 3)
        {
            if(preCmd == "with") list << d3style();
            else list << titleSpec() << "with" << modifiers() << "matrix" << "name" << "nooutput" << "output" << "using" << style();
        }
    }
    else if(firstCmd == "stats")
    {
        if(index == 1) list << fileName() << "nooutput" << "output";
        else if(index == 2) list << fileName() << "matrix" << "using" << "name";
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
        list << fileName();
    }
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

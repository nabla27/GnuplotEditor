/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#include "pdfviewer.h"

#include <QPdfDocument>
#include <QtPdfWidgets/QPdfView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QFileSystemWatcher>
#include <QSpinBox>
#include <QLabel>
#include <QPdfPageNavigator>

#include "logger.h"

//DEBUG
#include <QDebug>



PdfViewer::PdfViewer(QWidget *parent)
    : QWidget(parent)
    , view(new QPdfView(this))
    , document(new QPdfDocument(this))

    , timer(new QTimer(this))
    , fileWatcher(new QFileSystemWatcher(this))

    , pageSpinBox(new QSpinBox(this))
    , zoomSpinBox(new QDoubleSpinBox(this))
{
    view->setDocument(document);
    view->setPageMode(QPdfView::PageMode::MultiPage);
    timer->setInterval(500);
    timer->setSingleShot(true);

    connect(fileWatcher, &QFileSystemWatcher::fileChanged, timer, QOverload<void>::of(&QTimer::start));
    connect(timer, &QTimer::timeout, this, &PdfViewer::reload);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QHBoxLayout *hLayout = new QHBoxLayout;
    QLabel *pageLabel = new QLabel("Page", this);
    QLabel *zoomLabel = new QLabel("Zoom", this);

    setLayout(vLayout);
    vLayout->addLayout(hLayout);
    vLayout->addWidget(view);
    hLayout->addWidget(pageLabel);
    hLayout->addWidget(pageSpinBox);
    hLayout->addWidget(zoomLabel);
    hLayout->addWidget(zoomSpinBox);
    hLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

    vLayout->setSpacing(0);
    hLayout->setSpacing(5);
    vLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setContentsMargins(5, 0, 5, 0);

    pageSpinBox->setAutoFillBackground(true);
    zoomSpinBox->setMaximum(4);
    zoomSpinBox->setSingleStep(0.1);

    //edocumentはcloseDocument()で変更されるため，ここでconnectしない
    connect(pageSpinBox, &QSpinBox::valueChanged, this, &PdfViewer::setPdfPage);
    connect(view->pageNavigator(), &QPdfPageNavigator::currentPageChanged, pageSpinBox, &QSpinBox::setValue);
    connect(zoomSpinBox, &QDoubleSpinBox::valueChanged, view, &QPdfView::setZoomFactor);
    connect(view->pageNavigator(), &QPdfPageNavigator::currentZoomChanged, zoomSpinBox, &QDoubleSpinBox::setValue);
}

void PdfViewer::setFilePath(const QString &path)
{
    this->filePath = path;

    if(!fileWatcher->files().isEmpty())
        fileWatcher->removePaths(fileWatcher->files());

    fileWatcher->addPath(path);

    reload();

    pageSpinBox->setMaximum(document->pageCount() - 1);
    pageSpinBox->setValue(view->pageNavigator()->currentPage());
    zoomSpinBox->setValue(view->zoomFactor());
}

void PdfViewer::closeDocument()
{
    /* QPdfDocument::close()を実行してもファイルはopenされたままで，ファイルの削除などが行えない．
     * 新しくQPdfDocumentをセットする．
     * おそらくqpdfdocument.cppのQPdfDocumentPrivate::load()内のQIODevice::reset()が行われる必要がある．
     * ../Qt/6.4.1/Src/qtwebengine/src/pdf/qpdfdocument.cpp
     */
    document->close();
    document->deleteLater();
    document = new QPdfDocument(this);
    view->setDocument(document);
}

void PdfViewer::reload()
{
    /* gnuplot生成でファイルが変更された場合の一回目のloadでは失敗する．
     * gnuplotがファイルを開いているのことが原因かもしれない */
    const QPdfDocument::Error err = document->load(filePath);

    if(err != QPdfDocument::Error::None)
    {
        __LOGOUT__("loading error caused. \"" + filePath + "\".", Logger::LogLevel::Error);
    }
}

void PdfViewer::setPdfPage(const int page)
{
    view->pageNavigator()->jump(page, view->pageNavigator()->currentLocation());
}


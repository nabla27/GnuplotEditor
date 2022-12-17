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



PdfViewer::PdfViewer(QWidget *parent)
    : QWidget(parent)
    , view(new QPdfView(this))
    , document(new QPdfDocument(this))

    , timer(new QTimer(this))
    , fileWatcher(new QFileSystemWatcher(this))
{
    view->setDocument(document);
    view->setPageMode(QPdfView::PageMode::MultiPage);
    timer->setInterval(1000);
    timer->setSingleShot(true);

    connect(fileWatcher, &QFileSystemWatcher::fileChanged, timer, QOverload<void>::of(&QTimer::start));
    connect(timer, &QTimer::timeout, this, &PdfViewer::reload);

    QVBoxLayout *vLayout = new QVBoxLayout(this);

    setLayout(vLayout);
    vLayout->addWidget(view);

    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);
}

void PdfViewer::setFilePath(const QString &path)
{
    this->filePath = path;
    reload();
}

void PdfViewer::reload()
{
    document->load(filePath);
}


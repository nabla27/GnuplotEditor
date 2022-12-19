/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef PDFVIEWER_H
#define PDFVIEWER_H

#include <QWidget>


class QPdfDocument;
class QPdfView;
class QTimer;
class QFileSystemWatcher;
class QSpinBox;
class QDoubleSpinBox;


class PdfViewer : public QWidget
{
public:
    explicit PdfViewer(QWidget *parent);

public slots:
    void setFilePath(const QString& path);
    void closeDocument();
    void reload();

private slots:
    void setPdfPage(const int page);

private:
    QString filePath;

    QPdfView *view;
    QPdfDocument *document;

    QTimer *timer;
    QFileSystemWatcher *fileWatcher;

    QSpinBox *pageSpinBox;
    QDoubleSpinBox *zoomSpinBox;
};

#endif // PDFVIEWER_H

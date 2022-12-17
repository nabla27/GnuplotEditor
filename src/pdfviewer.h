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


class PdfViewer : public QWidget
{
public:
    explicit PdfViewer(QWidget *parent);

public slots:
    void setFilePath(const QString& path);

private slots:
    void reload();

private:
    QString filePath;

    QPdfView *view;
    QPdfDocument *document;

    QTimer *timer;
    QFileSystemWatcher *fileWatcher;
};

#endif // PDFVIEWER_H

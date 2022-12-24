/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include <QObject>


class QAbstractItemModel;


class EditorManager : public QObject
{
    Q_OBJECT

public:
    explicit EditorManager(QObject *parent);

public slots:
    void requestToolTip(const QString& text);
    void requestModel(const QString& text);

private:


signals:
    void modelSet(QAbstractItemModel *model);
    void toolTipSet(const QString& toolTip);
};

#endif // EDITORMANAGER_H

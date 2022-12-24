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
#include <QSet>


class QAbstractItemModel;
namespace editorplugin { class EditorManager; }



class EditorManager : public QObject
{
    Q_OBJECT

public:
    explicit EditorManager(QObject *parent);
    ~EditorManager();

public slots:
    void requestToolTip(const QString& text);
    void requestModel(const QString& text);

private:
    QHash<int, editorplugin::EditorManager*> managers;

signals:
    void modelSet(QAbstractItemModel *model);
    void toolTipSet(const QString& toolTip);
};

#endif // EDITORMANAGER_H

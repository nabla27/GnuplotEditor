#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>


class UpdateManager : public QDialog
{
    Q_OBJECT
public:
    explicit UpdateManager(QWidget *parent);
    ~UpdateManager();

    void startRequest(const QUrl& requestedUrl);

private slots:


private:
    static const QString downloadUrl;
    QLineEdit *urlLineEdit;
    QLineEdit *versionLineEdit;
    QLabel *messageLabel;
};

#endif // UPDATEMANAGER_H

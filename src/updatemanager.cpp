#include "updatemanager.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

const QString UpdateManager::downloadUrl = "https://github.com/nabla27/GnuplotEditor/archive/refs/heads/master.zip";

UpdateManager::UpdateManager(QWidget *parent)
    : QDialog(parent)
    , urlLineEdit(new QLineEdit(downloadUrl, this))
    , versionLineEdit(new QLineEdit("", this))
    , messageLabel(new QLabel("", this))
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QHBoxLayout *urlLayout = new QHBoxLayout;
    QHBoxLayout *versionLayout = new QHBoxLayout;
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    setLayout(vLayout);
    vLayout->addLayout(urlLayout);
    vLayout->addLayout(versionLayout);
    vLayout->addItem(spacer);

    QLabel *urlLabel = new QLabel("Url", this);
    QLabel *versionLabel = new QLabel("Version", this);

    urlLayout->addWidget(urlLabel);
    urlLayout->addWidget(urlLineEdit);
    versionLayout->addWidget(versionLabel);
    versionLayout->addWidget(versionLineEdit);

    const int label_width = 100;
    urlLabel->setFixedWidth(label_width);
    versionLabel->setFixedWidth(label_width);

    urlLineEdit->setReadOnly(true);
    versionLineEdit->setReadOnly(true);
}

UpdateManager::~UpdateManager()
{

}

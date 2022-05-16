#include "templatecustomwidget.h"
#include <QHBoxLayout>

TemplateCustomWidget::TemplateCustomWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlag(Qt::WindowType::Window, true);

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    QVBoxLayout *fileListLayout = new QVBoxLayout;
    QVBoxLayout *displayLayout = new QVBoxLayout;

    setLayout(hLayout);
    hLayout->addLayout(fileListLayout);
    hLayout->addLayout(displayLayout);
}

#include "iteminfodialog.h"

#include <QLayout>
#include <QJsonValue>
#include <QLineEdit>

ItemInfoDialog::ItemInfoDialog()
{
}

void ItemInfoDialog::setData(const QJsonObject &object)
{
    auto formLayout = new QVBoxLayout(this);
    for (auto it = object.constBegin(); it != object.constEnd(); it++) {
        auto lineLayout = new QHBoxLayout(this);

        auto keyEdit = new QLineEdit(this);
        keyEdit->setText(it.key());
        keyEdit->setReadOnly(true);
        keyEdit->setCursorPosition(0);
        keyEdit->installEventFilter(this);
        lineLayout->addWidget(keyEdit, 3);

        auto valueEdit = new QLineEdit(this);
        valueEdit->setText(it.value().toVariant().toString());
        valueEdit->setReadOnly(true);
        valueEdit->setCursorPosition(0);
        valueEdit->installEventFilter(this);
        lineLayout->addWidget(valueEdit, 2);

        formLayout->addLayout(lineLayout);
    }

    QWidget *viewport = new QWidget(this);
    viewport->setLayout(formLayout);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(viewport);
    scrollArea->setWidgetResizable(true);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(scrollArea);

    setLayout(layout);
}

bool ItemInfoDialog::eventFilter(QObject *o, QEvent *e)
{
    QLineEdit *l = qobject_cast<QLineEdit*>(o);
    if (!l) {
        return QObject::eventFilter(o, e);
    }
    if (e->type() == QEvent::MouseButtonPress) {
        l->selectAll();
        return true;
    }
    return QObject::eventFilter(o, e);
}

#include "PaintedWidget.hpp"

#include <QDebug>
#include <QPainter>

PaintedWidget::PaintedWidget(QWidget *parent)
    : QWidget(parent)
{
    QPixmap image;
    image.load("dump.png");
    if (image.isNull()) {
        setFixedSize(640, 480);
    } else {
        setFixedSize(image.size().width() / 2, image.size().height() / 2);
    }
}

void PaintedWidget::setImage(const QString &path, bool force)
{
    QPixmap image;
    image.load("dump.png");
    setFixedSize(image.size().width() / 2, image.size().height() / 2);

    if (m_image != path) {
        m_image = path;
    }

    if (m_image != path || force) {
        update();
    }
}

void PaintedWidget::setItemRect(const QRect &rect)
{
    if (m_itemRect == rect) {
        return;
    }

    m_itemRect = rect;
    update();
}

void PaintedWidget::setClickRect(const QRect &rect)
{
    if (m_clickRect == rect) {
        return;
    }

    m_clickRect = rect;
    update();
}

void PaintedWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QPixmap image;
    image.load("dump.png");
    image = image.scaled(image.width() / 2, image.height() / 2, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    painter.drawPixmap(0, 0, image);
}

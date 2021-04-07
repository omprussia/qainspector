// Copyright (c) 2019-2020 Open Mobile Platform LLC.
#include "PaintedWidget.hpp"

#include <QDebug>
#include <QPaintEvent>
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
    resize(image.size().width() / 2, image.size().height() / 2);

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

void PaintedWidget::setClickPoint(const QPointF &point)
{
    qDebug() << Q_FUNC_INFO << point;

    if (m_scaledPoint == point) {
        return;
    }
    m_scaledPoint = point;
    QPointF scaledPoint(point.x() / m_ratio, point.y() / m_ratio);
    m_clickPoint = scaledPoint;
    update();
}

QPointF PaintedWidget::scaledClickPoint()
{
    return m_clickPoint;
}

void PaintedWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    QPixmap image;
    image.load("dump.png");
    m_ratio = e->rect().width() / (float)image.width();
    m_scaledPoint.setX(m_clickPoint.x() * m_ratio);
    m_scaledPoint.setY(m_clickPoint.y() * m_ratio);
//    image = image.scaled(image.width() / 2, image.height() / 2, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    image = image.scaled(e->rect().width(), image.height() * m_ratio, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    painter.drawPixmap(0, 0, image);

    QFont font = painter.font();
    font.setPixelSize(8);
    painter.setFont(font);

    if (!m_itemRect.isNull()) {
        QRectF itemRect(m_itemRect.x() * m_ratio, m_itemRect.y() * m_ratio, m_itemRect.width() * m_ratio, m_itemRect.height() * m_ratio);

        // draw rect frame
        painter.setOpacity(0.4);

        QPen pen;
        pen.setWidth(1);
        pen.setColor(Qt::white);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(itemRect);

        itemRect.adjust(1, 1, 0, 0);

        QString text = QStringLiteral(" %1:%2 %3:%4 ").arg(m_itemRect.x()).arg(m_itemRect.y()).arg(m_itemRect.width()).arg(m_itemRect.height());
        QSize fontSize = painter.fontMetrics().size(Qt::TextSingleLine, text);
        const QRectF textRect{itemRect.topLeft(), QSizeF(fontSize)};

        // draw rect background
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::black);
        painter.drawRect(textRect);

        // draw rect text
        painter.setOpacity(1.0);

        painter.setPen(Qt::white);
        painter.drawText(textRect, Qt::AlignCenter, text);
    }

    if (!m_clickPoint.isNull()) {
        QString text = QStringLiteral(" %1:%2 ").arg((int)(m_clickPoint.x()))
                                                .arg((int)(m_clickPoint.y()));
        QSize fontSize = painter.fontMetrics().size(Qt::TextSingleLine, text);
        const QRectF textRect{m_scaledPoint, QSizeF(fontSize)};

        // draw click frame
        painter.setOpacity(0.6);

        QPen pen;
        pen.setWidth(2);
        pen.setColor(Qt::lightGray);
        painter.setPen(pen);
        painter.setBrush(Qt::black);
        painter.drawRect(textRect);

        // draw click text
        painter.setOpacity(1.0);

        painter.setPen(Qt::white);
        painter.drawText(textRect, Qt::AlignCenter, text);

        // draw click point
        pen.setColor(Qt::red);
        painter.setPen(pen);
        painter.drawEllipse(m_scaledPoint, 1, 1);
    }
}

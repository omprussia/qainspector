// Copyright (c) 2019-2020 Open Mobile Platform LLC.
#pragma once

#include <QWidget>

class PaintedWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PaintedWidget(QWidget *parent = nullptr);

public slots:
    void setImage(const QString &path, bool force = false);
    void setItemRect(const QRect &rect);
    void setClickPoint(const QPointF &point);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_image;
    QRect m_itemRect;
    QPointF m_clickPoint;
};


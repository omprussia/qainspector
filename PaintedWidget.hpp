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
    void setClickRect(const QRect &rect);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_image;
    QRect m_itemRect;
    QRect m_clickRect;
};


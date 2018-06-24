#ifndef DBUSCONNECTOR_H
#define DBUSCONNECTOR_H

#include <QObject>

class DBusConnector : public QObject
{
    Q_OBJECT
public:
    explicit DBusConnector(QObject *parent = nullptr);

signals:

public slots:
    void init();
};

#endif // DBUSCONNECTOR_H

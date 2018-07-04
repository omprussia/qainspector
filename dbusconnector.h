#ifndef DBUSCONNECTOR_H
#define DBUSCONNECTOR_H

#include <QDBusConnection>
#include <QObject>
#include <QJsonObject>
#include <QJSValue>

class DBusConnector : public QObject
{
    Q_OBJECT
public:
    explicit DBusConnector(QObject *parent = nullptr);

    Q_PROPERTY(bool connected READ isConnected WRITE setConnected NOTIFY connectedChanged)
    bool isConnected() const;
    void setConnected(bool connected);

    Q_PROPERTY(QString hostname MEMBER m_hostName NOTIFY hostnameChanged)
    Q_PROPERTY(QString port MEMBER m_hostPort NOTIFY portChanged)
    Q_PROPERTY(QString applicationName MEMBER m_applicationName NOTIFY applicationNameChanged)

public slots:
    void getDumpPage(QJSValue callback);
    void getDumpTree(QJSValue callback);

    void getGrabWindow(QJSValue callback);

signals:
    void connectedChanged(bool connected);
    void hostnameChanged();
    void portChanged();
    void applicationNameChanged();

private:
    QDBusConnection m_connection;
    QString m_hostName;
    QString m_hostPort;
    QString m_applicationName;
};

#endif // DBUSCONNECTOR_H

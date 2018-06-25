#include "dbusconnector.h"

#include <QDBusConnection>
#include <QTimer>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QFile>

#include <QDebug>

static const QString c_connectionAddress = QStringLiteral("tcp:host=%1,port=%2");
static const QString c_connectionName = QStringLiteral("network_bus");
static const QString c_serviceName = QStringLiteral("ru.omprussia.qaservice.%1");

DBusConnector::DBusConnector(QObject *parent)
    : QObject(parent)
    , m_connection(c_connectionName)
{

}

bool DBusConnector::isConnected() const
{
    return m_connection.isConnected();
}

void DBusConnector::setConnected(bool connected)
{
    const bool lastConnected = m_connection.isConnected();

    if (!connected && lastConnected) {
        m_connection.disconnectFromBus(c_connectionName);
    } else if (connected && !lastConnected) {
        m_connection = QDBusConnection::connectToBus(c_connectionAddress.arg(m_hostName, m_hostPort), c_connectionName);
    }

    qDebug() << Q_FUNC_INFO << m_hostName << m_hostPort;
    qDebug() << Q_FUNC_INFO << connected << m_connection.isConnected();

    if (lastConnected != m_connection.isConnected()) {
        emit connectedChanged(m_connection.isConnected());
    }
}

QJsonObject DBusConnector::getDumpTree()
{

}

void DBusConnector::getDumpPage(QJSValue callback)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(c_serviceName.arg(m_applicationName),
                                                      QStringLiteral("/ru/omprussia/qaservice"),
                                                      QStringLiteral("ru.omprussia.qaservice"),
                                                      QStringLiteral("dumpCurrentPage"));
    QDBusPendingCall async = m_connection.asyncCall(msg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, [callback](QDBusPendingCallWatcher *watcher) mutable {
        watcher->deleteLater();
        const QString dump = QDBusPendingReply<QString>(*watcher);
        if (callback.isCallable()) {
            callback.call({ QJSValue(dump) });
        } else {
            qDebug() << Q_FUNC_INFO << dump;
        }
    });
}

void DBusConnector::getGrabWindow(QJSValue callback)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(c_serviceName.arg(m_applicationName),
                                                      QStringLiteral("/ru/omprussia/qaservice"),
                                                      QStringLiteral("ru.omprussia.qaservice"),
                                                      QStringLiteral("grabWindow"));
    QDBusPendingCall async = m_connection.asyncCall(msg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, [callback](QDBusPendingCallWatcher *watcher) mutable {
        watcher->deleteLater();
        const QByteArray dump = QDBusPendingReply<QByteArray>(*watcher);

        QFile file("dump.png");
        if (file.open(QFile::WriteOnly)) {
            qDebug() << Q_FUNC_INFO << file.write(dump);
            file.close();
        }

        if (callback.isCallable()) {
            callback.call({ QJSValue(!dump.isEmpty()) });
        } else {
            qDebug() << Q_FUNC_INFO << !dump.isEmpty();
        }
    });
}

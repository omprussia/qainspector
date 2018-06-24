#include "dbusconnector.h"

#include <QDBusConnection>
#include <QTimer>
#include <QDebug>

DBusConnector::DBusConnector(QObject *parent) : QObject(parent)
{

}

void DBusConnector::init()
{
    QTimer::singleShot(1000, this, [this](){
        QDBusConnection conn = QDBusConnection::connectToBus("tcp:host=192.168.2.15,port=55557", "net_bus");
        qDebug() << "Bus:" << conn.isConnected();
        if (!conn.isConnected()) {
            QTimer::singleShot(0, this, &DBusConnector::init);
        }
    });
}

#ifndef SOCKETCONNECTOR_H
#define SOCKETCONNECTOR_H

#include <QObject>
#include <QJsonObject>
#include <QJSValue>

class QTcpSocket;
class SocketConnector : public QObject
{
    Q_OBJECT
public:
    explicit SocketConnector(QObject *parent = nullptr);

    Q_PROPERTY(bool connected READ isConnected WRITE setConnected NOTIFY connectedChanged)
    bool isConnected() const;
    void setConnected(bool connected);

    Q_PROPERTY(QString hostname MEMBER m_hostName NOTIFY hostnameChanged)
    Q_PROPERTY(QString port MEMBER m_hostPort NOTIFY portChanged)
    Q_PROPERTY(QString applicationName MEMBER m_applicationName NOTIFY applicationNameChanged)

public slots:
    void getDumpPage(QJSValue callback);
    void getDumpTree(QJSValue callback);
    void getDumpCover(QJSValue callback);

    void getGrabWindow(QJSValue callback);
    void getGrabCover(QJSValue callback);

    void findObject(int pointx, int pointy);

signals:
    void connectedChanged(bool connected);
    void hostnameChanged();
    void portChanged();
    void applicationNameChanged();

private:
    QTcpSocket* m_socket;
    QString m_hostName;
    QString m_hostPort;
    QString m_applicationName;
};

#endif // SOCKETCONNECTOR_H

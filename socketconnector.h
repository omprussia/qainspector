#ifndef SOCKETCONNECTOR_H
#define SOCKETCONNECTOR_H

#include <QObject>
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
    QString getDumpPage();
    void getDumpPage(QJSValue callback);

    QString getDumpTree();
    void getDumpTree(QJSValue callback);

    QString getDumpCover();
    void getDumpCover(QJSValue callback);

    bool getGrabWindow();
    void getGrabWindow(QJSValue callback);

    bool getGrabCover();
    void getGrabCover(QJSValue callback);

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

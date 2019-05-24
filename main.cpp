#include "mytreemodel.h"
#include "socketconnector.h"

#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QDebug>
#include <QTimer>

int main(int argc, char *argv[])
{
    qmlRegisterType<SocketConnector>("ru.omprussia.qainspector", 1, 0, "SocketConnection");
    qmlRegisterType<MyTreeModel>("ru.omprussia.qainspector", 1, 0, "ItemsTreeModes");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    QFileInfo png(QStringLiteral("dump.png"));
    engine.rootContext()->setContextProperty(QStringLiteral("myImage"), QUrl::fromLocalFile(png.absoluteFilePath()));

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

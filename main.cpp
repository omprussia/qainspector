#include "mytreemodel.h"

#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QDebug>
#include <QTimer>
#include "dbusconnector.h"

int main(int argc, char *argv[])
{
    qmlRegisterType<DBusConnector>("ru.omprussia.qainspector", 1, 0, "RemoteConnection");
    qmlRegisterType<MyTreeModel>("ru.omprussia.qainspector", 1, 0, "ItemsTreeModes");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    QFileInfo png("dump.png");
    engine.rootContext()->setContextProperty("myImage", QUrl::fromLocalFile(png.absoluteFilePath()));

    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

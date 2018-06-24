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
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    QFile dump("dump.json");
    if (dump.open(QFile::ReadOnly)) {
        const QByteArray data = dump.readAll();
        dump.close();

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);
        qDebug() << error.errorString();
        MyTreeModel *model = new MyTreeModel(doc.object(), &app);
        engine.rootContext()->setContextProperty("myModel", model);
    } else {
        qWarning() << "No file!";
    }

    QFileInfo png("dump.png");
    if (png.exists()) {
        engine.rootContext()->setContextProperty("myImage", QUrl::fromLocalFile(png.absoluteFilePath()));
    }

    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    DBusConnector conn(&app);
    QTimer::singleShot(0, &conn, &DBusConnector::init);

    return app.exec();
}

// Copyright (c) 2020 Open Mobile Platform LLС.
#include "mytreemodel.h"
#include "socketconnector.h"
#include "treeviewdialog.h"

#include <QApplication>

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
    QScopedPointer<QCoreApplication> app;

    if (argc == 2 && strcmp(argv[1], "--widgets") == 0) {
        app.reset(new QApplication(argc, argv));
        TreeViewDialog *w = new TreeViewDialog;
        w->show();

        QTimer::singleShot(0, w, &TreeViewDialog::init);
    } else {
        qmlRegisterType<SocketConnector>("ru.omprussia.qainspector", 1, 0, "SocketConnection");
        qmlRegisterType<MyTreeModel>("ru.omprussia.qainspector", 1, 0, "ItemsTreeModes");

        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        app.reset(new QGuiApplication(argc, argv));

        QQmlApplicationEngine *engine = new QQmlApplicationEngine;

        QFileInfo png(QStringLiteral("dump.png"));
        engine->rootContext()->setContextProperty(QStringLiteral("myImage"), QUrl::fromLocalFile(png.absoluteFilePath()));

        QTimer::singleShot(0, app.data(), [engine]() {
            engine->load(QUrl(QStringLiteral("qrc:/main.qml")));
        });
    }

    app->setOrganizationDomain("ru.omprussia");
    app->setOrganizationName("Open Mobile Platform");
    app->setApplicationName("qainspector");
    return app->exec();
}

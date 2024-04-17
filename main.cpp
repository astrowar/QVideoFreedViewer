#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "videoclient.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    //register the VideoClient class to use it in qml 
    qmlRegisterType<VideoClient>("VideoClient", 1, 0, "VideoClient");
    qmlRegisterType<ImageDisplay>("ImageDisplay", 1, 0, "ImageDisplay");


    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

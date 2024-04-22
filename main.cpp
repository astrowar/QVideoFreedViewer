#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "videoclient.h"
#include "sensorclient.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    
    //register the VideoClient class to use it in qml 
    qmlRegisterType<VideoClient>("VideoClientModule", 1, 0, "VideoClient");
    qmlRegisterType<ImageDisplay>("VideoClientModule", 1, 0, "ImageDisplay");
    qmlRegisterType<GraphicDisplay>("VideoClientModule", 1, 0, "GraphicDisplay");
    qmlRegisterType<SensorClient>("VideoClientModule", 1, 0, "SensorClient");


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

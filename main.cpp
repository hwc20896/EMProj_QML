#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQmlContext>
#include <iostream>
#include "managementbackend.hpp"

int main(int argc, char** argv) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    auto backend = new ManagementBackend(&engine);
    QQmlEngine::setObjectOwnership(backend, QQmlEngine::CppOwnership);
    engine.rootContext()->setContextProperty("backend", backend);

    engine.load(QUrl("qrc:/qml/src/ui/main.qml"));
    const auto topLevel = engine.rootObjects().value(0);
    if (const auto window = qobject_cast<QQuickWindow*>(topLevel)) {
        window->show();
        return app.exec();
    }
    std::cout << "Unable to create window\n";
    return -1;
}
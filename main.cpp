#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQmlContext>
#include <print>
#include "managementbackend.hpp"
#include "soundmanager.hpp"

int main(int argc, char** argv) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    auto backend = new ManagementBackend(&engine);
    QQmlEngine::setObjectOwnership(backend, QQmlEngine::CppOwnership);
    engine.rootContext()->setContextProperty("backend", backend);

    // 注册SoundManager到QML上下文
    auto& soundManager = SoundManager::instance();
    QQmlEngine::setObjectOwnership(&soundManager, QQmlEngine::CppOwnership);
    engine.rootContext()->setContextProperty("soundManager", &soundManager);

    engine.load(QUrl("qrc:/qml/src/ui/main.qml"));
    const auto topLevel = engine.rootObjects().value(0);
    if (const auto window = qobject_cast<QQuickWindow*>(topLevel)) {
        window->show();
        return app.exec();
    }
    std::println("Unable to create window.");
    return -1;
}
import QtQuick.Controls.Fusion
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow{
    id: root
    minimumWidth: 1100
    minimumHeight: 750

    property int currentGamemode: 0

    readonly property Component introPageComponent: Qt.createComponent("Intro.qml")
    readonly property Component gamePageComponent: Qt.createComponent("Game.qml")
    readonly property Component outroPageComponent: Qt.createComponent("Outro.qml")

    StackView{
        id: stackView
        anchors.fill: parent
        initialItem: introPageComponent.createObject()

        pushEnter: Transition { XAnimator { duration: 30 } }
        pushExit: Transition { XAnimator { duration: 30 } }
    }

    function getGame(){
        return gamePageComponent.createObject(stackView)
    }

    function getOutro(){
        return outroPageComponent.createObject(stackView)
    }
}

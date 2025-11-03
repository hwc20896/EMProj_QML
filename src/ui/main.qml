import QtQuick.Controls.Fusion
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow{
    id: root
    minimumWidth: 1000
    minimumHeight: 700

    property var currentGamemode: 0

    StackView{
        id: stackView
        anchors.fill: parent
        initialItem: introPageComponent

        pushEnter: Transition { XAnimator { duration: 30 } }
        pushExit: Transition { XAnimator { duration: 30 } }
    }

    Component{
        id: introPageComponent
        Intro{
            onStartMain: gamemode => {
                console.log("The game should be started now.");
                currentGamemode = gamemode;
                stackView.push(gamePageComponent);
            }
        }
    }

    Component{
        id: gamePageComponent
        Game{}
    }
    /*
    Component{
        id: outroPageComponent
    }
    */
}

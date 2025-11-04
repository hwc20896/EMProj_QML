import QtQuick
import QtQuick.Controls

Button{
    id: muteSwitchButton
    implicitHeight: 70
    implicitWidth: 70

    property size iconSize: Qt.size(70,70)

    readonly property string mutedIcon: "qrc:/images/drawables/mute.png"
    readonly property string unmutedIcon: "qrc:/images/drawables/unmute.png"

    flat: true
    icon{
        source: backend.isMuted? mutedIcon: unmutedIcon
        width: iconSize.width
        height: iconSize.height
        color: "transparent"
    }
    onClicked: {
        backend.isMuted = !backend.isMuted;
    }

    background: Rectangle{
        color: "white"
        radius: 5
    }
}

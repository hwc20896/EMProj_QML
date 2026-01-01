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
    icon.source: soundManager.isMuted? mutedIcon: unmutedIcon
    icon.width: iconSize.width
    icon.height: iconSize.height
    icon.color: "transparent"
    onClicked: {
        soundManager.isMuted = !soundManager.isMuted;
    }

    background: Rectangle{
        color: {
            if (parent.pressed) return "#e6e6e6";
            if (parent.hovered) return "#f0f0f0";
            return "#fff";
        }
        radius: 5
        border.color: "#bdbdbd"
        border.width: 1
    }
}

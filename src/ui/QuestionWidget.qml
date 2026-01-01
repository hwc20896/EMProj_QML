import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout{
    id: questionRoot

    //  Examples
    property string title: ""
    property var optionText: []
    property string descriptionText: ""

    signal popOption(string text)

    spacing: 30

    Text{
        id: questionTitle
        Layout.topMargin: gamePageRoot.height * 0.01
        Layout.leftMargin: gamePageRoot.width * 0.05
        Layout.rightMargin: gamePageRoot.width * 0.05

        text: `${backend.currentQuestionIndex + 1}. ${title}`
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        Layout.fillWidth: true

        font.pointSize: 25
        wrapMode: Text.WordWrap
    }

    GridLayout {
        id: grid
        columns: 2
        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: 30
        columnSpacing: 20
        rowSpacing: 20

        Repeater {
            id: optionButtons
            model: optionText

            delegate: Button {
                id: btn
                property string optionText: modelData
                property bool isWrong: false

                text: modelData
                enabled: !isWrong && !answerRevealed
                Layout.preferredWidth: 300
                Layout.preferredHeight: 120
                font.pointSize: 16

                background: Rectangle {
                    radius: 8
                    border.width: 2
                    border.color: {
                        if (!answerRevealed) return "#9E9E9E";
                        if (btn.text === correctOption) return "#2E7D32";
                        if (btn.text === selectedOption) return "#B71C1C";
                        return "#9E9E9E";
                    }

                    // 核心：动态背景色
                    color: {
                        if (!answerRevealed) {
                            if (parent.pressed) return "#e6e6e6";
                            if (parent.hovered) return "#f0f0f0";
                            return "#fff";
                        }

                        // 答案揭示状态
                        if (text === correctOption) {
                            return "#4CAF50"  // 正确答案绿色
                        }
                        if (text === selectedOption) {
                            return "#F44336"  // 用户选择的错误答案红色
                        }
                        return "#F5F5F5" // 未选中的选项
                    }
                }

                onClicked: {
                    if (isInReviveMode) {
                        handleReviveAnswer(text);
                    } else {
                        selectedOption = text;
                        backend.handleAnswer(text);
                    }
                }

                Rectangle {
                    id: strike
                    anchors.fill: parent
                    visible: btn.isWrong && btn.opacity > 0
                    opacity: 0.0
                    property real margin: 6
                    color: "transparent"
                    // 使用旋转的Rectangle作为划线
                    transform: Rotation {
                        origin.x: strike.width / 2
                        origin.y: strike.height / 2
                        angle: (Math.random() >= 0.5 ? 1 : -1) * 20 // 20度角
                    }

                    Rectangle {
                        anchors.centerIn: parent
                        width: parent.width * 1.1  // 确保线条足够长
                        height: 6  // 线条宽度
                        color: "#ffaa00"
                    }

                    NumberAnimation on opacity {
                        id: strikeFade
                        from: 0.0 
                        to: 1.0
                        duration: 200
                        easing.type: Easing.InOutQuad
                        running: false

                        onStarted: {
                            strike.visible = true;
                        }

                        onStopped: {
                            if (btn.isWrong) {
                                fadeOutTimer.start();
                            }
                        }
                    }
                }

                Timer {
                    id: fadeOutTimer
                    interval: 50
                    onTriggered: fadeOut.start()
                }

                PropertyAnimation {
                    id: fadeOut
                    target: btn
                    property: "opacity"
                    from: 1.0
                    to: 0.0
                    duration: 400
                    easing.type: Easing.InOutQuad
                    onStopped: {
                        btn.enabled = false; // ✅ 安全關閉點擊
                        // 隐藏Shape以防止后续绘制问题
                        strike.visible = false;
                    }
                }

                function markWrong() {
                    if (isWrong) return;
                    isWrong = true;
                    soundManager.playSwoon();
                    strikeFade.start();
                }
            }
        }
    }

    // 🔔 從 C++ 傳入錯誤選項名稱
    onPopOption: function (text) {
        for (let i = 0; i < optionButtons.count; ++i) {
            const btn = optionButtons.itemAt(i);
            if (btn.optionText === text) {
                btn.markWrong();
                break;
            }
        }
    }

    Text{
        id: description
        Layout.alignment: Qt.AlignCenter

        visible: descriptionText.length !== 0
        text: {
            if (answerRevealed && descriptionText.length !== 0) return `解析：${descriptionText}`;
            return "";
        }
        font.pointSize: 20
        font.bold: true
        color: "#ffffff"
    }
}
